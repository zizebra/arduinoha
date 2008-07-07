#include <ShortHighProtocolBase.h>
#ifndef UnknownProtocol_h
#define UnknownProtocol_h

class UnknownProtocol : public ShortHighProtocolBase {
	public:
		UnknownProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]),
			void (*TempReceived)(char * , float &) ,
			void (*HygroReceived)(char * , short &));
	private:
		void (*_TempReceived)(char * , float &);
		void (*_HygroReceived)(char * , short &);
	protected:
		virtual void DecodeBitstream();
};

#endif
