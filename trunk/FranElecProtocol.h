#include <TerminatedProtocolBase.h>
#ifndef FranElecProtocol_h
#define FranElecProtocol_h

class FranElecProtocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		FranElecProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceTripped)(char * id, unsigned short int &),
			void (*DeviceBatteryEmpty)(char * id, unsigned short int &),
			void (*debug)(const char *) );
	private:
		void (*_DeviceTripped)(char * id,  unsigned short int &);
		void (*_DeviceBatteryEmpty)(char * id, unsigned short int &);
	protected:
		virtual void DecodeBitstream();
};

#endif
