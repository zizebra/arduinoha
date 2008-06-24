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


ProtocolBase::ProtocolBase()
{
}

void ProtocolBase::Initialize()
{
	DecodedBitsBuffer = (volatile short int *)calloc( DecodedBitsBufferSize , sizeof(short int) );
	ResetDecodedBitsBuffer();
	ResetBitDecodeState();
}


void ProtocolBase::ResetBitDecodeState()
{
	BitDecodeState = 0;
}

void ProtocolBase::ResetDecodedBitsBuffer()
{
	for (int idx=0; idx<DecodedBitsBufferSize;idx++) DecodedBitsBuffer[idx]= -1;
	DecodedBitsBufferPosIdx = 0;
}

void ProtocolBase::ResetEncodedBitsBuffer()
{
	for (int idx=0; idx<EncodedBitsBufferSize;idx++) EncodedBitsBuffer[idx]= -1;
	EncodedBitsBufferPosIdx = 0;
}

