#include <ShortHighProtocolBase.h>
#ifndef LaCrosseProtocol_h
#define LaCrosseProtocol_h

class LaCrosseProtocol : public ShortHighProtocolBase {
	public:
/*		virtual void DecodePulse(short int pulse , unsigned int duration ) ;*/
		LaCrosseProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]) );
	private:
	protected:
		virtual void DecodeBitstream();
};

#endif
