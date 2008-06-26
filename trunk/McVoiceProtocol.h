#include <TerminatedProtocolBase.h>
#ifndef McVoiceProtocol_h
#define McVoiceProtocol_h

class McVoiceProtocol : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		McVoiceProtocol(
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*DeviceTripped)(unsigned short int &),
			void (*DeviceBatteryEmpty)(unsigned short int &),
			void (*debug)(const char *) );
	private:
		void (*_DeviceTripped)(unsigned short int &);
		void (*_DeviceBatteryEmpty)(unsigned short int &);
	protected:
		virtual void DecodeBitstream();
};

#endif
