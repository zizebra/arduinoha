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
static unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown, 
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	return quant_unknown;
}

static unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	return quant_unknown;
}

static unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	return quant_unknown;
}

static unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	else if (duration>=quant4_min && duration<=quant4_max) return quant4;
	return quant_unknown;
}

static unsigned short quantizeduration(unsigned int duration, unsigned short quant_unknown,
					unsigned short quant1, unsigned int quant1_min, unsigned int quant1_max,
					unsigned short quant2, unsigned int quant2_min, unsigned int quant2_max,
					unsigned short quant3, unsigned int quant3_min, unsigned int quant3_max,
					unsigned short quant4, unsigned int quant4_min, unsigned int quant4_max,
					unsigned short quant5, unsigned int quant5_min, unsigned int quant5_max )
{
	if (duration>=quant1_min && duration<=quant1_max) return quant1;
	else if (duration>=quant2_min && duration<=quant2_max) return quant2;
	else if (duration>=quant3_min && duration<=quant3_max) return quant3;
	else if (duration>=quant4_min && duration<=quant4_max) return quant4;
	else if (duration>=quant5_min && duration<=quant5_max) return quant5;
	return quant_unknown;
}
		virtual void StoreDecodedBit(short int bit) = 0;
		virtual void DecodeBitstream() = 0;
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
