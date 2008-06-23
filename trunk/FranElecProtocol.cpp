#include "FranElecProtocol.h"
#include "WConstants.h"

// [  31]   89 [  84]   29 [  28]   88 [  87]   27 [  31]   88 [  28]   89 [  29]   91 [  84]   27 [  32]   87 [  29]   88 [  30]   88 [  29]   91 [  27]   89 [  29]   88 [  31]   87 [  30]   89 [  85]   32 [  82]   34 [  26]   88 [  30]   89 [  29]   88 [  30]   91 [  26]   92 [  26]   91 [  28]  925 


enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

#define	ShortPulseDuration_Min 20
#define	ShortPulseDuration_Max 37

#define	LongPulseDuration_Min 77
#define	LongPulseDuration_Max 98

#define	TerminatorDuration_Min 920
#define	TerminatorDuration_Max 940

FranElecProtocol::FranElecProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_debug = debug;


	DecodedBitsBufferSize = 24;
}

void FranElecProtocol::DecodeBitstream()
{}

void FranElecProtocol::DecodePulse(short int pulse, unsigned int duration)
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
