// Author: Maciej Wojnicki
// CARDBOARD.LOFIROBOT.COM
// More info: https://cardboard.lofirobot.com/lasercut-robot-head/
// Face Tracing APP for Arduino: 

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Depending on your servo make, the pulse width min and max may vary, you 
// want these to be as small/large as possible without hitting the hard stop
// for max range. You'll have to tweak them as necessary to match the servos you
// have!
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates


int current_byte = 0;
int prev_byte = 0;

const int scale = 10;

int face_x;
int face_y;
int face_z;
int face_yaw;
int face_pitch;
int face_roll;
int face_mouth;

void setup() {

  Serial.begin(9600);

  pwm.begin();
  /*
   * In theory the internal oscillator (clock) is 25MHz but it really isn't
   * that precise. You can 'calibrate' this by tweaking this number until
   * you get the PWM update frequency you're expecting!
   * The int.osc. for the PCA9685 chip is a range between about 23-27MHz and
   * is used for calculating things like writeMicroseconds()
   * Analog servos run at ~50 Hz updates, It is importaint to use an
   * oscilloscope in setting the int.osc frequency for the I2C PCA9685 chip.
   * 1) Attach the oscilloscope to one of the PWM signal pins and ground on
   *    the I2C PCA9685 chip you are setting the value for.
   * 2) Adjust setOscillatorFrequency() until the PWM update frequency is the
   *    expected value (50Hz for most ESCs)
   * Setting the value here is specific to each individual I2C PCA9685 chip and
   * affects the calculations for the PWM update frequency. 
   * Failure to correctly set the int.osc value will cause unexpected PWM results
   */
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

 
}

void loop() {
  if (Serial.available() > 0) {
    current_byte = Serial.read();
    outputs_set();
    prev_byte = current_byte;
  }
}



void outputs_set() {

  //Face: X
  if (prev_byte == 201) {
    face_x = current_byte;
  }
  //Face: Y
  if (prev_byte == 202) {
    face_y = current_byte;
  }
  //Face: Z
  if (prev_byte == 203) {
    face_z = current_byte;
  }
  //Face: Yaw
  if (prev_byte == 204) {
    face_yaw = current_byte;
    servo(0,current_byte);
  }
  // Face: Pitch
  if (prev_byte == 205) {
    face_pitch = current_byte;
    servo(1,current_byte);
  }
  //Face: Roll
  if (prev_byte == 206) {
    face_roll = current_byte;
    servo(2,current_byte);
  }
  //Face: Mouth Openness
  if (prev_byte == 207) {
    face_mouth = current_byte;
    servo(3,current_byte);
  }
}


void servo(int servo_no, int position) {
  pwm.writeMicroseconds(servo_no,USMIN + position * scale);
}


void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);
}