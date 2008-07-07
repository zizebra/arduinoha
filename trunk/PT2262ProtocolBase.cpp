#include "PT2262ProtocolBase.h"
#include "WConstants.h"

// Up to 12 Tri-State Code Address Pins
// Up to 6 Data Pins

PT2262ProtocolBase::PT2262ProtocolBase(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	unsigned int _ShortPulseDuration_Min, unsigned int _ShortPulseDuration_Max,
	unsigned int _LongPulseDuration_Min, unsigned int _LongPulseDuration_Max,
	unsigned int _TerminatorDuration_Min , unsigned int _TerminatorDuration_Max ) : TerminatedProtocolBase(id, 24, 50, Bitstream)
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
    if (HIGH==pulse)
    { // een hoog signaal
        if (0==BitDecodeState && between(duration, ShortPulseDuration_Min, ShortPulseDuration_Max) ) BitDecodeState=1;
        else if (0==BitDecodeState && between(duration, LongPulseDuration_Min, LongPulseDuration_Max ) ) BitDecodeState=2;
	else
	{
	   BitDecodeState = 0;
	   DecodedBitsBufferPosIdx = 0;
       }
    }
    else
    { // curstate==LOW
	if (2==BitDecodeState && between(duration, ShortPulseDuration_Min, ShortPulseDuration_Max) )
	{
	      StoreDecodedBit(1);
              BitDecodeState = 0;
        } else if (1==BitDecodeState && between(duration, LongPulseDuration_Min, LongPulseDuration_Max) )
        {
	      StoreDecodedBit(0);
              BitDecodeState = 0;
        } else 
	{
		if (1==BitDecodeState && between(duration, TerminatorDuration_Min, TerminatorDuration_Max ) )
		{
            		Terminator();
		}
            BitDecodeState = 0;
	    DecodedBitsBufferPosIdx = 0;
        } 
    }
}
