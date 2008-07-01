#include "McVoiceProtocol.h"
#include "WConstants.h"

// Optimization: High bits are shorter ([58,29], [58,28], [57,29])
// Bitstreams from Mc Voice Smoke-detector "FR-1:
// 16 71 16 71 15 71 58 29 14 72 58 28 15 72 57 29 14 72 58 28 15 72 57 29 14 73 56 30 14 73 56 30 56 30 56 31 12 74 13 73 13 74 13 73 14 73 13 73 14 680
// 14 72 14 72 15 72 57 29 14 75 54 30 13 73 57 29 14 74 55 31 13 73 56 30 13 74 56 30 13 73 56 30 56 31 55 31 12 75 12 74 13 74 12 74 13 74 13 74 12 678

// Short0 = 16 , Long0 = 71 , Terminator = 680
// Short1 = 29 , Long1 = 58 
// s0 l0 s0 l0 s0 l0 l1 s1 s0 l0 l1 s1
// PulseCombinations = S0 L0 -> 0, L1, S1 -> 1 , S0, T -> end

McVoiceProtocol::McVoiceProtocol(
	char * id, 
	void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
	void (*DeviceTripped)(char * , unsigned short int &) ,
	void (*DeviceBatteryEmpty)(char *, unsigned short int &),
	void (*debug)(const char *) )  : ShortHighProtocolBase( id, Bitstream, debug , 11 , 16 , 53, 85 , 28 , 32 , 71 ,76 , 640 , 720 )
{
	_DeviceTripped = DeviceTripped;
	_DeviceBatteryEmpty = DeviceBatteryEmpty;
}

void McVoiceProtocol::DecodeBitstream()
{	bool BatteryEmpty = !DecodedBitsBuffer[16];

	unsigned short int device = (!DecodedBitsBuffer[15] * 1) + (!DecodedBitsBuffer[13] * 2) + (!DecodedBitsBuffer[11] * 4) + (!DecodedBitsBuffer[9] * 8) + (!DecodedBitsBuffer[7] * 16)  + (!DecodedBitsBuffer[5] * 32) + (!DecodedBitsBuffer[3] * 64) + (!DecodedBitsBuffer[1] * 128) ;

	if (_DeviceTripped!=0) _DeviceTripped(_id, device);
	if (_DeviceBatteryEmpty!=0 && BatteryEmpty)  _DeviceBatteryEmpty(_id, device);
}
