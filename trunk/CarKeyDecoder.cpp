#include "CarKeyDecoder.h"
#include "WConstants.h"

enum PulseDuration 
{
	PULSEDURATION_UNKNOWN = 0 , 
	PULSEDURATION_SHORT = 1 , 
	PULSEDURATION_LONG = 2, 
	PULSEDURATION_EXTRALONG = 3 , 
	PULSEDURATION_TERMINATOR = 4
};


CarKeyDecoder::CarKeyDecoder(
	void (*Bitstream)(volatile short int[]), 
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_debug = debug;

	ShortPulseDuration_Min = 9;
	ShortPulseDuration_Max = 25;

	LongPulseDuration_Min = 38;
	LongPulseDuration_Max = 58;

	ExtraLongPulseDuration_Min = 67;
	ExtraLongPulseDuration_Max = 97;

	TerminatorDuration_Min = 263;
	TerminatorDuration_Max = 283;

	DecodedBitsBufferSize = 61;
}

void CarKeyDecoder::DecodeBitstream()
{}

void CarKeyDecoder::DecodePulse(short int pulse, unsigned int duration)
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
				ResetBitDecodeState();
				break;
			case PULSEDURATION_LONG :
				if (BitDecodeState==1)
				{
					StoreDecodedBit(1); // L L
				} else if (BitDecodeState==2)
				{
					StoreDecodedBit(3); // X L
				}
				ResetBitDecodeState();			
				break;
			case PULSEDURATION_TERMINATOR : 
			        if (DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)    				
				{
					if (_ProtocolBitstream!=0) _ProtocolBitstream(DecodedBitsBuffer);
					DecodeBitstream();
				}

				ResetDecodedBitsBuffer();
				ResetBitDecodeState();
				break;
			default :
				ResetBitDecodeState();
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
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
				break;
		}
	}

}

