#include "HeaderedProtocolBase.h"
#include "WConstants.h"

void HeaderedProtocolBase::StoreDecodedBit(short int bit)
{
	// Store the decoded bit
        DecodedBitsBuffer[DecodedBitsBufferPosIdx] = bit;

	// Is the buffer full?
	if (DecodedBitsBufferPosIdx+1>=DecodedBitsBufferSize) 
	{ // Yes
		if (_ProtocolBitstream!=0) _ProtocolBitstream("Unknown\0" , DecodedBitsBufferSize , DecodedBitsBuffer);
		DecodeBitstream();

		BitDecodeState = 0;
		ResetDecodedBitsBuffer();
	}

	DecodedBitsBufferPosIdx++;
}



