#include <ProtocolBase.h>
#ifndef TerminatedProtocolBase_h
#define TerminatedProtocolBase_h

class TerminatedProtocolBase : public ProtocolBase {
	public:
	private:
	protected:
		virtual void StoreDecodedBit(short int bit) ;
};

#endif
