#include <TerminatedProtocolBase.h>
#ifndef CarKeyDecoder_h
#define CarKeyDecoder_h

class CarKeyDecoder : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse ,unsigned int duration) ;
		CarKeyDecoder(
			void (*Bitstream)(const char * , unsigned short , volatile short int[]), 
			void (*debug)(const char *) );
	private:
		unsigned int ShortPulseDuration_Min;
		unsigned int ShortPulseDuration_Max;

		unsigned int LongPulseDuration_Min ;
		unsigned int LongPulseDuration_Max ;

		unsigned int TerminatorDuration_Min ;
		unsigned int TerminatorDuration_Max ;

		unsigned int ExtraLongPulseDuration_Min ;
		unsigned int ExtraLongPulseDuration_Max ;
	protected:
		virtual void DecodeBitstream();
};

#endif
