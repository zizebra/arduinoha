#include "FranElecProtocol.h"
#include "WConstants.h"

// Product: 	PIR Motion Sensor
// Productcode: FRANELEC SENS01
// Chip: 	PT2262
// [  26] 91 [  81] 32 [  25] 92 [  80] 33 [  24] 94 [  79] 34 [  23] 94 [  79] 35 [  23] 94 [  79] 35 [  22] 95 [  79] 36 [  21] 96 [  78] 36 [  22] 96  [  77] 38 [  76] 38 [  77] 38 [  20] 98 [  20] 97 [  21] 97 [  21] 97 [  20] 97 [  21] 97 [  21] 933
// [  31] 89 [  84] 29 [  28] 88 [  87] 27 [  31] 88 [  28] 89 [  29] 91 [  84] 27 [  32] 87 [  29] 88 [  30] 88 [  29] 91 [  27] 89 [  29] 88 [  31] 87 [  30] 89 [  85] 32 [  82] 34 [  26] 88 [  30] 89 [  29] 88 [  30] 91 [  26] 92 [  26] 91 [  28]  925 


// Product:	Wireless Door Bell
// Productcode: FRANELEC ANTI01
// Chip:	M3D-95 / E0927



enum PulseDuration 
{
	DURATION_UNKNOWN = 0 , 
	DURATION_SHORT = 1 , 
	DURATION_LONG = 2, 
	DURATION_TERMINATOR = 3
};

#define	ShortPulseDuration_Min 17
#define	ShortPulseDuration_Max 39

#define	LongPulseDuration_Min 75
#define	LongPulseDuration_Max 99

#define	TerminatorDuration_Min 920
#define	TerminatorDuration_Max 940

FranElecProtocol::FranElecProtocol(
	char * id, 
	void (*Bitstream)(const char *, unsigned short , volatile short int[]), 
	void (*DeviceTripped)(char * id, unsigned short int &) ,
	void (*DeviceBatteryEmpty)(char * id, unsigned short int &),
	void (*debug)(const char *) ) : TerminatedProtocolBase(id, 24, 26, Bitstream, debug)
{
	_DeviceTripped = DeviceTripped;
	_DeviceBatteryEmpty = DeviceBatteryEmpty;
}

void FranElecProtocol::DecodeBitstream()
{
	if ((DecodedBitsBuffer[0]!=0) || (DecodedBitsBuffer[2]!=0) || (DecodedBitsBuffer[4]!=0) || (DecodedBitsBuffer[6]!=0) || (DecodedBitsBuffer[8]!=0) || 
		(DecodedBitsBuffer[10]!=0) || (DecodedBitsBuffer[12]!=0) || (DecodedBitsBuffer[14]!=0) /*|| (DecodedBitsBuffer[18]!=0) || (DecodedBitsBuffer[19]!=0) ||
		(DecodedBitsBuffer[20]!=0) || (DecodedBitsBuffer[21]!=0) || (DecodedBitsBuffer[22]!=0) || (DecodedBitsBuffer[23]!=0)*/) return;
	if  (DecodedBitsBuffer[17]!=1) return;

	bool BatteryEmpty = !DecodedBitsBuffer[16];

	unsigned short int device = (!DecodedBitsBuffer[15] * 1) + (!DecodedBitsBuffer[13] * 2) + (!DecodedBitsBuffer[11] * 4) + (!DecodedBitsBuffer[9] * 8) + (!DecodedBitsBuffer[7] * 16)  + (!DecodedBitsBuffer[5] * 32) + (!DecodedBitsBuffer[3] * 64) + (!DecodedBitsBuffer[1] * 128) ;

	if (_DeviceTripped!=0) _DeviceTripped(_id, device);
	if (_DeviceBatteryEmpty!=0 && BatteryEmpty)  _DeviceBatteryEmpty(_id, device);
}

void FranElecProtocol::DecodePulse(short int pulse, unsigned int duration)
{
    int durationresult = quantizeduration( duration, DURATION_UNKNOWN, 
					DURATION_SHORT , ShortPulseDuration_Min, ShortPulseDuration_Max, 
					DURATION_LONG , LongPulseDuration_Min, LongPulseDuration_Max, 
					DURATION_TERMINATOR, TerminatorDuration_Min, TerminatorDuration_Max );
    if (HIGH==pulse)
    { // een hoog signaal
       switch (durationresult)
       {
         case DURATION_SHORT:
           if (0==BitDecodeState) BitDecodeState=1;
           else 
	   {
		BitDecodeState = 0;
	   }
           break;
         case DURATION_LONG:
           if (0==BitDecodeState) BitDecodeState=2;
           else
	   {
		 BitDecodeState = 0;
	   }
           break;
         default:
	   BitDecodeState = 0;
	   ResetDecodedBitsBuffer();
           break;
       }
    }
    else
    { // curstate==LOW
        switch (durationresult)
        {
          case DURATION_SHORT:
            if (2==BitDecodeState) 
            { // een 1 ontvangen
	      StoreDecodedBit(1);
              BitDecodeState = 0;
            } else 
            {
	      ResetDecodedBitsBuffer();
              BitDecodeState = 0;
            }
            break;
          case DURATION_LONG:
            if (1==BitDecodeState) 
            { // "0"
	      StoreDecodedBit(0);
              BitDecodeState = 0;
            } else 
	    {
		BitDecodeState = 0;
		ResetDecodedBitsBuffer();
	    }
            break;  
          case DURATION_TERMINATOR :
            if (1==BitDecodeState && DecodedBitsBufferPosIdx+1==DecodedBitsBufferSize)
            {
		if (_ProtocolBitstream!=0) _ProtocolBitstream(_id , DecodedBitsBufferSize , DecodedBitsBuffer);
		DecodeBitstream();			
            } 
            BitDecodeState = 0;
	    ResetDecodedBitsBuffer();
            break;
          default: 
            BitDecodeState = 0;
	    ResetDecodedBitsBuffer();
            break;  
        }
    }
}
