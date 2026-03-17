void handleProgramChangeUSB(byte channel, byte number){
  MIDI.sendProgramChange(number, channel);
}

void handleNoteOnUSB(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOn(pitch,velocity,channel);
}

void handleNoteOffUSB(byte channel, byte pitch, byte velocity) {
  MIDI.sendNoteOff(pitch,velocity,channel);
}

void handleProgramChange(byte channel, byte number){
  usbMIDI.sendProgramChange(number, channel);
}

void handleNoteOn(byte channel, byte pitch, byte velocity) {
  usbMIDI.sendNoteOn(pitch,velocity,channel);
}

void handleNoteOff(byte channel, byte pitch, byte velocity) {
  usbMIDI.sendNoteOff(pitch,velocity,channel);
}

void handleControlChange(byte channel, byte number, byte value) {
   MIDI.sendControlChange(number, value, channel);

  if (channel == controlChannel) {
    digitalWrite(LED_BUILTIN, HIGH);
    switch (number) {
      
      case 13:  //debug on - off
        debug = value;
        if(debug) Serial.println("Debug ON");
        break;

      case 14:  //split point
        split = value;
        if (split > 127) split = 127;
        if (debug) {
          Serial.print("Split = ");
          Serial.println(split);
        }
        break;
      
      case 16:  //scale
        if (value == 0) {
          scale = 0;
          if (debug) Serial.println("Scale Chromatic");
        }
        if (value == 1) {
          scale = 1;
          if (debug) Serial.println("Scale Pentatonic ");
        }
        break;

      case 17:  //transpose
        transpose = value;
        if (transpose > 106) transpose = 106;
        if (debug) {
          Serial.print("Transpose = ");
          Serial.println(transpose);
        }
        break;

      case 18:  //Set MIDI out channel
        midiChannelA = value;
        if (midiChannelA > 15) midiChannelA = 15;
        if (debug) {
          Serial.print("MIDI Channel A = ");
          Serial.println(midiChannelA);
        }
        break;

        case 15:  //Set MIDI out channel
        midiChannelB = value;
        if (midiChannelB > 15) midiChannelB = 15;
        if (debug) {
          Serial.print("MIDI Channel B = ");
          Serial.println(midiChannelB);
        }
        break;

      case 19:  //Recalibratie Touch pins
        if (value == 1 && calibrated == 0) {
          doCalibrate();
          calibrated = 1;
        }
        if (value == 0 && calibrated == 1) calibrated = 0;
        digitalWrite(LED_BUILTIN, LOW);
        break;

      case 20:  //MIDI Thru on - off
        if (value == 0) {
          MIDI.turnThruOff();
          if (debug) Serial.println("MIDI Thru OFF");
        }
        if (value == 1) {
          MIDI.turnThruOn();
          if (debug) Serial.println("MIDI Thru ON ");
        }
        break;

      case 21:  //set controller no.
        if (value == 0) {
          if (debug) Serial.println("POT 0 OFF");
          enablePot0 = false;
        }
        if (value > 0 && value <= 127) {
          controlNo1 = value;
          enablePot0 = true;
          if (debug) {
            Serial.print("POT 0 CC No = ");
            Serial.println(value);
          }
        }
        break;
      case 22:  //set controller no.
        if (value == 0) {
          if (debug) Serial.println("POT 1 OFF");
          enablePot1 = false;
        }
        if (value > 0 && value <= 127) {
          controlNo2 = value;
          enablePot1 = true;
          if (debug) {
            Serial.print("POT 1 CC No = ");
            Serial.println(value);
          }
        }
        break;
      case 23:  //set controller no.
        if (value == 0) {
          if (debug) Serial.println("POT 2 OFF");
          enablePot2 = false;
        }
        if (value > 0 && value <= 127) {
          controlNo3 = value;
          enablePot2 = true;
          if (debug) {
            Serial.print("POT 2 CC No = ");
            Serial.println(value);
          }
        }
        break;

      case 24:  //AfterTouch on / off
        if (value == 0) {
          if (debug) Serial.println("AfterTouch OFF");
          aftEnable = false;
        }
        if (value == 1) {
          aftEnable = true;
          if (debug) {
            Serial.println("AfterTouch ON");
          }
        }
        break;

      case 25:  //Control Change Touch on / off
        if (value == 0) {
          if (debug) Serial.println("Control Change Touch OFF");
          cctEnable = false;
        }
        if (value == 1) {
          cctEnable = true;
          if (debug) {
            Serial.println("Control Change Touch ON ");
          }
        }
        break;

      case 26:  //Note Touch on / off
        if (value == 0) {
          if (debug) Serial.println("Note Touch OFF");
          noteEnable = false;
        }
        if (value == 1) {
          noteEnable = true;
          if (debug) {
            Serial.println("Note Touch ON");
          }
        }
        break;

      case 27:  //set controller no.
        if (value == 0) {
          if (debug) Serial.println("Control Change Touch Off");
          cctEnable = false;
        }
        if (value > 0 && value <= 127) {
          cctNo = value;
          cctEnable = true;
          if (debug) {
            Serial.print("Control Change Touch No = ");
            Serial.println(value);
          }
        }
        break;

      default:
        break;
    }
  }
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);
}
