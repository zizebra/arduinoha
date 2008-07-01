#include "UnknownProtocol.h"
#include "WConstants.h"

//[   7]   50 [  91]   56 [  88]   60 [  84]   62 [  35]   62 [  83]   63 [  33]   63 [  83]   64 [  81]   65 [  81]   64 [  81]   66 [  79]   67 [  30]   67 [  78]   68 [  78]   67 [  78]   69 [  77]   69 [  27]   69 [  28]   69 [  28]   67 [  78]   69 [  28]   69 [  27]   70 [  26]   70 [  76]   70 [  76]   70 [  26]   71 [  75]   70 [  76]   71 [  25]   71 [  26]   70 [  26]   71 [  75]   71 [  25]   71 [  26]   72 [  24]   71 [  75]   71 [  75]   72 [  24]   72 [  74]   72 [  74]   70 [  75]   73 [  24]   72 [  74] 1753 


#define DURATION_UNKNOWN 0 
#define DURATION_SHORT 1
#define DURATION_LONG  2 
#define DURATION_TERMINATOR 3

#define	ShortHighPulseDuration_Min 24
#define	ShortHighPulseDuration_Max 28

#define	LongHighPulseDuration_Min 73
#define	LongHighPulseDuration_Max 80

#define	LongLowPulseDuration_Min 67
#define	LongLowPulseDuration_Max 73


#define	TerminatorDuration_Min 1700
#define	TerminatorDuration_Max 7000



UnknownProtocol::UnknownProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*debug)(const char *) ) : TerminatedProtocolBase(id, 34, 36 , Bitstream, debug)
{
}

void UnknownProtocol::DecodeBitstream()
{}

void UnknownProtocol::DecodePulse(short int pulse, unsigned int duration)
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
					DURATION_LONG , LongLowPulseDuration_Min, LongLowPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );

        switch (durationresult)
        {
          case DURATION_LONG:
            if (1==BitDecodeState) 
            { // "0"
	      StoreDecodedBit(0);
              BitDecodeState = 0;
            } else  if (2==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
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
