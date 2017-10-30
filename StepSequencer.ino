#include <EEPROM.h>
//This is a copy, here is a change
int buttonPin[4] = {14, 16, 17, 18,};
int ledPin[4] = {3, 5, 7, 9};
boolean lastButtonState[4] = {LOW, LOW, LOW, LOW};
boolean lastState1 = LOW;
boolean lastState2 = LOW;
boolean buttonState1 = LOW;
boolean buttonState2 = LOW;
boolean buttonState[4] = {LOW, LOW, LOW, LOW};
boolean stepState[3][4] = { {true, false, false, false}, {false, true, false, true}, {true, false, true, false}};
int button1 = 31;
int button2 = 32;
int channel = 0;
int counter = 0;
int counterd = 0;

int tempo = 0;

int currentStep = 0;
unsigned long lastStepTime = 0;

void setup() {
  for (int i = 0; i < 6; i++) {
    pinMode(buttonPin[i], INPUT);
  }
  for (int i; i < 4; i++) {
    pinMode(ledPin[i], OUTPUT);
  }
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  loadFromEEPROM();
}

void loop() {
  checkButtonsChannel();
  sequence();
  checkButtons();
  setLeds();
}


void checkButtons() {
  for (int i = 0; i < 4; i++) {
    lastButtonState[i] = buttonState[i];
    buttonState[i] = digitalRead(buttonPin[i]);

    if (buttonState[i] == HIGH && lastButtonState[i] == LOW) {
      if (stepState[channel][i] == false) {
        stepState[channel][i] = true;
      } else if (stepState[channel][i] == true) {
        stepState[channel][i] = false;
      }
      saveToEEPROM();
    }
    
  }
}

void sequence() {

  tempo = analogRead(A1);

  if (millis() > lastStepTime + tempo) {   //if its time to go to the next step...
    //digitalWrite(ledPin[currentStep], LOW );  //turn off the current led
    currentStep = currentStep + 1;         //increment to the next step
    usbMIDI.sendNoteOff(60, 0, 1);
    usbMIDI.sendNoteOff(63, 0, 1);
    usbMIDI.sendNoteOff(65, 0, 1);
    if (currentStep > 3) {
      currentStep = 0;
    }
    if (stepState[0][currentStep] == true) {
      usbMIDI.sendNoteOff(60, 0, 1);
      usbMIDI.sendNoteOn(60, 127, 1);
    } if (stepState[1][currentStep] == true) {
      usbMIDI.sendNoteOff(63, 0, 1);
      usbMIDI.sendNoteOn(63, 127, 1);
    } if (stepState[2][currentStep] == true) {
      usbMIDI.sendNoteOff(65, 0, 1);
      usbMIDI.sendNoteOn(65, 127, 1);
    }
    //digitalWrite(ledPin[currentStep], HIGH); //turn on the new led
    lastStepTime = millis();               //set lastStepTime to the current time
  }
}

void checkButtonsChannel() {
  lastState1 = buttonState1;
  buttonState1 = digitalRead(button1);
  lastState2 = buttonState2;
  buttonState2 = digitalRead(button2);


  if (buttonState1 == HIGH && lastState1 == LOW) {
    if (channel > 0) {
      channel -= 1;
    }
  }
  if (buttonState2 == HIGH && lastState2 == LOW) {
    if (channel < 2) {
      channel += 1;
    }
  }
}

void setLeds() {
  for (int i = 0; i < 4; i++) {
    if (stepState[channel][i] == true) {
      digitalWrite(ledPin[i], HIGH);
    } else if (stepState[channel][i] == false) {
      digitalWrite(ledPin[i], LOW);
    }
    
    if (currentStep == i) {
      digitalWrite(ledPin[i], HIGH);
    } else {
      digitalWrite(ledPin[i], LOW);
    }
  }
}

void saveToEEPROM(){
  counter = 0;
  for (int i = 0; i<3; i++){
    for (int j = 0; j<4; j++){
      EEPROM.write(counter, stepState[i][j]);
      counter++;
    }
  }
}

void loadFromEEPROM(){
  for (int i = 0; i<3; i++){
    for (int j = 0; j<4; j++){
      stepState[i][j] = EEPROM.read(counterd);
      counterd++;
    }
  }
}

