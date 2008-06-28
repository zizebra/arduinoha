#include <stdlib.h>
#include "WConstants.h"
#include "ProtocolBase.h"

/*!
 * \brief Pure-virtual workaround.
 *
 * The avr-libc does not support a default implementation for handling 
 * possible pure-virtual calls. This is a short and empty workaround for this.
 */
extern "C" {
  void __cxa_pure_virtual()
  {
    // put error handling here
  }
}

ProtocolBase::ProtocolBase(
	char * id, 
	unsigned short decodedbitsbuffersize , 
	unsigned short encodedbitsbuffersize ,
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*debug)(const char *))
{
	_id = id;
	DecodedBitsBufferSize = decodedbitsbuffersize;
	EncodedBitsBufferSize = encodedbitsbuffersize;
	_ProtocolBitstream = Bitstream;
	_debug = debug;
}

void ProtocolBase::Initialize()
{
	DecodedBitsBuffer = (volatile short int *)calloc( DecodedBitsBufferSize , sizeof(short int) );
	ResetDecodedBitsBuffer();
	BitDecodeState = 0;
}

void ProtocolBase::ResetDecodedBitsBuffer()
{
	DecodedBitsBufferPosIdx = 0;
}

void ProtocolBase::ResetEncodedBitsBuffer()
{
	EncodedBitsBufferPosIdx = 0;
}

void ProtocolBase::StoreEncodedPulse(unsigned int duration)
{
	if (EncodedBitsBufferPosIdx < EncodedBitsBufferSize)
	{
		EncodedBitsBuffer[EncodedBitsBufferPosIdx] = duration;
	}
	EncodedBitsBufferPosIdx ++;
}


