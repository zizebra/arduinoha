#include "RanexProtocol.h"
#include <stdlib.h>
#include "WConstants.h"

// HS2262A-R4

// Define the different pulses
#define	DURATION_UNKNOWN 0
#define DURATION_SHORT 1
#define	DURATION_LONG 2 
#define DURATION_TERMINATOR 3

// Define the different pulse timings
#define	ShortPulseDuration_Min 5
#define	ShortPulseDuration_Max 20

#define	LongPulseDuration_Min 30
#define	LongPulseDuration_Max 75

#define	TerminatorDuration_Min 430
#define	TerminatorDuration_Max 460

RanexProtocol::RanexProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*DeviceCommand)(char * , unsigned short int &, bool &) ,
	void (*debug)(const char *) )  : TerminatedProtocolBase(id, 12, 12 * 4 + 2 , Bitstream, debug)
{
	_DeviceCommand = DeviceCommand;
}

void RanexProtocol::DecodeBitstream()
{
	unsigned short int device = (!DecodedBitsBuffer[1] ) + (!DecodedBitsBuffer[2] << 1) + (!DecodedBitsBuffer[3] << 2);
	bool command = DecodedBitsBuffer[8];
	if (_DeviceCommand!=0) _DeviceCommand(_id, device, command);
}

void RanexProtocol::EncodePulse(unsigned short int pulse)
{
	if (DURATION_SHORT==pulse) 
			StoreEncodedPulse( 10 ); 
	else if (DURATION_LONG==pulse)
			StoreEncodedPulse( 47 ); 
	else if (DURATION_TERMINATOR==pulse)
			StoreEncodedPulse( 450 );
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
    int durationresult = DURATION_UNKNOWN; 

    if (HIGH==pulse)
    { // een hoog signaal
    	 durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max);
       switch (durationresult)
       {
         case DURATION_SHORT:
           if (0==BitDecodeState) BitDecodeState=1;
           else if (2==BitDecodeState) BitDecodeState=3;
           else 
	   {
		ResetDecodedBitsBuffer();
		BitDecodeState = 0;
	   }
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=11;
           else if (12==BitDecodeState) BitDecodeState=13;
           else if (2==BitDecodeState) BitDecodeState=23;
           else
	   {
 		 ResetDecodedBitsBuffer();
		 BitDecodeState = 0;
	   }
           break;
         default:
	   BitDecodeState = 0;
	   ResetDecodedBitsBuffer();
           break;
       }
    }
    else
    { // curstate==LOW
    	 durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );

        switch (durationresult)
        {
          case DURATION_SHORT:
            if (0==BitDecodeState) BitDecodeState = 31;
            else if (11==BitDecodeState) BitDecodeState = 12;
            else if (13==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
              BitDecodeState = 0;
            } else if (23==BitDecodeState)
            {
	      StoreDecodedBit(2);
              BitDecodeState = 0;
            } else
            {
	      ResetDecodedBitsBuffer();
              BitDecodeState = 0;
            }
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) BitDecodeState=2;
            else if (3==BitDecodeState)
            { // "0"
	      StoreDecodedBit(0);
              BitDecodeState = 0;
            } else 
	    {
		BitDecodeState = 0;
		ResetDecodedBitsBuffer();
	    }
            break;  
          case DURATION_TERMINATOR :	    
            if (1==BitDecodeState) Terminator();
            BitDecodeState = 0;
	    ResetDecodedBitsBuffer();
            break;
          default: 
            BitDecodeState = 0;
	    ResetDecodedBitsBuffer();
            break;  
        }
    }
}
