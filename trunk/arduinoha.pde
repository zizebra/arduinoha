#define RANEX
//#define ELRO
//#define CARKEY
//#define X10
//#define MCVOICE
//#define SKYTRONIC

struct pulse {
	unsigned int duration;
	unsigned short state;
};


#include <ProtocolBase.h>

#ifdef RANEX
#include <RanexProtocol.h>
#endif

#ifdef ELRO
#include <ElroDecoder.h>
#endif

#ifdef MCVOICE
#include <McVoiceDecoder.h>
#endif

#ifdef CARKEY
#include <CarKeyDecoder.h>
#endif

#ifdef X10
#include <X10Decoder.h>
#endif

#ifdef SKYTRONIC
#include <SkytronicHomeLinkProtocol.h>
#endif




// --------------------------------------------------------------------------------------------------------------------------------------------------


// The pin of the diagnostic led
#define LEDPIN 13

// The pin number of the RSSI signal
#define RSSIPIN 2

// The pin number of the data signal
#define DATAPIN 8

// The irq number of the RSSI signal
#define RSSIIRQNR 0 //(RSSI) PIN 2 = IRQNR 0 , PIN 3 = IRQNR 1

// The pin number of the transmission signal
#define TXPIN 12

// This defines the number of received pulses which can be stored in the Circular buffer. 
// This buffer is used to allow pulses to be received while previous pulses are still being processed.
#define ReceivedPulsesCircularBufferSize 10


// --------------------------------------------------------------------------------------------------------------------------------------------------


// A reference to the received-pulses-CircularBuffer.
volatile pulse * receivedpulsesCircularBuffer = 0;

// The index number of the first pulse which will be read from the buffer.
unsigned short receivedpulsesCircularBuffer_readpos = 0;

// The index number of the position which will be used to store the next received pulse.
unsigned short receivedpulsesCircularBuffer_writepos = 0;

unsigned int prevTime; // ICR1 (timetick) value  of the start of the previous Pulse


unsigned short sendposidx = 0;
unsigned int * sendpulses;
short sendrepeatsleft;
unsigned short sendpulsecount;
unsigned short sendpulsestate = LOW;

unsigned short sendreceivestate = 0;
// 1 = listening
// 2 = RSSI pin triggered, receiving, restarting RSSI listening after timeout
// 3 = RSSI pin triggered, receiving
// 4 = sending, after finishing, restart listening
// 5 = sending, after finishing, dont restart listening

// --------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef RANEX
void Ranex_DeviceCommandReceived(unsigned short int &device, bool &command)
{
  Serial.print("RANEX: ");
  Serial.print(device, DEC);
  if (command) Serial.println(" ON"); else Serial.println(" OFF");
}
#endif

#ifdef ELRO
void Elro_DeviceCommandReceived(unsigned short int &device, bool &command)
{
  Serial.print("Elro: ");
  Serial.print(device, DEC);
  if (command) Serial.println(" ON"); else Serial.println(" OFF");
}
#endif

#ifdef MCVOICE
void McVoice_DeviceTrippedReceived(unsigned short int &device)
{
  Serial.print("McVoice: ");
  Serial.println(device, DEC);
}

void McVoice_DeviceBatteryEmptyReceived(unsigned short int &device)
{
  Serial.print("McVoice, Battery Empty: ");
  Serial.println(device, DEC);
}
#endif


// --------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef RANEX
RanexProtocol ranexProtocol = RanexProtocol(Ranex_BitstreamReceived, Ranex_DeviceCommandReceived , debug);
#endif

#ifdef ELRO
ElroDecoder elroDecoder = ElroDecoder(Elro_BitstreamReceived, Elro_DeviceCommandReceived , debug);
#endif

#ifdef MCVOICE
McVoiceDecoder mcvoiceDecoder = McVoiceDecoder(McVoice_BitstreamReceived, McVoice_DeviceTrippedReceived , McVoice_DeviceBatteryEmptyReceived, debug);
#endif

#ifdef X10
X10Decoder x10Decoder = X10Decoder(X10_BitstreamReceived, debug);
#endif

#ifdef CARKEY
CarKeyDecoder carkeyDecoder = CarKeyDecoder(CarKey_BitstreamReceived, debug);
#endif

#ifdef SKYTRONIC
SkytronicHomeLinkProtocol skytronicHomeLinkProtocol = SkytronicHomeLinkProtocol(SkytronicHomeLink_BitstreamReceived, debug);
#endif



// --------------------------------------------------------------------------------------------------------------------------------------------------


void debug(const char *val)
{
    Serial.print( val );
    Serial.print(" ");
}

// --------------------------------------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------------------------------------

#ifdef RANEX
void Ranex_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.print("RanexBitstream:");
  for (int idx=0; idx<12;idx++) Serial.print(bitbuffer[idx],DEC);
}
#endif

#ifdef X10
void X10_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.print("X10: ");
  for (int idx=0; idx<8;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.print(" ");
  for (int idx=8; idx<16;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.print(" ");
  for (int idx=16; idx<24;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.print(" ");
  for (int idx=24; idx<32;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.println("");
}
#endif

#ifdef ELRO
void Elro_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.println("ELRO");  
  for (int idx=0; idx<24;idx++) Serial.print(bitbuffer[idx],DEC);
}
#endif

#ifdef MCVOICE
void McVoice_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.print("McVoice: ");  
  for (int idx=0; idx<24;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.println("");
}
#endif

#ifdef SKYTRONIC
void SkytronicHomeLink_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.print("SkytronicHomeLink: ");  
  for (int idx=0; idx<10;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.println("");
}
#endif

#ifdef CARKEY
void CarKey_BitstreamReceived(volatile short int bitbuffer[])
{
  Serial.print("CarKey: ");  
  for (int idx=0; idx<61;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.println("");
}
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------------------------------------
// ISR
// --------------------------------------------------------------------------------------------------------------------------------------------------




// This Interrupt Service Routine will be called upon each change of data on DATAPIN.
ISR(TIMER1_CAPT_vect)
{ 
    unsigned int newTime = ICR1;
    
    if (sendreceivestate==2)
    {

      byte curstate;    
      if (bit_is_set(TCCR1B, ICES1)==0)
      { // ISR was triggerd by falling edge
        // set the ICES bit, so next INT is on rising edge
        TCCR1B|=(1<<ICES1);
        curstate = LOW;
      }
      else
      { // ISR was triggered by rising edge
        //clear the ICES1 bit so next INT is on falling edge
        TCCR1B&=(~(1<<ICES1));      
        curstate = HIGH;
      } 

      // Store pulse in receivedpulsesCircularBuffer
      receivedpulsesCircularBuffer[receivedpulsesCircularBuffer_writepos].state = !curstate;
      receivedpulsesCircularBuffer[receivedpulsesCircularBuffer_writepos].duration = newTime - prevTime;
    
      // Wrap buffer
      if (++receivedpulsesCircularBuffer_writepos >= ReceivedPulsesCircularBufferSize) 
      {
        receivedpulsesCircularBuffer_writepos = 0;
      }
    
      // Check overflow
      if (receivedpulsesCircularBuffer_writepos==receivedpulsesCircularBuffer_readpos)
      {
        Serial.print("!");
      }

    }
      prevTime = newTime;    
}


// This Interrupt Service Routine will be called upon a timer-overflow.
ISR(TIMER1_OVF_vect) 
{  
  if (sendreceivestate==2 || sendreceivestate==3) 
  {
    TIMSK1=0; //Disable timer interrupt; Stop edge timer, stop overflow interrupt

    if (sendreceivestate==2)
    {
      AttachToRssiInterrupt();
    } else sendreceivestate=0;
  } else if (sendreceivestate==4 || sendreceivestate==5)
  {
    if (++sendposidx>=sendpulsecount)
    {
      sendposidx = 0 ;
      sendrepeatsleft--;

      if (sendrepeatsleft<0)
      {
        digitalWrite(TXPIN,LOW);
        
        free(sendpulses);
        
        // Start receiving again
        TIMSK1=0; //Disable timer interrupt; Stop edge timer, stop overflow interrupt
        Serial.println("sent complete");
        if (sendreceivestate==4)
        {
          AttachToRssiInterrupt();
        } 
        sendreceivestate = 0;
        return;
      } 
    }
    sendpulsestate = !sendpulsestate;

    TCNT1 = (65535-sendpulses[sendposidx]) ;

    digitalWrite(TXPIN,sendpulsestate);
  }
}



// --------------------------------------------------------------------------------------------------------------------------------------------------



void InitializePins()
{
  pinMode(DATAPIN, INPUT);
  pinMode(RSSIPIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(TXPIN, OUTPUT);
}


void AttachToRssiInterrupt()
{
  Serial.println("AttachToRssiInterrupt");
  //Zet interrupts. Zie 14.11
  TCCR1A=0;   //Normal mode (timer)
  TCCR1B=0<<ICES1 | 1<<CS12 | 0<<CS11 | 0<<CS10 | 1<<ICNC1;  //Pre-scaler: 256, noise canceler, interrupt on edge: faling
  TIMSK1=0<<ICIE1;  //Disable timer event interrupt

  attachInterrupt(RSSIIRQNR, rssiPinTriggered , RISING);
  
  sendreceivestate = 1;
}

void Start()
{
  InitializePins();

#ifdef RANEX  
  ranexProtocol.Initialize();
#endif

#ifdef ELRO
  elroDecoder.Initialize();
#endif 

#ifdef MCVOICE
  mcvoiceDecoder.Initialize();
#endif

#ifdef X10
  x10Decoder.Initialize();
#endif

#ifdef CARKEY
  carkeyDecoder.Initialize();
#endif

#ifdef SKYTRONIC
  skytronicHomeLinkProtocol.Initialize();
#endif  

  AttachToRssiInterrupt();       
}

void Stop()
{  
  Serial.println("Detachinterrupt RSSI");
  if (sendreceivestate==1 || sendreceivestate==2 || sendreceivestate==3)
  {
    detachInterrupt(RSSIIRQNR);
    if (sendreceivestate==2)
    {
      sendreceivestate = 3;
      while (sendreceivestate!=0)
      {
        delay(10);
      }
    }
    sendreceivestate = 0;
  }
  
  TIMSK1=0;
}

void rssiPinTriggered(void)
{
  cli();
  Serial.println("RSSI TRIGGER");  
  sendreceivestate = 2;
  
  detachInterrupt(RSSIIRQNR);
  
  //clear the ICES1 bit so next INT is on falling edge
  TCCR1B&=(~(1<<ICES1));      

   // reset TCNT1
  TCNT1 = 0;
  
  //Enable timer interrupt; start timer
  TIMSK1=1<<ICIE1 | 1<<TOIE1; 
  
  //clear eventuele openstaande interrupts.
  TIFR1=0xff;

  prevTime = 0;
  
  sei();
}





void setup()
{
    Serial.begin(115200);
    
    receivedpulsesCircularBuffer = (volatile pulse * ) malloc ( ReceivedPulsesCircularBufferSize * sizeof(pulse) );
    
    Start();
}



void send(unsigned int * pulses)
{
    if (sendreceivestate==1 || sendreceivestate==2 || sendreceivestate==3 )
    {
        Stop();
    }

    if (sendreceivestate==4)
    {
        sendreceivestate = 5;        
        while(sendreceivestate!=0)
        {
            delay(10);
        }
    } 
   
    sendpulses = pulses;
    sendrepeatsleft = 5;
    sendpulsecount = 12 * 4 + 2;
    sendposidx = 0;    
    sendpulsestate = HIGH;

    sendreceivestate = 4;
    // reset TCNT1
    TCNT1 = 65535- pulses[sendposidx];
    digitalWrite(TXPIN, sendpulsestate);
  
    //Enable timer interrupt; start timer
    TIMSK1=1<<ICIE1 | 1<<TOIE1;     
}

void loop()
{
  // Is there a pulse still in the receivedpulses Circular buffer?
  if (receivedpulsesCircularBuffer_readpos != receivedpulsesCircularBuffer_writepos)
  { // yes
    unsigned int duration = receivedpulsesCircularBuffer[receivedpulsesCircularBuffer_readpos].duration;
    unsigned short state = receivedpulsesCircularBuffer[receivedpulsesCircularBuffer_readpos].state;
    if (state==HIGH) Serial.print("["); else Serial.print(" ");
    if (duration<10) Serial.print("   "); else if (duration<100) Serial.print("  "); else if (duration<1000) Serial.print(" ");
    Serial.print(duration , DEC);
    if (state==HIGH) Serial.print("]"); else Serial.print(" ");

#ifdef RANEX  
    ranexProtocol.DecodePulse(state, duration );
#endif

#ifdef ELRO
    elroDecoder.DecodePulse(state, duration);
#endif ELRO

#ifdef MCVOICE
    mcvoiceDecoder.DecodePulse(state, duration );
#endif

#ifdef X10
    x10Decoder.DecodePulse(state , duration );
#endif

#ifdef CARKEY
    carkeyDecoder.DecodePulse(state, duration );
#endif

#ifdef SKYTRONIC
    skytronicHomeLinkProtocol.DecodePulse(state , duration );
#endif    
    
    if (++receivedpulsesCircularBuffer_readpos >= ReceivedPulsesCircularBufferSize) 
    {
      receivedpulsesCircularBuffer_readpos = 0;
    }
  }

  if (Serial.available()>0)
  {
    int b = Serial.read();
    Serial.print("Key received: ");
    Serial.println(b,DEC);
    if (b>=48 && b<=58 ) 
    {
      #ifdef RANEX
      send( ranexProtocol.EncodeCommand(b-48 , HIGH) ) ;
      send( ranexProtocol.EncodeCommand(b-48 , LOW) ) ;
      #endif
    }


  }
  
  

}


// Code uitbreiden zodat een Stop wacht tot de receiving klaar is.
