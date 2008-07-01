#include <TerminatedProtocolBase.h>
#ifndef PT2262ProtocolBase_h
#define PT2262ProtocolBase_h

class PT2262ProtocolBase : public TerminatedProtocolBase {
	public:
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		PT2262ProtocolBase(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*debug)(const char *) ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int ,
			unsigned int , unsigned int );
	private:
	protected:
		virtual void DecodeBitstream() = 0;
		unsigned int ShortPulseDuration_Min ;
		unsigned int ShortPulseDuration_Max ;

		unsigned int LongPulseDuration_Min ;
		unsigned int LongPulseDuration_Max ;

		unsigned int TerminatorDuration_Min ;
		unsigned int TerminatorDuration_Max ;
};

#endif
