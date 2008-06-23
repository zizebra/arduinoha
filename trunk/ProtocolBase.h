#ifndef ProtocolBase_h
#define ProtocolBase_h

class ProtocolBase {
	public:
		ProtocolBase();
		virtual void DecodePulse(short int ,unsigned int) = 0;
		virtual void Initialize();
	private:
	protected:
		void (*_debug)(const char *);
		unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown, 
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max );
		unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max );
		unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max );
		unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max );
		unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max,
					unsigned short quant5, unsigned int quant5_min, unsigned int quant5_max );
		virtual void StoreDecodedBit(short int bit) = 0;
		virtual void DecodeBitstream() = 0;
		unsigned int ShortPulseDuration_Min, ShortPulseDuration_Max;
		unsigned int LongPulseDuration_Min, LongPulseDuration_Max;
		unsigned int TerminatorDuration_Min, TerminatorDuration_Max;
		volatile short int *DecodedBitsBuffer;
		unsigned int *EncodedBitsBuffer;
		short int DecodedBitsBufferSize ;
		short int EncodedBitsBufferSize ;
		short int BitDecodeState;
		short int DecodedBitsBufferPosIdx;
		short int EncodedBitsBufferPosIdx;
		void ResetBitDecodeState();
		void ResetDecodedBitsBuffer();
		void ResetEncodedBitsBuffer();
		void (*_ProtocolBitstream)(volatile short int[]);
};

#endif
