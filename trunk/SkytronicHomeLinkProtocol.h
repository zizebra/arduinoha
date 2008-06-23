#include <HeaderedProtocolBase.h>
#ifndef SkytronicHomeLinkProtocol_h
#define SkytronicHomeLinkProtocol_h

class SkytronicHomeLinkProtocol : public HeaderedProtocolBase {
	public:		
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		SkytronicHomeLinkProtocol(
			void (*Bitstream)(volatile short int[]), 
			void (*debug)(const char *) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
