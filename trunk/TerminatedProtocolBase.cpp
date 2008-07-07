#include "TerminatedProtocolBase.h"
#include "WConstants.h"

TerminatedProtocolBase::TerminatedProtocolBase(
		char * id, 
		unsigned short decodedbitsbuffersize ,
		unsigned short encodedbitsbuffersize ,
		void (*Bitstream)(const char *, unsigned short , volatile short int[])
) : ProtocolBase(id, decodedbitsbuffersize, encodedbitsbuffersize, Bitstream)
{
}


void TerminatedProtocolBase::StoreDecodedBit(short int bit)
{
	// Is the buffer full?
	if (DecodedBitsBufferPosIdx+1>=DecodedBitsBufferSize) 
	{ // Yes
		// shift every bit one position left
		for (int idx=1; idx<DecodedBitsBufferSize;idx++) DecodedBitsBuffer[idx-1]=DecodedBitsBuffer[idx];
		DecodedBitsBuffer[DecodedBitsBufferSize-1] = -1;
	} else
	{ // No
		DecodedBitsBufferPosIdx++;
	}

	// Store the decoded bit
        DecodedBitsBuffer[DecodedBitsBufferPosIdx] = bit;
}


void TerminatedProtocolBase::Terminator()
{
	if (DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)
        {
		if (_ProtocolBitstream!=0) _ProtocolBitstream( _id , DecodedBitsBufferSize, DecodedBitsBuffer);
		DecodeBitstream();			
        } 
}
