#include <TerminatedProtocolBase.h>
#ifndef FranElecProtocol_h
#define FranElecProtocol_h

class FranElecProtocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		FranElecProtocol(
			void (*Bitstream)(volatile short int[]), 
			void (*debug)(const char *) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
