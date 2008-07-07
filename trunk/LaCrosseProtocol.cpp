#include "LaCrosseProtocol.h"
#include "WConstants.h"

// [  23]   93 [  68]   31 [  23]   93 [  68]   32 [  21]   95 [  21]   95 [  21]   95 [  66]   33 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  21]   95 [  66]   33 [  66]   33 [  20]   97 [  19]   97 [  19]   97 [  19]   96 [  20]   96 [  20]   96 [  21]  969


LaCrosseProtocol::LaCrosseProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[])
) : ShortHighProtocolBase(id, 24, 26, Bitstream, 17 , 25 , 60 , 70 , 25 , 35 , 90 , 100 , 950, 990, -1 , 0, 1 , -1)
{
}

void LaCrosseProtocol::DecodeBitstream()
{}
