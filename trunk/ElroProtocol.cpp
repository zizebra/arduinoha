#include "ElroProtocol.h"
#include "WConstants.h"

enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1, 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

#define	ShortPulseDuration_Min 10
#define	ShortPulseDuration_Max 30

#define	LongPulseDuration_Min 50
#define	LongPulseDuration_Max 70

#define	TerminatorDuration_Min 600
#define	TerminatorDuration_Max 640



ElroProtocol::ElroProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*DeviceCommand)(unsigned short int &, bool &) ,
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_DeviceCommand = DeviceCommand;
	_debug = debug;

	DecodedBitsBufferSize = 24;
}

void ElroProtocol::DecodeBitstream()
{	if ((DecodedBitsBuffer[0]!=0) || (DecodedBitsBuffer[2]!=0) || (DecodedBitsBuffer[4]!=0) || (DecodedBitsBuffer[6]!=0) || 
		(DecodedBitsBuffer[8]!=0) || (DecodedBitsBuffer[10]!=0) || (DecodedBitsBuffer[12]!=0) || (DecodedBitsBuffer[14]!=0) || 
		(DecodedBitsBuffer[16]!=0) || (DecodedBitsBuffer[18]!=0) || (DecodedBitsBuffer[20]!=0) || (DecodedBitsBuffer[22]!=0) ) return;
	if ((DecodedBitsBuffer[19]!=1) || (DecodedBitsBuffer[21]!=1)) return;

	unsigned short int device = (DecodedBitsBuffer[9] * 1) + (DecodedBitsBuffer[11] * 2) + (DecodedBitsBuffer[13] * 4) + 
					(DecodedBitsBuffer[15] * 8) + (DecodedBitsBuffer[17] * 16) ;

	bool command = DecodedBitsBuffer[23];
	if (_DeviceCommand!=0) _DeviceCommand(device, command);
}

void ElroProtocol::DecodePulse(short int pulse, unsigned int duration)
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
           else 
	   {
		ResetBitDecodeState();
	   }
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=2;
           else
	   {
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
            if (2==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
              ResetBitDecodeState();
            } else 
            {
	      ResetDecodedBitsBuffer();
              ResetBitDecodeState();
            }
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) 
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
