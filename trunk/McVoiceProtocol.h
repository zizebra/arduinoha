#include <ShortHighProtocolBase.h>
#ifndef McVoiceProtocol_h
#define McVoiceProtocol_h

class McVoiceProtocol : public ShortHighProtocolBase {
	public:
		McVoiceProtocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceTripped)(char * , unsigned short int &),
			void (*DeviceBatteryEmpty)(char * , unsigned short int &) );
	private:
		void (*_DeviceTripped)(char * , unsigned short int &);
		void (*_DeviceBatteryEmpty)(char * , unsigned short int &);
	protected:
		virtual void DecodeBitstream();
};

#endif
