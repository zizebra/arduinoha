#define ShowPulses
#define RANEX
#define ELRO
#define CARKEY
#define X10
#define MCVOICE
#define SKYTRONIC
#define SKYTRONIC2
#define FRANELEC
#define LACROSSE
#define UNKNOWN

struct pulse {
	unsigned int duration;
	unsigned short state;
};

#include <ProtocolBase.h>

#ifdef RANEX
#include <RanexProtocol.h>
#endif

#ifdef ELRO
#include <ElroProtocol.h>
#endif

#ifdef MCVOICE
#include <McVoiceProtocol.h>
#endif

#ifdef CARKEY
#include <CarKeyProtocol.h>
#endif

#ifdef X10
#include <X10Protocol.h>
#endif

#ifdef SKYTRONIC
#include <SkytronicHomeLinkProtocol.h>
#endif

#ifdef SKYTRONIC2
#include <Skytronic2Protocol.h>
#endif

#ifdef FRANELEC
#include <FranElecProtocol.h>
#endif

#ifdef LACROSSE
#include <LaCrosseProtocol.h>
#endif

#ifdef UNKNOWN
#include <UnknownProtocol.h>
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
// The Arduino must catch up or an overflow will occur and pulses will be lost.
#define ReceivedPulsesCircularBufferSize 20


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
void Ranex_DeviceCommandReceived(char * id, unsigned short int &device, bool &command)
{
  Serial.print(id);
  Serial.print(device, DEC);
  if (command) Serial.println(" ON"); else Serial.println(" OFF");
}
#endif

#ifdef ELRO
void Elro_DeviceCommandReceived(char * id,  unsigned short int &device, bool &command)
{
  Serial.print(id);
  Serial.print(device, DEC);
  if (command) Serial.println(" ON"); else Serial.println(" OFF");
}
#endif

#ifdef MCVOICE
void McVoice_DeviceTrippedReceived(char * id, unsigned short int &device)
{  
  Serial.print(id);
  Serial.println(device, DEC);
}

void McVoice_DeviceBatteryEmptyReceived(char * id , unsigned short int &device)
{
  Serial.print( id );
  Serial.print(" Battery Empty: ");
  Serial.println(device, DEC);
}
#endif

#ifdef FRANELEC
void FranElec_DeviceTrippedReceived(char * id,  unsigned short int &device)
{
  Serial.print(id);
  Serial.println(device, DEC);
}

void FranElec_DeviceBatteryEmptyReceived(char * id, unsigned short int &device)
{
  Serial.print(id);
  Serial.print(" Battery Empty: ");
  Serial.println(device, DEC);
}
#endif

#ifdef SKYTRONIC
void SkyTronic_DeviceTrippedReceived(char * id, unsigned short int &device, unsigned short int &house)
{
  Serial.print(id);
  Serial.print(device, DEC);
  Serial.print(" ");
  Serial.println(house, DEC);
}
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------------


#ifdef RANEX
RanexProtocol ranexProtocol = RanexProtocol("Ranex 433\0", BitstreamReceived, Ranex_DeviceCommandReceived , debug);
#endif

#ifdef ELRO
ElroProtocol elroProtocol = ElroProtocol("Elro 433\0" , BitstreamReceived, Elro_DeviceCommandReceived , debug);
#endif

#ifdef MCVOICE
McVoiceProtocol mcvoiceProtocol = McVoiceProtocol("McVoice 433\0" , BitstreamReceived, McVoice_DeviceTrippedReceived , McVoice_DeviceBatteryEmptyReceived, debug);
#endif

#ifdef X10
X10Protocol x10Protocol = X10Protocol("X10 433\0",BitstreamReceived, debug);
#endif

#ifdef CARKEY
CarKeyProtocol carkeyProtocol = CarKeyProtocol("CarKey 433\0", BitstreamReceived, debug);
#endif

#ifdef SKYTRONIC
SkytronicHomeLinkProtocol skytronicHomeLinkProtocol = SkytronicHomeLinkProtocol("Skytronic 433\0" , BitstreamReceived, SkyTronic_DeviceTrippedReceived, debug);
#endif

#ifdef SKYTRONIC2
Skytronic2Protocol skytronic2Protocol = Skytronic2Protocol("Skytronic2 433\0" , BitstreamReceived, debug);
#endif

#ifdef FRANELEC
FranElecProtocol franelecProtocol = FranElecProtocol("FranElec 433\0", BitstreamReceived, FranElec_DeviceTrippedReceived , FranElec_DeviceBatteryEmptyReceived, debug);
#endif

#ifdef LACROSSE
LaCrosseProtocol lacrosseProtocol = LaCrosseProtocol("LaCrosse 433\0" , BitstreamReceived, debug);
#endif

#ifdef UNKNOWN
UnknownProtocol unknownProtocol = UnknownProtocol("Unknown 433\0" , BitstreamReceived, debug);
#endif

// --------------------------------------------------------------------------------------------------------------------------------------------------


void debug(const char *val)
{
    Serial.print( val );
    Serial.print(" ");
}

// --------------------------------------------------------------------------------------------------------------------------------------------------


void BitstreamReceived(const char *protocol, unsigned short length, volatile short int bitbuffer[])
{
  Serial.print(protocol);
  for (int idx=0; idx<length;idx++) Serial.print(bitbuffer[idx],DEC);
  Serial.println("");
}

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
        if (sendreceivestate==4)
        {
          AttachToRssiInterrupt();
        } 
        sendreceivestate = 0;
        return;
      } 
    }
    sendpulsestate = !sendpulsestate;
    unsigned int duration = sendpulses[sendposidx];

#ifdef ShowPulses
    if (sendpulsestate)
    {
      Serial.print("<");
    }
    Serial.print(duration,DEC); 
        if (sendpulsestate)
    {
          Serial.print(">");
    }
#endif    
    TCNT1 = 65535-duration ;

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
  AttachToRssiInterrupt();       
}

void Stop()
{  
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
  sendreceivestate = 2;

   // reset TCNT1
  TCNT1 = 0;
  
  //Enable timer interrupt; start timer
  TIMSK1=1<<ICIE1 | 1<<TOIE1; 
  
  //clear eventuele openstaande interrupts.
  TIFR1=0xff;

  
  //clear the ICES1 bit so next INT is on falling edge
  TCCR1B&=(~(1<<ICES1));      


  prevTime = 0;

  detachInterrupt(RSSIIRQNR);
  
  sei();
}





void setup()
{
    Serial.begin(115200);
    
    receivedpulsesCircularBuffer = (volatile pulse * ) malloc ( ReceivedPulsesCircularBufferSize * sizeof(pulse) );
    Start();
}



void send(unsigned int * pulses, unsigned short nrpulses)
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
    sendpulsecount = nrpulses; 
    sendposidx = 0;    
    sendpulsestate = HIGH;

    sendreceivestate = 4;

    unsigned int duration = pulses[sendposidx]; 
    
#ifdef ShowPulses
    if (sendpulsestate)
    {
      Serial.print("<");
    }
    Serial.print(duration,DEC); 
        if (sendpulsestate)
    {
          Serial.print(">");
    }
#endif

    
    // reset TCNT1
    TCNT1 = ((unsigned int) 65535)- duration;
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
#ifdef ShowPulses    
    if (state==HIGH) Serial.print("["); else Serial.print(" ");
    if (duration<10) Serial.print("   "); else if (duration<100) Serial.print("  "); else if (duration<1000) Serial.print(" ");
    Serial.print(duration , DEC);
    if (state==HIGH) Serial.print("]"); else Serial.print(" ");
#endif

#ifdef RANEX  
    ranexProtocol.DecodePulse(state, duration );
#endif

#ifdef ELRO
    elroProtocol.DecodePulse(state, duration);
#endif ELRO

#ifdef MCVOICE
    mcvoiceProtocol.DecodePulse(state, duration );
#endif

#ifdef X10
    x10Protocol.DecodePulse(state , duration );
#endif

#ifdef CARKEY
    carkeyProtocol.DecodePulse(state, duration );
#endif

#ifdef SKYTRONIC
    skytronicHomeLinkProtocol.DecodePulse(state , duration );
#endif    

#ifdef SKYTRONIC2
    skytronic2Protocol.DecodePulse(state , duration );
#endif    

#ifdef FRANELEC
    franelecProtocol.DecodePulse(state , duration );
#endif    
    
#ifdef LACROSSE
    lacrosseProtocol.DecodePulse(state , duration );
#endif    

#ifdef UNKNOWN
    unknownProtocol.DecodePulse(state , duration );
#endif    

    if (++receivedpulsesCircularBuffer_readpos >= ReceivedPulsesCircularBufferSize) 
    {
      receivedpulsesCircularBuffer_readpos = 0;
    }
  }

  if (Serial.available()>0)
  {
    int b = Serial.read();
    Serial.println(b,DEC);
/*    if (b>=48 && b<=58 ) 
    {
      #ifdef RANEX
      send( ranexProtocol.EncodeCommand(b-48 , HIGH) , 12 * 4 + 2) ;
      send( ranexProtocol.EncodeCommand(b-48 , LOW) , 12 * 4 + 2) ;
      #endif
    }
    if (b==97)
    {
      #ifdef SKYTRONIC
      send( skytronicHomeLinkProtocol.EncodeCommand(0 , 3) , (14 * 2 + 2 + 8 ) * 2) ;
      #endif 
    }*/

  }

}
