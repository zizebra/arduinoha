#include "ElroProtocol.h"
#include "WConstants.h"

ElroProtocol::ElroProtocol(
	char * id, 
	void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
	void (*DeviceCommand)(char * , unsigned short int &, bool &) ,
	void (*debug)(const char *) ) : PT2262ProtocolBase(id, Bitstream, debug, 10, 30 , 50 , 70, 600, 640)
{
	_DeviceCommand = DeviceCommand;
}

void ElroProtocol::DecodeBitstream()
{	unsigned short int device = (DecodedBitsBuffer[9] << 0) + (DecodedBitsBuffer[11] << 1) + (DecodedBitsBuffer[13] << 2) + (DecodedBitsBuffer[15] << 3) + (DecodedBitsBuffer[17] << 4) ;

	bool command = DecodedBitsBuffer[23];
	if (_DeviceCommand!=0) _DeviceCommand(_id, device, command);
}


