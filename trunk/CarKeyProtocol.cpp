#include "CarKeyProtocol.h"
#include "WConstants.h"

enum PulseDuration 
{
	PULSEDURATION_UNKNOWN = 0 , 
	PULSEDURATION_SHORT = 1 , 
	PULSEDURATION_LONG = 2, 
	PULSEDURATION_EXTRALONG = 3 , 
	PULSEDURATION_TERMINATOR = 4
};

#define	ShortPulseDuration_Min 9
#define	ShortPulseDuration_Max 25

#define	LongPulseDuration_Min 38
#define	LongPulseDuration_Max 58

#define	ExtraLongPulseDuration_Min 67
#define	ExtraLongPulseDuration_Max 97

#define	TerminatorDuration_Min 263
#define	TerminatorDuration_Max 283



CarKeyProtocol::CarKeyProtocol(
	char * id, 
	void (*Bitstream)(const char * , unsigned short ,volatile short int[]), 
	void (*debug)(const char *) ) : TerminatedProtocolBase(id, 61, 0, Bitstream, debug)
{
}

void CarKeyProtocol::DecodeBitstream()
{}

void CarKeyProtocol::DecodePulse(short int pulse, unsigned int duration)
{
	int durationresult = quantizeduration( duration, PULSEDURATION_UNKNOWN , 
			PULSEDURATION_SHORT , ShortPulseDuration_Min , ShortPulseDuration_Max,
			PULSEDURATION_LONG , LongPulseDuration_Min , LongPulseDuration_Max ,
			PULSEDURATION_EXTRALONG , ExtraLongPulseDuration_Min , ExtraLongPulseDuration_Max ,
			PULSEDURATION_TERMINATOR , TerminatorDuration_Min , TerminatorDuration_Max);

	if (HIGH==pulse)
	{ // een hoog signaal
		switch (durationresult)
		{

			case PULSEDURATION_SHORT :
				if (BitDecodeState==1) 
				{
					StoreDecodedBit(0); // L K
				} else if (BitDecodeState==2)
				{
					StoreDecodedBit(2); // X K
				} else
				{
					ResetDecodedBitsBuffer();
				}
				BitDecodeState = 0;
				break;
			case PULSEDURATION_LONG :
				if (BitDecodeState==1)
				{
					StoreDecodedBit(1); // L L
				} else if (BitDecodeState==2)
				{
					StoreDecodedBit(3); // X L
				}
				BitDecodeState = 0;			
				break;
			case PULSEDURATION_TERMINATOR : 
				Terminator();
				ResetDecodedBitsBuffer();
				BitDecodeState = 0;
				break;
			default :
				BitDecodeState = 0;
				ResetDecodedBitsBuffer();
				break;
		}		
	} else if (LOW==pulse)
	{
		switch (durationresult)
		{
			case PULSEDURATION_LONG :
				if (BitDecodeState==0) BitDecodeState = 1;
				break;
			case PULSEDURATION_EXTRALONG :
				if (BitDecodeState==0) BitDecodeState = 2;
				break;
			default :
				BitDecodeState = 0;
				ResetDecodedBitsBuffer();
				break;
		}
	}

}

