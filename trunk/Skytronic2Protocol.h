#include <TerminatedProtocolBase.h>
#ifndef Skytronic2Protocol_h
#define Skytronic2Protocol_h

class Skytronic2Protocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		Skytronic2Protocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
