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

unsigned short ProtocolBase::quantizeduration(unsigned int duration, unsigned short quant_unknown, 
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	return quant_unknown;
}

unsigned short ProtocolBase::quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	return quant_unknown;
}

unsigned short ProtocolBase::quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	return quant_unknown;
}

unsigned short ProtocolBase::quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	else if (duration>=quant4_min && duration<=quant4_max) return quant4;
	return quant_unknown;
}

unsigned short ProtocolBase::quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max,
					unsigned short quant5, unsigned int quant5_min, unsigned int quant5_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	else if (duration>=quant4_min && duration<=quant4_max) return quant4;
	else if (duration>=quant5_min && duration<=quant5_max) return quant5;
	return quant_unknown;
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

