#include <TerminatedProtocolBase.h>
#ifndef Skytronic2Protocol_h
#define Skytronic2Protocol_h

class Skytronic2Protocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		Skytronic2Protocol(
			void (*Bitstream)(volatile short int[]), 
			void (*debug)(const char *) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
