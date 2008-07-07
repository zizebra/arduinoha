#include "RanexProtocol.h"
#include <stdlib.h>

// HS2262A-R4

RanexProtocol::RanexProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*DeviceCommand)(char * , unsigned short int &, bool &) )  : PT2262ProtocolBase(id, Bitstream, 5, 20 , 30 , 75, 430, 460)
{
	_DeviceCommand = DeviceCommand;
}

void RanexProtocol::DecodeBitstream()
{
	unsigned short int device = (!DecodedBitsBuffer[2] ) + (!DecodedBitsBuffer[4] << 1) + (!DecodedBitsBuffer[6] << 2);
	bool command = DecodedBitsBuffer[16];
	if (_DeviceCommand!=0) _DeviceCommand(_id, device, command);
}

void RanexProtocol::EncodeBit(unsigned short bit)
{
	if (bit==0)
	{
			StoreEncodedPulse( 10 ); 
			StoreEncodedPulse( 47 );
			StoreEncodedPulse( 10 );
			StoreEncodedPulse( 47 );
	} else
	{
			StoreEncodedPulse( 47 );
			StoreEncodedPulse( 10 );
			StoreEncodedPulse( 47 );
			StoreEncodedPulse( 10 );
	}
}

unsigned int * RanexProtocol::EncodeCommand(unsigned short int device, bool command)
{
	EncodedBitsBuffer = (unsigned int *)calloc( EncodedBitsBufferSize , sizeof(unsigned int) );
	ResetEncodedBitsBuffer();

	EncodeBit(0);
	EncodeBit(!(device & 1)); 
	EncodeBit(!(device & 2)); 
	EncodeBit(!(device & 4));
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(command);
	EncodeBit(0);
	EncodeBit(0);
	EncodeBit(0);

	StoreEncodedPulse( 10 );
	StoreEncodedPulse( 450 );

	return EncodedBitsBuffer;
	
}
