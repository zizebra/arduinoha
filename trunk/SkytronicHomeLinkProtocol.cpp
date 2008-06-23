#include "SkytronicHomeLinkProtocol.h"
#include "WConstants.h"

#define PULSEDURATION_UNKNOWN 0 
#define PULSEDURATION_LEADERCARRIER 1
#define PULSEDURATION_SHORT 2 
#define PULSEDURATION_LONG 3 
#define PULSEDURATION_TRAILER  4 

#define LeaderCarrier_Min 140
#define LeaderCarrier_Max 155

#define DurationShort_Min 25
#define DurationShort_Max 40

#define DurationLong_Min 85
#define DurationLong_Max 95

#define TrailerSilence_Min 2500
#define TrailerSilence_Max 6000


SkytronicHomeLinkProtocol::SkytronicHomeLinkProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_debug = debug;
	

	DecodedBitsBufferSize = 10;
	EncodedBitsBufferSize = 10 * 2 + 1 + 1;
}

void SkytronicHomeLinkProtocol::DecodeBitstream()
{
	int device = (DecodedBitsBuffer[0] * 1) + (DecodedBitsBuffer[1] * 2) + (DecodedBitsBuffer[2] * 4) + (DecodedBitsBuffer[3] * 8) + (DecodedBitsBuffer[4] * 16) + (DecodedBitsBuffer[5] * 32);
	int house = 65 + (DecodedBitsBuffer[10] * 1) + (DecodedBitsBuffer[9] * 2) ;
}

void SkytronicHomeLinkProtocol::DecodePulse(short int pulse, unsigned int duration)
{
    int durationresult = quantizeduration( duration, PULSEDURATION_UNKNOWN ,
					  PULSEDURATION_LEADERCARRIER , LeaderCarrier_Min , LeaderCarrier_Max,
					  PULSEDURATION_SHORT , DurationShort_Min , DurationShort_Max,
					  PULSEDURATION_LONG , DurationLong_Min, DurationLong_Max,
					  PULSEDURATION_TRAILER , TrailerSilence_Min , TrailerSilence_Max);

    if (HIGH==pulse)
    { // een hoog signaal
	switch (BitDecodeState)
	{
		case 0 :
			if (durationresult==PULSEDURATION_LEADERCARRIER) 
			{
				ResetDecodedBitsBuffer();
				BitDecodeState = 1;
			}
			else
			{
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			}
			break;
		case 2 :
			if (durationresult==PULSEDURATION_SHORT) 
			{
				StoreDecodedBit(2);
				BitDecodeState = 1;
			} else 
			{
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			}
			break;
		case 3 : 
			if (durationresult==PULSEDURATION_SHORT)
			{
				StoreDecodedBit(0);
				BitDecodeState=1;
			} else if (durationresult==PULSEDURATION_LONG)
			{
				StoreDecodedBit(1);
				BitDecodeState=1;
			} else 
			{
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			}
			break;
		default :
			ResetBitDecodeState();
			ResetDecodedBitsBuffer();
			break;
	}
    }
    else
    { // curstate==LOW
        switch (BitDecodeState)
        {
		case 1 :
			if (durationresult==PULSEDURATION_SHORT) 
			{
				BitDecodeState = 2;
			}
			else if (durationresult==PULSEDURATION_LONG)
			{
				BitDecodeState = 3;
			} else if (durationresult==PULSEDURATION_TRAILER)
			{
		            	if (DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)
		            	{
					if (_ProtocolBitstream!=0) _ProtocolBitstream(DecodedBitsBuffer);
					DecodeBitstream();			
            		    	} 
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			} else
			{
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			}
			break;
		default :
			ResetDecodedBitsBuffer();
			ResetBitDecodeState();
        }
    }

}
