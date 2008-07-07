#include "SkytronicHomeLinkProtocol.h"
#include <stdlib.h>

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
// One trigger, sends 10 commands

#define LeaderHigh_Min 1
#define LeaderHigh_Max 370

#define LeaderLow_Min 110
#define LeaderLow_Max 155

#define DurationShort_Min 25
#define DurationShort_Max 40

#define DurationLong_Min 85
#define DurationLong_Max 100

#define TerminatorSilence_Min 2200
#define TerminatorSilence_Max 6000


SkytronicHomeLinkProtocol::SkytronicHomeLinkProtocol(
	char * id, 
	void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
	void (*DeviceTripped)(char * , unsigned short int &,unsigned short int &) ) : TerminatedProtocolBase(id, 14 , 32 , Bitstream )
{
	_DeviceTripped = DeviceTripped;
}

void SkytronicHomeLinkProtocol::DecodeBitstream()
{
	unsigned short device = (DecodedBitsBuffer[0] * 1) + (DecodedBitsBuffer[1] * 2) + (DecodedBitsBuffer[2] * 4) + (DecodedBitsBuffer[3] * 8) + (DecodedBitsBuffer[4] * 16) + (DecodedBitsBuffer[5] * 32) ;
	unsigned short house = (DecodedBitsBuffer[10] * 1) + (DecodedBitsBuffer[9] * 2) ;
	if (_DeviceTripped!=0) _DeviceTripped(_id, device, house);
}

void SkytronicHomeLinkProtocol::DecodePulse(short int pulse, unsigned int duration)
{
    if (0!=pulse)
    { // een hoog signaal
	if (0==BitDecodeState && between(duration , LeaderHigh_Min , LeaderHigh_Max ) )  BitDecodeState = 1; 
	else if (2==BitDecodeState && between(duration , DurationShort_Min , DurationShort_Max) ) BitDecodeState = 3;
	else 
	{
		BitDecodeState = 0;
		DecodedBitsBufferPosIdx = 0;
	}
    }
    else
    { // curstate==LOW
	if (1==BitDecodeState && between(duration, LeaderLow_Min , LeaderLow_Max)) BitDecodeState = 2;
	else if (3==BitDecodeState && between(duration, DurationShort_Min , DurationShort_Max))
	{
	      	StoreDecodedBit(1);
		BitDecodeState = 2;
	} else if (3==BitDecodeState && between(duration, DurationLong_Min, DurationLong_Max))
	{
		StoreDecodedBit(0);
		BitDecodeState = 2;
	} else
	{
		if (3==BitDecodeState && between(duration, TerminatorSilence_Min , TerminatorSilence_Max)) Terminator();
		BitDecodeState = 0;
		DecodedBitsBufferPosIdx = 0;
	}
    }

}

void SkytronicHomeLinkProtocol::EncodeBit(unsigned short bit)
{
	if (bit!=0) 
	{
		StoreEncodedPulse( 34 ); 
		StoreEncodedPulse( 34 ); 
	} else
	{
		StoreEncodedPulse( 34 ); 
		StoreEncodedPulse( 92 ); 
	}
}

unsigned int * SkytronicHomeLinkProtocol::EncodeCommand(unsigned short int device, unsigned short int house)
{
	EncodedBitsBuffer = (unsigned int *)calloc( EncodedBitsBufferSize , sizeof(unsigned int) );
	ResetEncodedBitsBuffer();

	// Carrier
	StoreEncodedPulse( 3 );
	StoreEncodedPulse( 120 );

	EncodeBit(device & 1); 
	EncodeBit(device & 2); 
	EncodeBit(device & 4); 
	EncodeBit(device & 8); 
	EncodeBit(device & 16); 
	EncodeBit(device & 32); 
	EncodeBit(1);
	EncodeBit(1);
	EncodeBit(1);
	if (house & 2 !=0) EncodeBit(1); else EncodeBit(0);
	if (house & 1 !=0) EncodeBit(1); else EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(1);

	// Terminator
	StoreEncodedPulse( 34 ); 
	StoreEncodedPulse( 2700 );

	return EncodedBitsBuffer;
}


