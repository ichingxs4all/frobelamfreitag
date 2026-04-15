// Author: Maciej Wojnicki
// CARDBOARD.LOFIROBOT.COM
// More info: https://cardboard.lofirobot.com/lasercut-robot-head/
// Face Tracing APP for Arduino: 

#include <Servo.h>

int current_byte = 0;
int prev_byte = 0;

Servo serwo1;
Servo serwo2;
Servo serwo3;
Servo serwo4;

int face_x;
int face_y;
int face_z;
int face_yaw;
int face_pitch;
int face_roll;
int face_mouth;

void setup() {
  Serial1.begin(9600);
  serwo1.attach(16);  // attaches the servo on GIO16 to the servo object
  serwo2.attach(17);  
  serwo3.attach(18);
  serwo4.attach(19);

}

void loop() {
  if (Serial1.available() > 0) {
    current_byte = Serial1.read();
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
    servo1(current_byte);
  }
  // Face: Pitch
  if (prev_byte == 205) {
    face_pitch = current_byte;
    servo2(current_byte);
  }
  //Face: Roll
  if (prev_byte == 206) {
    face_roll = current_byte;
    servo4(current_byte);
  }
  //Face: Mouth Openness
  if (prev_byte == 207) {
    face_mouth = current_byte;
    servo3(current_byte);
  }
}

void servo1(int position) {
  serwo1.write(position * 1.8);
}
void servo2(int position) {
  serwo2.write(position * 1.8);
}
void servo3(int position) {
  serwo3.write(position * 1.8);
}
void servo4(int position) {
  serwo4.write(position * 1.8);
}