#include "SkytronicHomeLinkProtocol.h"
#include <stdlib.h>
#include "WConstants.h"

// Product: Skylink Household Alert Receiver
// Productcode: GM-433R
// Chip: Unknown
// Status: Send signals dont get recognized by the product

// Product: Skylink Water Sensor
// Productcode: WA-434T
// Chip: Unknown
// [   2]  121 [  36]   90 [  34]   92 [  33]   92 [  34]   91 [  34]   92 [  33]   92 [  33]   30 [  32]   31 [  32]   31 [  31]   32 [  31]   31 [  31]   95 [  30]   95 [  31]   32 [  30] 2921
// [ 352]  121 [  35]   90 [  35]   91 [  34]   93 [  32]   93 [  32]   93 [  33]   93 [  32]   30 [  32]   31 [  32]   31 [  30]   33 [  30]   33 [  29]   96 [  30]   96 [  30]   32 [  29] 3111 
// [   3]  120 [  36]   90 [  35]   91 [  34]   91 [  34]   92 [  33]   92 [  33]   92 [  34]   29 [  33]   30 [  32]   31 [  32]   31 [  31]   31 [  32]   94 [  31]   94 [  31]   32 [  30] 2885 
// Status: decoded correctly


#define DURATION_UNKNOWN 0 
#define DURATION_LEADERHIGH 1
#define DURATION_LEADERLOW 5
#define DURATION_SHORT 2 
#define DURATION_LONG 3 
#define DURATION_TERMINATOR  4 

#define LeaderHigh_Min 1
#define LeaderHigh_Max 370

#define LeaderLow_Min 110
#define LeaderLow_Max 155

#define DurationShort_Min 25
#define DurationShort_Max 40

#define DurationLong_Min 85
#define DurationLong_Max 100

#define TerminatorSilence_Min 2500
#define TerminatorSilence_Max 6000


SkytronicHomeLinkProtocol::SkytronicHomeLinkProtocol(
	void (*Bitstream)(volatile short int[]), 
	void (*DeviceTripped)(unsigned short int &,unsigned short int &) ,
	void (*debug)(const char *) )
{
	_ProtocolBitstream = Bitstream;
	_DeviceTripped = DeviceTripped;
	_debug = debug;
	

	DecodedBitsBufferSize = 14;
	EncodedBitsBufferSize = 14 * 2 + 2 + 2;
}

void SkytronicHomeLinkProtocol::DecodeBitstream()
{
	unsigned short device = (DecodedBitsBuffer[0] * 1) + (DecodedBitsBuffer[1] * 2) + (DecodedBitsBuffer[2] * 4) + (DecodedBitsBuffer[3] * 8) + (DecodedBitsBuffer[4] * 16) + (DecodedBitsBuffer[5] * 32) ;
	unsigned short house = (DecodedBitsBuffer[10] * 1) + (DecodedBitsBuffer[9] * 2) ;
	if (_DeviceTripped!=0) _DeviceTripped(device, house);
}

void SkytronicHomeLinkProtocol::DecodePulse(short int pulse, unsigned int duration)
{
    int durationresult = DURATION_UNKNOWN;
    if (HIGH==pulse)
    { // een hoog signaal
	switch (BitDecodeState)
	{
		case 0 :
		    	durationresult = quantizeduration( duration, DURATION_UNKNOWN ,					  
					DURATION_LEADERHIGH , LeaderHigh_Min , LeaderHigh_Max);

			if (durationresult==DURATION_LEADERHIGH) 
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
  			durationresult = quantizeduration( duration, DURATION_UNKNOWN ,					  
					  DURATION_SHORT , DurationShort_Min , DurationShort_Max);

			if (durationresult==DURATION_SHORT) 
			{
				BitDecodeState = 3;
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
			durationresult = quantizeduration( duration, DURATION_UNKNOWN ,
					  DURATION_LEADERLOW , LeaderLow_Min , LeaderLow_Max);
			if (durationresult==DURATION_LEADERLOW) 
			{
				BitDecodeState = 2;
			} else
			{
				ResetBitDecodeState();
				ResetDecodedBitsBuffer();
			}
			break;
		case 3 :
			durationresult = quantizeduration( duration, DURATION_UNKNOWN ,					  
					  DURATION_SHORT , DurationShort_Min , DurationShort_Max,
					  DURATION_LONG , DurationLong_Min, DurationLong_Max,
					  DURATION_TERMINATOR , TerminatorSilence_Min , TerminatorSilence_Max);
			if (durationresult==DURATION_SHORT) 
			{
			      	StoreDecodedBit(1);
				BitDecodeState = 2;
			}
			else if (durationresult==DURATION_LONG)
			{
				StoreDecodedBit(0);
				BitDecodeState = 2;
			} else if (durationresult==DURATION_TERMINATOR)
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

void SkytronicHomeLinkProtocol::EncodePulse(unsigned short int pulse)
{
	switch (pulse)
	{
		case DURATION_SHORT:
			StoreEncodedPulse( 35 ); 
			break;
		case DURATION_LONG:
			StoreEncodedPulse( 90 ); 
			break;
		case DURATION_LEADERHIGH:
			StoreEncodedPulse( 132 );
			break;
		case DURATION_LEADERLOW:
			StoreEncodedPulse( 120 );
			break;
		case DURATION_TERMINATOR:
			StoreEncodedPulse( 2700 );
			break;
	}
}

void SkytronicHomeLinkProtocol::EncodeBit(unsigned short bit)
{
	switch (bit)
	{
		case 1 :
			EncodePulse(DURATION_SHORT);
			EncodePulse(DURATION_SHORT);
			break;
		case 0 :
			EncodePulse(DURATION_SHORT);
			EncodePulse(DURATION_LONG);
			break;
	}
}

void SkytronicHomeLinkProtocol::EncodeCarrier()
{
	EncodePulse( DURATION_LEADERHIGH );
	EncodePulse( DURATION_LEADERLOW );
}

void SkytronicHomeLinkProtocol::EncodeTerminator()
{
	EncodePulse(DURATION_SHORT);
	EncodePulse(DURATION_TERMINATOR);
}

unsigned int * SkytronicHomeLinkProtocol::EncodeCommand(unsigned short int device, unsigned short int house)
{
	EncodedBitsBuffer = (unsigned int *)calloc( EncodedBitsBufferSize , sizeof(unsigned int) );
	ResetEncodedBitsBuffer();

	EncodeCarrier();
	if (device & 1 !=0) EncodeBit(1); else EncodeBit(0);
	if (device & 2 !=0) EncodeBit(1); else EncodeBit(0);
	if (device & 4 !=0) EncodeBit(1); else EncodeBit(0);
	if (device & 8 !=0) EncodeBit(1); else EncodeBit(0);
	if (device & 16 !=0) EncodeBit(1); else EncodeBit(0);
	if (device & 32 !=0) EncodeBit(1); else EncodeBit(0);
	EncodeBit(1);
	EncodeBit(1);
	EncodeBit(1);
	if (house & 2 !=0) EncodeBit(1); else EncodeBit(0);
	if (house & 1 !=0) EncodeBit(1); else EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(1);
	EncodeTerminator();
	return EncodedBitsBuffer;
	
}


