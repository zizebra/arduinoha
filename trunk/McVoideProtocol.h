#include <TerminatedProtocolBase.h>
#ifndef McVoiceDecoder_h
#define McVoiceDecoder_h

class McVoiceDecoder : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		McVoiceDecoder(
			void (*Bitstream)(volatile short int[]), 
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
