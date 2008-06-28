#include <HeaderedProtocolBase.h>
#ifndef X10Protocol_h
#define X10Protocol_h

class X10Protocol : public HeaderedProtocolBase{
	public:
		virtual void DecodeBitstream();
		virtual void DecodePulse(short int pulse , unsigned int duration ) ;
		X10Protocol(
			char * id, 
			void (*Bitstream)(const char * , unsigned short, volatile short int[]), 
			void (*debug)(const char *) );
	private:
		unsigned int LeaderCarrier_Min, LeaderCarrier_Max;
		unsigned int LeaderSilence_Min, LeaderSilence_Max;
		unsigned int Bit1_Min, Bit1_Max;
		unsigned int Bit0_Min, Bit0_Max;
		unsigned int TrailerEdge_Min, TrailerEdge_Max ;
		unsigned int TrailerSilence_Min, TrailerSilence_Max;
	protected:
};

#endif
