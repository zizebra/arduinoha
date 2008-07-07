#include <TerminatedProtocolBase.h>
#ifndef SkytronicHomeLinkProtocol_h
#define SkytronicHomeLinkProtocol_h

class SkytronicHomeLinkProtocol : public TerminatedProtocolBase {
	public:		
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		SkytronicHomeLinkProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceTripped)(char * , unsigned short int &, unsigned short int &) ) ;
		unsigned int * EncodeCommand(unsigned short int , unsigned short int );
	private:
		void (*_DeviceTripped)(char * , unsigned short int &, unsigned short int &);
		void EncodeBit(unsigned short bit);
	protected:
		virtual void DecodeBitstream();
};

#endif
