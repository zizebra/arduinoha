#include "X10Protocol.h"
#include "WConstants.h"

#define PULSEDURATION_UNKNOWN 0 
#define PULSEDURATION_LEADERCARRIER 1
#define PULSEDURATION_LEADERSILENCE 2
#define PULSEDURATION_BIT1 3 
#define PULSEDURATION_BIT0 4 
#define PULSEDURATION_TRAILEREDGE  5 
#define PULSEDURATION_TRAILERSILENCE  6 

#define LeaderCarrier_Min 500
#define LeaderCarrier_Max 535

#define LeaderSilence_Min 250
#define LeaderSilence_Max 300

#define Bit1_Min 125
#define Bit1_Max 150
#define Bit0_Min 60
#define Bit0_Max 75

#define TrailerEdge_Min 20
#define TrailerEdge_Max 40

#define TrailerSilence_Min 2200
#define TrailerSilence_Max 2400


X10Protocol::X10Protocol(
	void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_debug = debug;

	DecodedBitsBufferSize = 32;
}

void X10Protocol::DecodeBitstream()
{
	for (int idx=0;idx<8;idx++)
	{
		if (DecodedBitsBuffer[0 + idx] == DecodedBitsBuffer[8 + idx] || DecodedBitsBuffer[16 + idx]!=DecodedBitsBuffer[24 + idx]) return;
	}
	if (DecodedBitsBuffer[10]==0) 
	{
		// On
	} else 
	{
		// Off
	}
	int device = 1 + (DecodedBitsBuffer[11] * 1) + (DecodedBitsBuffer[12] * 2) + (DecodedBitsBuffer[9] * 4) + (DecodedBitsBuffer[5] * 8);
	int house = 65 + (DecodedBitsBuffer[4] * 1) + (DecodedBitsBuffer[0] * 4) ;
}

int dur = 0;


void X10Protocol::DecodePulse(short int pulse, unsigned int duration)
{
	int durationresult = PULSEDURATION_UNKNOWN;
    if (HIGH==pulse)
    { // een hoog signaal
	switch (BitDecodeState)
	{
		case 0 :
			durationresult = quantizeduration( duration, PULSEDURATION_UNKNOWN ,
					PULSEDURATION_LEADERCARRIER , LeaderCarrier_Min , LeaderCarrier_Max );
			if (durationresult==PULSEDURATION_LEADERCARRIER) 
			{
				ResetDecodedBitsBuffer();
				BitDecodeState = 1;
			}
			else
			{
				BitDecodeState = 0;
				ResetDecodedBitsBuffer();
			}
			break;
		case 2 :
			dur = duration;
			BitDecodeState = 3;
			break;
		case 3 :
			durationresult = quantizeduration( dur, PULSEDURATION_UNKNOWN ,
					PULSEDURATION_BIT0 , Bit0_Min , Bit0_Max,
					PULSEDURATION_BIT1 , Bit1_Min , Bit1_Max );
			dur = duration;
			if (durationresult==PULSEDURATION_BIT0)
			{
				StoreDecodedBit(0);
			} else if (durationresult==PULSEDURATION_BIT1)
			{
				StoreDecodedBit(1);
			} else
			{
				ResetDecodedBitsBuffer();
				BitDecodeState = 0;
			}
			
			break;
		default :
			durationresult = quantizeduration( duration, PULSEDURATION_UNKNOWN ,
				PULSEDURATION_LEADERCARRIER , LeaderCarrier_Min , LeaderCarrier_Max );
			if (durationresult==PULSEDURATION_LEADERCARRIER)
			{
				BitDecodeState = 1;
			} else
			{
				
				BitDecodeState = 0;
			}
			ResetDecodedBitsBuffer();
			break;
	}
    }
    else
    { // curstate==LOW
        switch (BitDecodeState)
        {
		case 1 :
			durationresult = quantizeduration( duration, PULSEDURATION_UNKNOWN ,
					PULSEDURATION_LEADERSILENCE , LeaderSilence_Min , LeaderSilence_Max );
			if (durationresult==PULSEDURATION_LEADERSILENCE) 
			{
				ResetDecodedBitsBuffer();
				BitDecodeState = 2;
			}
			else
			{
				BitDecodeState = 0;
				ResetDecodedBitsBuffer();
			}
			break;
		case 3 :
			dur = dur + duration;
			break;
		default :
			ResetDecodedBitsBuffer();
			BitDecodeState = 0;
        }
    }

}
