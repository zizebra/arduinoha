#include <TerminatedProtocolBase.h>
#ifndef ElroDecoder_h
#define ElroDecoder_h

class ElroDecoder : public TerminatedProtocolBase{
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		ElroDecoder(
			void (*Bitstream)(volatile short int[]), 
			void (*DeviceCommand)(unsigned short int &, bool &),
			void (*debug)(const char *) );
	private:
		void (*_DeviceCommand)(unsigned short int &, bool &);
	protected:
		virtual void DecodeBitstream();
};

#endif
