#include <Arduino.h>
#include <types.h>
#include <MIDI.h>

#define NUM_BANKS         8                              // number of group of keys (for Fatar 61 they're arranged in 8 banks)
#define NUM_KEYS         61                              // number of keys in the keyboard used
#define NUM_KEYZONES      2
#define NUM_BIT_MPLX      3
#define LEFT              0
#define RIGHT             1
#define HITRIG_VELOCITY   1
#define SWS_TIMER       200                              // switches scanning period in milliseconds
#define NUM_MIDI_PORTS    2                              // number of MIDI in/out ports (couples)

bank_t banks[NUM_BANKS];
bank_t prev_banks[NUM_BANKS];
key_fatar_t keys[NUM_KEYS];                              // number of keys of the keybed (Fatar 61)


teensy_port_reg enMplx[] = {
   { 6, 10 },
   { 9, 11 }
}; // pins 6 and 9 correspond to bits 10 and 11 of GPIO2 (GPIO7), used to enable the Left or Right multiplexer
teensy_port_reg addrMplx[] = {
   { 10, 0 },
   { 11, 2 },
   { 12, 1 }
}; // pins 10, 11 and 12 correspond to bits 0, 2, 1 of GPIO2 (GPIO7), used to address scan banks (multiplexer)
teensy_port_reg inGridMatrix[NUM_BANKS] = {
   { 14, 18 },
   { 15, 19 },
   { 16, 23 },
   { 17, 22 },
   { 18, 17 },
   { 19, 16 },
   { 20, 26 },
   { 21, 27 }
}; // pins 14, 15, 16, 17, 18, 19, 20 and 21 correspond to bits 18, 19, 23, 22, 17, 16, 26 and 27 of GPIO1 (GPIO6), used to read input from grid matrix

byte switchPins[] = {2, 3, 4};                           // Pins of the 3 switches for mode selector
byte midiInPins[] = {0, 7};                              // RX1, RX2 pins respectively: for reading MIDI signals
byte midiOutPins[] = {1, 8};                             // TX1, TX2 pins respectively: for writing MIDI signals

byte left, right;                                        // read word from keyboard: pins are connected LEFT: BRBRBRBR MKMKMKMK, RIGHT: BRBRBRBR MKMKMKMK

int lastSwsScan = 0;          // last moment in milliseconds when a scan of the switches has been performed
bool highTrig = false;        // current status of high trigger switch
byte midiOut = 0b11;          // current status of midi outputs (0b01 => Midi1, 0b10 => Midi2, 0b11 => both)

MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>* midiPort[NUM_MIDI_PORTS];
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);

const int midiCh = 6;

/*
#define CPU_RESET_CYCLECOUNTER    do { ARM_DEMCR |= ARM_DEMCR_TRCENA;          \
                                       ARM_DWT_CTRL |= ARM_DWT_CTRL_CYCCNTENA; \
                                       ARM_DWT_CYCCNT = 0; } while(0)
volatile int cycles;
*/

void process_midi_in(int midiPortIndex);
void kbd_scan();
void kbd_increment();
byte compact_dr(word dr, teensy_port_reg arrangement[], int size);
void trigger(key_fatar_t *key, event_t event);
void output_midi(int note, int velocity, bool isOn);
int velocity(int t);
void sws_scan();

void setup() {
   // Start serial comm for debug purposes
   Serial.begin(9600);

   // Setup output pins
   for (int i=0; i<NUM_KEYZONES; i++) {
      pinMode(enMplx[i].pin, OUTPUT);
   }
   for (int i=0; i<NUM_BIT_MPLX; i++) {
      pinMode(addrMplx[i].pin, OUTPUT);
   }

   // Setup input pins for keys (pulldown resistors)
   for (int i=0; i<NUM_BANKS; i++) {
      pinMode(inGridMatrix[i].pin, INPUT_PULLDOWN);
   }
   // Setup input pins for mode-switches (pullup resistors)
   for (int i=0; i<3; i++) {
      pinMode(switchPins[i], INPUT_PULLUP);
   }

   // Initialize banks array at 0
   memset(banks, 0x00, sizeof(banks));
   memset(prev_banks, 0x00, sizeof(prev_banks));
   
   // Initialize keyboard keys struct
   for (int key=0; key<NUM_KEYS; key++) {
      keys[key].midi_note = 36 + key;
      keys[key].t = 0;
      keys[key].played = false;
   }
   
   midiPort[0] = &MIDI1;
   midiPort[1] = &MIDI2;
   for (int i = 0; i < NUM_MIDI_PORTS; i++) {
      midiPort[i]->begin(midiCh);
   }
}

// MAIN LOOP
void loop() {
   for (int i = 0; i < NUM_MIDI_PORTS; i++) {
      if (midiPort[i]->read()) process_midi_in(i);
   }
   kbd_scan();
   kbd_increment();
   if (millis() - lastSwsScan > SWS_TIMER) {
      sws_scan();
   }
}

void process_midi_in(int midiPortIndex) {
   int note, velocity, channel, d1, d2;

   byte type = midiPort[midiPortIndex]->getType();
   switch (type) {
   case midi::NoteOn:
      note = midiPort[midiPortIndex]->getData1();
      velocity = midiPort[midiPortIndex]->getData2();
      channel = midiPort[midiPortIndex]->getChannel();
      if (velocity > 0) {
         Serial.println(String("Note On:  ch=") + channel + ", note=" + note + ", velocity=" + velocity);
      } else {
         Serial.println(String("Note Off: ch=") + channel + ", note=" + note);
      }
      break;
   case midi::NoteOff:
      note = midiPort[midiPortIndex]->getData1();
      velocity = midiPort[midiPortIndex]->getData2();
      channel = midiPort[midiPortIndex]->getChannel();
      Serial.println(String("Note Off: ch=") + channel + ", note=" + note + ", velocity=" + velocity);
      break;
   default:
      d1 = midiPort[midiPortIndex]->getData1();
      d2 = midiPort[midiPortIndex]->getData2();
      Serial.println(String("Message, type=") + type + ", data = " + d1 + " " + d2);
   }
}

// Scan all the keys divided in banks
void kbd_scan() {
   // Scan and store
   for(int bank=0; bank<NUM_BANKS; bank++) {
      prev_banks[bank] = banks[bank]; // Store previous state so we can look for changes

      // Scan left keyboard
      GPIO7_DR_SET = (1 << enMplx[LEFT].bit) | (bank & 0x7);
      delayMicroseconds(10); // Debounce
      left = compact_dr((GPIO6_DR >> 16), inGridMatrix, 8);
      GPIO7_DR_CLEAR = (1 << enMplx[LEFT].bit);
      delayMicroseconds(10); // Debounce
     
      // Scan right keyboard
      GPIO7_DR_SET = (1 << enMplx[RIGHT].bit) | (bank & 0x7);
      delayMicroseconds(10); // Debounce
      right = compact_dr((GPIO6_DR >> 16), inGridMatrix, 8);
      GPIO7_DR_CLEAR = (0xC07);
      delayMicroseconds(10); // Debounce

      banks[bank].breaks = (left & 0x0F) | (right & 0x0F) << 4;
      banks[bank].makes = (left & 0xF0) >> 4 | (right & 0xF0);
   }

   // Process
   for(int bank=0; bank<NUM_BANKS; bank++) {
      byte diff;
      
      // Check top switches and fire events
      diff = banks[bank].breaks ^ prev_banks[bank].breaks;
      if(diff) {
         //Serial.println(bank);
         for(int key=0; key<8; key++) {
            if(bitRead(diff, key)) { 
               event_t event = bitRead(banks[bank].breaks, key) ? KEY_TOUCHED : KEY_TOP;
               trigger(&keys[bank+key*8], event);
            }
         }
      }

      // Check bottom switches and fire events
      diff = banks[bank].makes ^ prev_banks[bank].makes;
      if(diff) {
         for(int key=0; key<8; key++) {
            if(bitRead(diff, key)) { 
               event_t event = bitRead(banks[bank].makes, key) ? KEY_PRESSED : KEY_RELEASED;           
               trigger(&keys[bank+key*8], event);
            }
         }
      }
   }
}

byte compact_dr(word dr, teensy_port_reg arrangement[], int size) {
   byte out = 0b0;
   for(int i=0; i<size; i++) {
      word temp = (dr & (1 << (int(arrangement[i].bit - 16))));
      if ((arrangement[i].bit - 16 - i > 0)) 
         out |= temp >> int((arrangement[i].bit - 16 - i));
         else out |= temp << int((abs(arrangement[i].bit - 16 - i)));
   }
   return out;
}

// Send a MIDI message of note ON or note OFF if the key reach top or bottom, change the states of the keys
void trigger(key_fatar_t *key, event_t event) {
   switch (event) {
      case KEY_TOUCHED:
         if(highTrig==true) {
            output_midi(key->midi_note, HITRIG_VELOCITY, true);
            key->state = KEY_IS_DOWN;
            key->played = true;
            return;
         }
         key->state = KEY_IS_GOING_DOWN;
         key->t = 0;
      break;

      case KEY_PRESSED:
         if(key->played == false && highTrig==false) {
            output_midi(key->midi_note, velocity(key->t), true);          
            key->state = KEY_IS_DOWN;
            key->played = true;
         }
         key->t = 0;
      break;

      case KEY_RELEASED:
         key->state = KEY_IS_GOING_UP;
         key->t = 0;
      break;

      case KEY_TOP:
         if(key->played == true) {
            output_midi(key->midi_note, velocity(key->t), false);
            key->state = KEY_IS_UP;
            key->played = false;
         }
         key->t = 0;  
      break;

      default:
      break;
   }
   return;
}

void output_midi(int note, int velocity, bool isOn) {
   String debugMidi = "MIDI " + String(midiOut);
   if (isOn) debugMidi += " ON ";
   else debugMidi += " OFF ";
   debugMidi += String(note) + " V: " + String(velocity);
   Serial.println(debugMidi);
   if (midiOut - 1 < NUM_MIDI_PORTS) {
      if (isOn) midiPort[midiOut - 1]->sendNoteOn(note, velocity, midiCh);
      else midiPort[midiOut - 1]->sendNoteOff(note, velocity, midiCh);
   } else {
      for (int i = 0; i < NUM_MIDI_PORTS; i++) {
         if (isOn) midiPort[i]->sendNoteOn(note, velocity, midiCh);
         else midiPort[i]->sendNoteOff(note, velocity, midiCh);
      }
   }
   return;
}

// Increment the t values of each touched or released key
void kbd_increment() {
   for(int key=0; key<NUM_KEYS; key++) {
      state_t state = keys[key].state;
      if(state == KEY_IS_GOING_UP || state == KEY_IS_GOING_DOWN) {
         keys[key].t++;
      }
   }
   return;
}

// Compute the velocity from the time through a piecewise linear function
// ####################### NEED SOME TUNING #############################
int velocity(int t) {
   if(t<11)
      return 127;
   if(t<28)
      return int(-1.588*t + 142.882);
   if(t<41)
      return int(-1.5385*t + 141.5385);
   if(t<67)
      return int(-0.7692*t + 110.769);
   if(t<111)
      return int(-0.4545*t + 90);
   if(t<181)
      return int(-0.2857*t + 71.4286);
   if(t<330)
      return int(-0.1267*t + 42.8);
   if(t>329)
      return 1;
}

void sws_scan() {
   midiOut = (GPIO9_DR >> 4) & 0x3;
   highTrig = !(digitalReadFast(4));
   lastSwsScan = millis();
}
