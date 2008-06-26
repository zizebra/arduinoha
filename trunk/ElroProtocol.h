#include <TerminatedProtocolBase.h>
#ifndef ElroProtocol_h
#define ElroProtocol_h

class ElroProtocol : public TerminatedProtocolBase{
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		ElroProtocol(
			void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
			void (*DeviceCommand)(unsigned short int &, bool &),
			void (*debug)(const char *) );
	private:
		void (*_DeviceCommand)(unsigned short int &, bool &);
	protected:
		virtual void DecodeBitstream();
};

#endif
