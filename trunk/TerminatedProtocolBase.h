#include <ProtocolBase.h>
#ifndef TerminatedProtocolBase_h
#define TerminatedProtocolBase_h

class TerminatedProtocolBase : public ProtocolBase {
	public:
		TerminatedProtocolBase(
			char * id, 
			unsigned short decodedbitsbuffersize , 
			unsigned short encodedbitsbuffersize ,
			void (*Bitstream)(const char *, unsigned short , volatile short int[])
		);
	private:
	protected:
		void Terminator();
		virtual void DecodeBitstream() = 0;
		virtual void StoreDecodedBit(short int bit) ;
};

#endif
