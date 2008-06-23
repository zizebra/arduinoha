#include <TerminatedProtocolBase.h>
#ifndef Skytronic2_h
#define Skytronic2_h

class Skytronic2 : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		Skytronic2(
			void (*Bitstream)(volatile short int[]), 
			void (*debug)(const char *) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
