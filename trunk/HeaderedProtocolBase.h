#include <ProtocolBase.h>
#ifndef HeaderedProtocolBase_h
#define HeaderedProtocolBase_h

class HeaderedProtocolBase : public ProtocolBase {
	public:
		HeaderedProtocolBase(
			char * id, 
			unsigned short decodedbitsbuffersize , 
			unsigned short encodedbitsbuffersize ,
			void (*Bitstream)(const char *, unsigned short , volatile short int[]) );
	private:
	protected:
		virtual void StoreDecodedBit(short int bit) ;
};

#endif
