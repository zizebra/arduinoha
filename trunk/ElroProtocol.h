#include <PT2262ProtocolBase.h>
#ifndef ElroProtocol_h
#define ElroProtocol_h

class ElroProtocol : public PT2262ProtocolBase {
	public:
		ElroProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
			void (*DeviceCommand)(char *, unsigned short int &, bool &),
			void (*debug)(const char *) );
	private:
		void (*_DeviceCommand)(char *, unsigned short int &, bool &);
	protected:
		virtual void DecodeBitstream();
};

#endif
