#include "HeaderedProtocolBase.h"
#include "WConstants.h"

HeaderedProtocolBase::HeaderedProtocolBase(
		char * id, 
		unsigned short decodedbitsbuffersize , 
		unsigned short encodedbitsbuffersize ,
		void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
		void (*debug)(const char *)
) : ProtocolBase(id, decodedbitsbuffersize, encodedbitsbuffersize, Bitstream, debug)
{
}

void HeaderedProtocolBase::StoreDecodedBit(short int bit)
{
	// Store the decoded bit
        DecodedBitsBuffer[DecodedBitsBufferPosIdx] = bit;

	// Is the buffer full?
	if (DecodedBitsBufferPosIdx+1>=DecodedBitsBufferSize) 
	{ // Yes
		if (_ProtocolBitstream!=0) _ProtocolBitstream( _id , DecodedBitsBufferSize , DecodedBitsBuffer);
		DecodeBitstream();

		BitDecodeState = 0;
		ResetDecodedBitsBuffer();
	}

	DecodedBitsBufferPosIdx++;
}



