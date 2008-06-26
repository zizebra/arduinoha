#include <TerminatedProtocolBase.h>
#ifndef LaCrosseProtocol_h
#define LaCrosseProtocol_h

class LaCrosseProtocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		LaCrosseProtocol(
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*debug)(const char *) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
