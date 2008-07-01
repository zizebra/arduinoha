#include "ShortHighProtocolBase.h"
#include "WConstants.h"

//

enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

ShortHighProtocolBase::ShortHighProtocolBase(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*debug)(const char *) ,
	unsigned int _ShortHighPulseDuration_Min, unsigned int _ShortHighPulseDuration_Max,
	unsigned int _LongHighPulseDuration_Min, unsigned int _LongHighPulseDuration_Max,
	unsigned int _ShortLowPulseDuration_Min, unsigned int _ShortLowPulseDuration_Max,
	unsigned int _LongLowPulseDuration_Min, unsigned int _LongLowPulseDuration_Max,
	unsigned int _TerminatorDuration_Min , unsigned int _TerminatorDuration_Max ) : TerminatedProtocolBase(id, 24, 26, Bitstream, debug)
{
	ShortHighPulseDuration_Min = _ShortHighPulseDuration_Min;
	ShortHighPulseDuration_Max = _ShortHighPulseDuration_Max;

	LongHighPulseDuration_Min = _LongHighPulseDuration_Min;
	LongHighPulseDuration_Max = _LongHighPulseDuration_Max;

	ShortLowPulseDuration_Min = _ShortLowPulseDuration_Min;
	ShortLowPulseDuration_Max = _ShortLowPulseDuration_Max;

	LongLowPulseDuration_Min = _LongLowPulseDuration_Min;
	LongLowPulseDuration_Max = _LongLowPulseDuration_Max;

	TerminatorDuration_Min = _TerminatorDuration_Min;
	TerminatorDuration_Max = _TerminatorDuration_Max;
}


void ShortHighProtocolBase::DecodePulse(short int pulse, unsigned int duration)
{
    unsigned int durationresult = DURATION_UNKNOWN ; 
    if (HIGH==pulse)
    { // een hoog signaal
	durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortHighPulseDuration_Min, ShortHighPulseDuration_Max, 
					DURATION_LONG , LongHighPulseDuration_Min, LongHighPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );
       switch (durationresult)
       {
         case DURATION_SHORT:
           if (0==BitDecodeState) BitDecodeState=1; else BitDecodeState = 0;
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=2; else BitDecodeState = 0;
           break;
         default:
	   BitDecodeState = 0;
	   DecodedBitsBufferPosIdx = 0;
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
            if (2==BitDecodeState) StoreDecodedBit(1); else DecodedBitsBufferPosIdx = 0;
            BitDecodeState = 0;
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) StoreDecodedBit(0); else DecodedBitsBufferPosIdx = 0;
            BitDecodeState = 0;
            break;  
          case DURATION_TERMINATOR :
            if (1==BitDecodeState) Terminator();
            BitDecodeState = 0;
	    DecodedBitsBufferPosIdx = 0;
            break;
          default: 
            BitDecodeState = 0;
	    DecodedBitsBufferPosIdx = 0;
            break;  
        }
    }
}
