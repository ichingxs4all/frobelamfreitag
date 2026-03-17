#include <Adafruit_TinyUSB.h> // Set USB Stack to Adafruit TinyUSB in Tools menu
#include <MIDI.h> // MIDI Library

// Include the library
#include <Control_Surface.h>

// Create a filtered analog object on pin A0:
FilteredAnalog<12,      // Output precision in bits
               2,       // The amount of filtering
               uint16_t // The integer type for the calculations
               >
  analog0 = A0;


// Create a filtered analog object on pin A0:
FilteredAnalog<12,      // Output precision in bits
               2,       // The amount of filtering
               uint16_t // The integer type for the calculations
               >
  analog1 = A1;



// Create a filtered analog object on pin A0:
FilteredAnalog<12,      // Output precision in bits
              2,       // The amount of filtering
               uint16_t // The integer type for the calculations
               >
  analog2 = A2;



#include "TouchyTouch.h"

const int touch_pins[] = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22 };
int touch_velocity[] = { 100,100,100,100,100,100,100,100,100,100,100,100,100,100, 100,100,100,100,100,100,100};
int touch_threshold[]={ 1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000,1000, 1000,1000,1000,1000,1000,1000,1000};

const int touch_count = sizeof(touch_pins) / sizeof(int);

TouchyTouch touches[touch_count];

// USB MIDI object
Adafruit_USBD_MIDI usb_midi;

// Create a new instance of the Arduino MIDI Library,
// and attach usb_midi as the transport.
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, usbMIDI);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

const uint16_t pentatonicTable[54] = { 0,1,3,6,8,10,13,15,18,20,22,25,27,30,32,34,37,39,42,44,46,49,51,54,56,58,61,63,66,68,70,73,75,78,80,82,85,87,90,92,94,97,99,102,104,106,109,111,114,116,118,121,123,126 };

u_int8_t midiChannelA = 1;  //The channel we send MIDI messages to
u_int8_t midiChannelB = 2;  //The channel we send MIDI messages to

const u_int8_t controlChannel = 16; //The MIDI channel we are listing on for commands

bool debug = false;

u_int8_t channel,note;

u_int8_t transpose = 48;

int scale = 0 ; //Defaults to chromatic ( 0), 1 = pentatonic

bool calibrated = 0;

int controlNo1 = 70;
int controlNo2 = 71;
int controlNo3 = 72;

int cctNo = 14;

int split = 127;

bool enablePot0 = false;
bool enablePot1 = false;
bool enablePot2 = false;

bool aftEnable = false;
bool cctEnable = false;
bool noteEnable = true;

int lastAftertouch;

void setup() {

   // Select the correct ADC resolution
  FilteredAnalog<>::setupADC();
  analog0.resetToCurrentValue();
  analog1.resetToCurrentValue();
  analog2.resetToCurrentValue();
 
  if(debug){
  Serial.begin(115200);
  Serial.println("PicoTouch");
  }

  MIDI.setHandleNoteOn(handleNoteOn);  // Put only the name of the function
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleProgramChange(handleProgramChange);
  MIDI.setHandleControlChange(handleControlChange);

  
  usbMIDI.setHandleNoteOn(handleNoteOnUSB);
  usbMIDI.setHandleNoteOff(handleNoteOffUSB);
  usbMIDI.setHandleProgramChange(handleProgramChangeUSB);
  usbMIDI.setHandleControlChange(handleControlChange);

 

  // Initialize MIDI, and listen to all MIDI channels
  // This will also call usb_midi's begin()
  MIDI.begin(MIDI_CHANNEL_OMNI);
  usbMIDI.begin(MIDI_CHANNEL_OMNI);

  pinMode(LED_BUILTIN, OUTPUT);

  for (int i = 0; i < touch_count; i++) {
    touches[i].begin( touch_pins[i] );
  }
  delay(2000);
  doCalibrate();
  calibrated = 1;
  
}

void loop() {
  if(BOOTSEL) doCalibrate();
  checkAnalogPorts();

  for ( int i = 0; i < touch_count; i++) {
    touches[i].update();
    if(touches[i].touched()){
      int touchval = touches[i].raw_value / 75;
      if (touchval > 127) touchval = 127;
      if (touchval!=lastAftertouch){
        if(aftEnable){
          note = i+transpose;
          channel = midiChannelA;
      if (note > split-transpose) channel = midiChannelB;
      MIDI.sendAfterTouch(touchval,channel);
      usbMIDI.sendAfterTouch(touchval,channel);
        }
        if(cctEnable){
      MIDI.sendControlChange(cctNo, touchval, channel);
      usbMIDI.sendControlChange(cctNo, touchval, channel);
        }
      lastAftertouch = touchval;
      delay(10);
      }
    }
    if ( touches[i].pressed() ) {
      touch_velocity[i]= touches[i].raw_value - touches[i].threshold;
      
      //if(debug){
      //Serial.print("Pin pressed ");
      //Serial.println( touches[i].pin );     
      //}
      
      note = i+transpose;
      channel = midiChannelA;
     

      if (note > split-transpose) {
        channel = midiChannelB;
        if(debug) {
          Serial.print("Split to channel ");
          Serial.println(channel);
        }
      }

      if(scale == 0 && noteEnable){
         if(debug) {
        Serial.print("Note On ");
        Serial.print(note);
        Serial.print(" on channel ");
        Serial.println(channel);
      }
      MIDI.sendNoteOn(note, touch_velocity[i], channel);
      usbMIDI.sendNoteOn(note, touch_velocity[i], channel);
      }
      if(scale == 1 && noteEnable) {
        if(debug) {
        Serial.print("Note On ");
        Serial.print(pentatonicTable[note]);
        Serial.print(" on channel ");
        Serial.println(channel);
        }
      MIDI.sendNoteOn(pentatonicTable[note], touch_velocity[i], channel);
      usbMIDI.sendNoteOn(pentatonicTable[note], touch_velocity[i], channel);
      }
      digitalWrite(LED_BUILTIN, HIGH);
    }

    if ( touches[i].released() ) {
      //if(debug){
      //Serial.print("Pin release ");
      //Serial.println( touches[i].pin );
      //}

      note = i+transpose;
      channel = midiChannelA;
      if (note > split-transpose) channel = midiChannelB;

      if(scale == 0 && noteEnable){
         if(debug) {
        Serial.print("Note Off ");
        Serial.print(note);
        Serial.print(" on channel ");
        Serial.println(channel);
      }
      MIDI.sendNoteOff(note, 0, channel);
      usbMIDI.sendNoteOff(note, 0, channel);
      }
      if(scale == 1 && noteEnable){
        if(debug) {
        Serial.print("Note Off ");
        Serial.print(pentatonicTable[note]);
        Serial.print(" on channel ");
        Serial.println(channel);
        }
      MIDI.sendNoteOff(pentatonicTable[note], 0, channel);
      usbMIDI.sendNoteOff(pentatonicTable[note], 0, channel);
      }
      digitalWrite(LED_BUILTIN, LOW);
    }

  }
  MIDI.read(); // read and discard any incoming MIDI messages
  usbMIDI.read();
  delay(5);
}

void doCalibrate(){
  for (int i = 0; i < touch_count; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            touches[i].recalibrate();
            touch_threshold[i] = touches[i].threshold;
            if(debug)Serial.println(touch_threshold[i]);
          }
          if(debug)Serial.println("Calibrated");
          delay(1000);
          digitalWrite(LED_BUILTIN, LOW);
}
