#include "RanexProtocol.h"
#include <stdlib.h>
#include "WConstants.h"

enum PulseDuration 
{
	DURATION_UNKNOWN = 0,
	DURATION_SHORT = 1,
	DURATION_LONG = 2 ,
	DURATION_TERMINATOR = 3
};

RanexProtocol::RanexProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*DeviceCommand)(unsigned short int &, bool &) ,
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_DeviceCommand = DeviceCommand;
	_debug = debug;

	ShortPulseDuration_Min = 5;
	ShortPulseDuration_Max = 20;

	LongPulseDuration_Min = 30;
	LongPulseDuration_Max = 75;

	TerminatorDuration_Min = 430;
	TerminatorDuration_Max = 460;

	DecodedBitsBufferSize = 12;
	EncodedBitsBufferSize = 12 * 4 + 2;
}

void RanexProtocol::DecodeBitstream()
{
	unsigned short int device = (!DecodedBitsBuffer[1] * 1 ) + (!DecodedBitsBuffer[2] * 2) + (!DecodedBitsBuffer[3] * 4);
	bool command = DecodedBitsBuffer[8];
	if (_DeviceCommand!=0) _DeviceCommand(device, command);
}

void RanexProtocol::StoreEncodedPulse(unsigned int duration)
{
	if (EncodedBitsBufferPosIdx < EncodedBitsBufferSize)
	{
		EncodedBitsBuffer[EncodedBitsBufferPosIdx] = duration;
	}
	EncodedBitsBufferPosIdx ++;
}

void RanexProtocol::EncodePulse(unsigned short int pulse)
{
	switch (pulse)
	{
		case DURATION_SHORT:
			StoreEncodedPulse( 10 ); 
			break;
		case DURATION_LONG:
			StoreEncodedPulse( 47 ); 
			break;
		case DURATION_TERMINATOR:
			StoreEncodedPulse( 450 );
			break;
	}
}

void RanexProtocol::EncodeBit(unsigned short bit)
{
	switch (bit)
	{
		case 0 :
			EncodePulse(DURATION_SHORT);
			EncodePulse(DURATION_LONG);
			EncodePulse(DURATION_SHORT);
			EncodePulse(DURATION_LONG);
			break;
		case 1 :
			EncodePulse(DURATION_LONG);
			EncodePulse(DURATION_SHORT);			
			EncodePulse(DURATION_LONG);
			EncodePulse(DURATION_SHORT);
			break;
	}
}

void RanexProtocol::EncodeTerminator()
{
	EncodePulse(DURATION_SHORT);
	EncodePulse(DURATION_TERMINATOR);
}

unsigned int * RanexProtocol::EncodeCommand(unsigned short int device, bool command)
{
	EncodedBitsBuffer = (unsigned int *)calloc( EncodedBitsBufferSize , sizeof(unsigned int) );
//	EncodedBitsBuffer = (unsigned int *)malloc( EncodedBitsBufferSize * sizeof(unsigned int) );
	ResetEncodedBitsBuffer();

	EncodeBit(0);
	if ((device & 1)!=0) EncodeBit(0); else EncodeBit(1);
	if ((device & 2)!=0) EncodeBit(0); else EncodeBit(1);
	if ((device & 4)!=0) EncodeBit(0); else EncodeBit(1);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	if (command) EncodeBit(1); else EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeTerminator();
	return EncodedBitsBuffer;
	
}

void RanexProtocol::DecodePulse(short int pulse,unsigned int duration)
{
    int durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );
    if (HIGH==pulse)
    { // een hoog signaal
       switch (durationresult)
       {
         case DURATION_SHORT:
           if (0==BitDecodeState) BitDecodeState=1;
           else if (2==BitDecodeState) BitDecodeState=3;
           else 
	   {
		ResetDecodedBitsBuffer();
		ResetBitDecodeState();
	   }
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=11;
           else if (12==BitDecodeState) BitDecodeState=13;
           else if (2==BitDecodeState) BitDecodeState=23;
           else
	   {
 		 ResetDecodedBitsBuffer();
		 ResetBitDecodeState();
	   }
           break;
         default:
	   ResetBitDecodeState();
	   ResetDecodedBitsBuffer();
           break;
       }
    }
    else
    { // curstate==LOW
        switch (durationresult)
        {
          case DURATION_SHORT:
            if (0==BitDecodeState) BitDecodeState = 31;
            else if (11==BitDecodeState) BitDecodeState = 12;
            else if (13==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
              ResetBitDecodeState();
            } else if (23==BitDecodeState)
            {
	      StoreDecodedBit(2);
              ResetBitDecodeState();
            } else
            {
	      ResetDecodedBitsBuffer();
              ResetBitDecodeState();
            }
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) BitDecodeState=2;
            else if (3==BitDecodeState)
            { // "0"
	      StoreDecodedBit(0);
              ResetBitDecodeState();
            } else 
	    {
		ResetBitDecodeState();
		ResetDecodedBitsBuffer();
	    }
            break;  
          case DURATION_TERMINATOR :	    
            if (1==BitDecodeState && DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)
            {
		if (_ProtocolBitstream!=0) _ProtocolBitstream(DecodedBitsBuffer);
		DecodeBitstream();			
            } 
            ResetBitDecodeState();
	    ResetDecodedBitsBuffer();
            break;
          default: 
            ResetBitDecodeState();
	    ResetDecodedBitsBuffer();
            break;  
        }
    }
}
