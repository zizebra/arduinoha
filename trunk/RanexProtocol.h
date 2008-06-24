#include <TerminatedProtocolBase.h>
#ifndef RanexProtocol_h
#define RanexProtocol_h

class RanexProtocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		RanexProtocol(
			void (*Bitstream)(volatile short int[]), 
			void (*DeviceCommand)(unsigned short int &, bool &),
			void (*debug)(const char *) );
		unsigned int * EncodeCommand(unsigned short int device, bool command);
	private:
		void StoreEncodedPulse(unsigned int duration);
		void EncodePulse(unsigned short int pulse);
		void EncodeBit(unsigned short bit);
		void EncodeTerminator();
		void (*_DeviceCommand)(unsigned short int &, bool &);
	protected:
		virtual void DecodeBitstream();
};

#endif
