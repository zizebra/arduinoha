#include "LaCrosseProtocol.h"
#include "WConstants.h"

// [  23]   93 [  68]   31 [  23]   93 [  68]   32 [  21]   95 [  21]   95 [  21]   95 [  66]   33 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  66]   33 [  66]   33 [  20]   97 [  19]   97 [  19]   97 [  19]   96 [  20]   96 [  20]   96 [  21]  969


enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

#define	ShortHighPulseDuration_Min 17
#define	ShortHighPulseDuration_Max 25

#define	ShortLowPulseDuration_Min 25
#define	ShortLowPulseDuration_Max 35

#define	LongHighPulseDuration_Min 60
#define	LongHighPulseDuration_Max 70

#define	LongLowPulseDuration_Min 90
#define	LongLowPulseDuration_Max 100


#define	TerminatorDuration_Min 950
#define	TerminatorDuration_Max 990



LaCrosseProtocol::LaCrosseProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*debug)(const char *) ) : TerminatedProtocolBase(id, 24, 26 , Bitstream, debug)
{
}

void LaCrosseProtocol::DecodeBitstream()
{}

void LaCrosseProtocol::DecodePulse(short int pulse, unsigned int duration)
{
    int durationresult = DURATION_UNKNOWN;
    if (HIGH==pulse)
    { // een hoog signaal
    	durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortHighPulseDuration_Min, ShortHighPulseDuration_Max, 
					DURATION_LONG , LongHighPulseDuration_Min, LongHighPulseDuration_Max );

       switch (durationresult)
       {
         case DURATION_SHORT:
           if (0==BitDecodeState) BitDecodeState=1;
           else 
	   {
		BitDecodeState = 0;
	   }
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=2;
           else
	   {
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
					DURATION_SHORT , ShortLowPulseDuration_Min, ShortLowPulseDuration_Max, 
					DURATION_LONG , LongLowPulseDuration_Min, LongLowPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );

        switch (durationresult)
        {
          case DURATION_SHORT:
            if (2==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
              BitDecodeState = 0;
            } else 
            {
	      ResetDecodedBitsBuffer();
              BitDecodeState = 0;
            }
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) 
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
            if (1==BitDecodeState && DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)
            {
		if (_ProtocolBitstream!=0) _ProtocolBitstream(_id , DecodedBitsBufferSize , DecodedBitsBuffer);
		DecodeBitstream();			
            } 
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
