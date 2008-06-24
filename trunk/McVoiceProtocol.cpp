#include "McVoiceProtocol.h"
#include "WConstants.h"

// Optimization: High bits are shorter ([58,29], [58,28], [57,29])
// Bitstreams from Mc Voice Smoke-detector "FR-1:
// 16 71 16 71 15 71 58 29 14 72 58 28 15 72 57 29 14 72 58 28 15 72 57 29 14 73 56 30 14 73 56 30 56 30 56 31 12 74 13 73 13 74 13 73 14 73 13 73 14 680
// 14 72 14 72 15 72 57 29 14 75 54 30 13 73 57 29 14 74 55 31 13 73 56 30 13 74 56 30 13 73 56 30 56 31 55 31 12 75 12 74 13 74 12 74 13 74 13 74 12 678

// Short0 = 16 , Long0 = 71 , Terminator = 680
// Long1 = 58 , Short1 = 29
// s0 l0 s0 l0 s0 l0 l1 s1 s0 l0 l1 s1
// PulseCombinations = S0 L0 -> 0, L1, S1 -> 1 , S0, T -> end

enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

#define	ShortPulseDuration_Min 10
#define	ShortPulseDuration_Max 35

#define	LongPulseDuration_Min 50
#define	LongPulseDuration_Max 80

#define	TerminatorDuration_Min 640
#define	TerminatorDuration_Max 720



McVoiceProtocol::McVoiceProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*DeviceTripped)(unsigned short int &) ,
	void (*DeviceBatteryEmpty)(unsigned short int &),
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_DeviceTripped = DeviceTripped;
	_DeviceBatteryEmpty = DeviceBatteryEmpty;
	_debug = debug;


	DecodedBitsBufferSize = 24;
}

void McVoiceProtocol::DecodeBitstream()
{	if ((DecodedBitsBuffer[0]!=0) || (DecodedBitsBuffer[2]!=0) || (DecodedBitsBuffer[4]!=0) || (DecodedBitsBuffer[6]!=0) || (DecodedBitsBuffer[8]!=0) || 
		(DecodedBitsBuffer[10]!=0) || (DecodedBitsBuffer[12]!=0) || (DecodedBitsBuffer[14]!=0) /*|| (DecodedBitsBuffer[18]!=0) || (DecodedBitsBuffer[19]!=0) ||
		(DecodedBitsBuffer[20]!=0) || (DecodedBitsBuffer[21]!=0) || (DecodedBitsBuffer[22]!=0) || (DecodedBitsBuffer[23]!=0)*/) return;
	if  (DecodedBitsBuffer[17]!=1) return;

	bool BatteryEmpty = !DecodedBitsBuffer[16];

	unsigned short int device = (!DecodedBitsBuffer[15] * 1) + (!DecodedBitsBuffer[13] * 2) + (!DecodedBitsBuffer[11] * 4) + (!DecodedBitsBuffer[9] * 8) + (!DecodedBitsBuffer[7] * 16)  + (!DecodedBitsBuffer[5] * 32) + (!DecodedBitsBuffer[3] * 64) + (!DecodedBitsBuffer[1] * 128) ;

	if (_DeviceTripped!=0) _DeviceTripped(device);
	if (_DeviceBatteryEmpty!=0 && BatteryEmpty)  _DeviceBatteryEmpty(device);
}

void McVoiceProtocol::DecodePulse(short int pulse, unsigned int duration)
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
