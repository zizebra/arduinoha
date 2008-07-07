#include <PT2262ProtocolBase.h>
#ifndef RanexProtocol_h
#define RanexProtocol_h

class RanexProtocol : public PT2262ProtocolBase {
	public:
		RanexProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceCommand)(char * , unsigned short int &, bool &) );
		unsigned int * EncodeCommand(unsigned short int device, bool command);
	private:
		void (*_DeviceCommand)(char * , unsigned short int &, bool &);
		void EncodeBit(unsigned short bit);
	protected:
		virtual void DecodeBitstream();
};

#endif
