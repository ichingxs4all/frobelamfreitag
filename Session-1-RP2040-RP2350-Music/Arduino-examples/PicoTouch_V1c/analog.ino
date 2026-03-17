void checkAnalogPorts(){

   if(analog0.update() && enablePot0){
    if(debug){
      Serial.print("Pot 0 value = ");
      Serial.println(map(analog0.getValue(),0, 4095,0,127));
    }
    MIDI.sendControlChange(controlNo1, map(analog0.getValue(),0, 4095,0,127), midiChannelA);
    usbMIDI.sendControlChange(controlNo1, map(analog0.getValue(),0, 4095,0,127), midiChannelA);
  }
  if(analog1.update() && enablePot1){
    if(debug){
      Serial.print("Pot 1 value = ");
      Serial.println(map(analog1.getValue(),0, 4095,0,127));
    }
    MIDI.sendControlChange(controlNo2, map(analog1.getValue(),0, 4095,0,127), midiChannelA);
    usbMIDI.sendControlChange(controlNo2, map(analog1.getValue(),0, 4095,0,127), midiChannelA);
  }
  if(analog2.update() && enablePot2){
    if(debug){
      Serial.print("Pot 2 value = ");
      Serial.println(map(analog2.getValue(),0, 4095,0,127));
    }
    MIDI.sendControlChange(controlNo3, map(analog2.getValue(),0, 4095,0,127), midiChannelA);
    usbMIDI.sendControlChange(controlNo3, map(analog2.getValue(),0, 4095,0,127), midiChannelA);
  }
}