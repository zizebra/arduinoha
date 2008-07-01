#include "PT2262ProtocolBase.h"
#include "WConstants.h"

// Up to 12 Tri-State Code Address Pins
// Up to 6 Data Pins

enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

PT2262ProtocolBase::PT2262ProtocolBase(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*debug)(const char *) ,
	unsigned int _ShortPulseDuration_Min, unsigned int _ShortPulseDuration_Max,
	unsigned int _LongPulseDuration_Min, unsigned int _LongPulseDuration_Max,
	unsigned int _TerminatorDuration_Min , unsigned int _TerminatorDuration_Max ) : TerminatedProtocolBase(id, 24, 26, Bitstream, debug)
{
	ShortPulseDuration_Min = _ShortPulseDuration_Min;
	ShortPulseDuration_Max = _ShortPulseDuration_Max;

	LongPulseDuration_Min = _LongPulseDuration_Min;
	LongPulseDuration_Max = _LongPulseDuration_Max;

	TerminatorDuration_Min = _TerminatorDuration_Min;
	TerminatorDuration_Max = _TerminatorDuration_Max;
}


void PT2262ProtocolBase::DecodePulse(short int pulse, unsigned int duration)
{
    int durationresult = DURATION_UNKNOWN; 

    if (HIGH==pulse)
    { // een hoog signaal
	durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max );
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
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max, 
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
