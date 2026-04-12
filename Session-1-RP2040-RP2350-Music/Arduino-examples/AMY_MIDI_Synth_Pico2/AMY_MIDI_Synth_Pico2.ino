#include <AMY-Arduino.h>

#define LED_BUILTIN 3 //for PicoSynth

bool enable_chorus = false;
bool enable_echo = false;
bool enable_reverb = true;

static long last_millis = 0;
static const long millis_interval = 250;
static bool led_state = 0;

int potA,potB,potC,prevA,prevB,prevC;

void setup_polyphony() {
 
  amy_event e = amy_default_event();
  e.reset_osc = RESET_AMY;
  amy_add_event(&e);

  e = amy_default_event();
  e.synth = 1;
  e.patch_number = 1;
  e.num_voices = 6;
  amy_add_event(&e);

  // Reconfigure synth 2 with a bass sound
  e = amy_default_event();
  e.synth = 2;
  e.patch_number = 30;  // Juno A47 Funky Bass
  e.num_voices = 6;
  amy_add_event(&e);

  
  if(enable_reverb) config_reverb(0.5f, 0.85f, 0.5f, 3000.0f);
  
  if(enable_chorus) config_chorus(0.75f, 320, 0.5f, 0.5f);

  if(enable_echo) config_echo(0.5f, 150.0f, 160.0f, 0.5f, 0.0f); 

}

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 1);

  amy_config_t amy_config = amy_default_config();
 
  amy_config.features.startup_bleep = 1;
  // Install the default_synths on synths (MIDI chans) 1, 2, and 10 (this is the default).
  amy_config.features.default_synths = 1;

  // Pins for i2s board
  // On Pi Pico (RP2040, RP2350), i2s_lrc has to be i2s_bclk + 1, otherwise code will stop on an assert.
  amy_config.audio = AMY_AUDIO_IS_I2S;
  amy_config.i2s_mclk = 7;
  amy_config.i2s_bclk = 8;
  amy_config.i2s_lrc = 9;
  amy_config.i2s_dout = 10;
  amy_config.i2s_din = 11;

  // If you want MIDI over UART (5-pin or 3-pin serial MIDI)
  amy_config.midi = AMY_MIDI_IS_UART;
  //amy_config.midi = AMY_MIDI_IS_USB_GADGET;

  // Pins for UART MIDI
  amy_config.midi_out = 4;
  amy_config.midi_in = 5;

  amy_start(amy_config);

  setup_polyphony();

}


void loop() {
  // Your loop() must contain this call to amy:
  amy_update();
  blink_led();
  //readPots();
 
}

void blink_led(){
 // Flash on-board LED every 250ms
  int now_millis = millis();
  if ((now_millis - last_millis) > millis_interval) {
    last_millis = now_millis;
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state);  // turn the LED on (HIGH is the voltage level)
  }
}


void readPots(){
  int rawA= analogRead(A0);

  int rawB= analogRead(A1);

  int rawC= analogRead(A2);
 

  if(potA != prevA) {
    send_value(0, potA);
    prevA==potA;
  }
  if(potB != prevB) {
  send_value(1, potB);
  prevB==potB;
  }
  if(potC != prevC) {
    send_value(2, potC);
    prevC==potC;
  }

  Serial.print(potA);
  Serial.print(" , ");
  Serial.print(potB);
  Serial.print(" , ");
  Serial.println(potC);


}

void send_value(int parameter, int value ){


}