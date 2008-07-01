#include <TerminatedProtocolBase.h>
#ifndef ShortHighProtocolBase_h
#define ShortHighProtocolBase_h

class ShortHighProtocolBase : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		ShortHighProtocolBase(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*debug)(const char *) ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int );
	private:
	protected:
		virtual void DecodeBitstream() = 0;
		unsigned int ShortHighPulseDuration_Min ;
		unsigned int ShortHighPulseDuration_Max ;

		unsigned int LongHighPulseDuration_Min ;
		unsigned int LongHighPulseDuration_Max ;

		unsigned int LongLowPulseDuration_Min ;
		unsigned int LongLowPulseDuration_Max ;

		unsigned int ShortLowPulseDuration_Min ;
		unsigned int ShortLowPulseDuration_Max ;

		unsigned int TerminatorDuration_Min ;
		unsigned int TerminatorDuration_Max ;
};

#endif
