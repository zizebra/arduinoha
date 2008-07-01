#include <PT2262ProtocolBase.h>
#ifndef FranElecProtocol_h
#define FranElecProtocol_h

class FranElecProtocol : public PT2262ProtocolBase {
	public:
		FranElecProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceTripped)(char * id, unsigned short int &) ,
			void (*DeviceBatteryEmpty)(char * id, unsigned short int &),
			void (*debug)(const char *) );
	private:
	protected:
		void (*_DeviceTripped)(char * id,  unsigned short int &);
		void (*_DeviceBatteryEmpty)(char * id, unsigned short int &);
		virtual void DecodeBitstream();
};

#endif
