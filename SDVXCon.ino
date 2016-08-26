// Arduino Uno Sound Voltex Controller Script
// By 5argon
// https://github.com/5argon/SDVXArduinoUno
// Contact - @5argondesu

//Sketch -> Include Library > Manage Library... > Search for "Encoder" then install it.
#include <Encoder.h>

//Uncomment to send serial button/knob message to Arduino debug console to test if your connection work or not without the game.
//When debugging, serial message also activates pin 1 (TX), so if your config used pin 1 this may causes problems.
//When actually using, you MUST comment out; As debug message is the same channel as keyboard message.
//(If you forgot, a push of some button might quit your works without saving and shutdown your computer at the same time.) 
//#define SERIAL_DEBUGGING

// -- Setup your button connections to the Arduino Uno pins here. --

//Button switch. The other side should be connecting to GND. (Switch is in pull-up mode when not pressed)
int const BTA = 0;
int const BTB = 12;
int const BTC = 6;
int const BTD = 11;
int const FXL = 5;
int const FXR = 7;
int const START = 13;

//Button's LED. From the pin below to LED + resistor to GND.
int const BTLA = A5;
int const BTLB = A4;
int const BTLC = A2;
int const BTLD = A0;
int const FXLL = 8;
int const FXLR = 10;
int const STARTL = A3;

//Encoders. If you are using a Copal, the white cable is A and the green cable is B. (Red goes to 5V and black goes to GND)
//VOLL is the left encoder, VOLR is the right encoder. Please DO NOT use pin 13 on Uno. (LED connected pin)

//You MUST connect one of each encoder's white or green cable to pin 2 and 3. These 2 pins on an Uno are special external interrupt pins. (It's fast)
//For example, VOLLA = 2 and VOLRA = 3. (both encoder's white). The green cable can choose any pin.
//In short, this will make both encoder good. Without this you will die on Firestorm EXH.

int const VOLLA = 2;
int const VOLLB = 4;
int const VOLRA = 3;
int const VOLRB = 9;

// -- Key mapping --
// Go to http://www.usb.org/developers/hidpage/Hut1_12v2.pdf page 53
// For the list of key specifications from official USB devs.
// Or this for Japanese keyboards http://www2d.biglobe.ne.jp/~msyk/keyboard/layout/usbkeycode.html (look at the last column, it's in HEX)

uint8_t keyBTA = 22; //s
uint8_t keyBTB = 7; //d
uint8_t keyBTC = 14; //k
uint8_t keyBTD = 15; //l

uint8_t keyFXL = 6; //c
uint8_t keyFXR = 16; //m
//日本人の方はこちらです
//uint8_t keyFXL = 0x8A; //無変換
//uint8_t keyFXR = 0x8B; //変換


uint8_t keySTART = 40; //Enter

uint8_t keyVOLLCCW = 20; //q
uint8_t keyVOLLCW = 26; //w
uint8_t keyVOLRCCW = 18; //o
uint8_t keyVOLRCW = 19; //p

// -- More options --

//If you experience double presses of buttons when you pressed only once, try increasing this value.
unsigned const long debounceDelay = 5; 
//Bigger value makes the knob less sensitive. Note that it should not be 0. When it's 0 when you spin quickly the knob will report both CW and CCW directions.
//Value too low can also make the knob go either direction randomly when you just touch the knob.
unsigned const long spinTolerance = 4; 

// ----

Encoder encL(VOLLA,VOLLB);
Encoder encR(VOLRA,VOLRB);
long encLPosition, encRPosition = 0;

uint8_t buf[8] = { 0 }; 

typedef enum KnobState {
  CW,
  CCW,
  Stop
} KnobState;

bool BTAState, BTBState, BTCState, BTDState, FXLState, FXRState, StartState = LOW;
bool BTAHwState, BTBHwState, BTCHwState, BTDHwState, FXLHwState, FXRHwState, StartHwState = LOW;
KnobState VOLLState, VOLRState = Stop;
unsigned long BTADebounce, BTBDebounce, BTCDebounce, BTDDebounce, FXLDebounce, FXRDebounce, StartDebounce = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  setupButton(BTA);
  setupButton(BTB);
  setupButton(BTC);
  setupButton(BTD);
  setupButton(FXL);
  setupButton(FXR);
  setupButton(START);

  setupLED(BTLA);
  setupLED(BTLB);
  setupLED(BTLC);
  setupLED(BTLD);
  setupLED(FXLL);
  setupLED(FXLR);
  setupLED(STARTL);
}

void setupButton(int button)
{
  pinMode(button, INPUT_PULLUP);
}

void setupLED(int led)
{
  pinMode(led, OUTPUT);
}

void checkButton(int button)
{
  int hardwareState = digitalRead(button);

  if(hardwareState != getButtonHwState(button))
  {
    setDebounce(button,millis());
  }

  if((millis() - getDebounce(button)) > debounceDelay)
  {
    if(hardwareState == HIGH)
    {
      if(getButtonState(button) == HIGH)
      {
       lightOff(button);
       setButtonState(button,LOW);
       buttonReleaseAction(button);
      }
    }
    else
    {
      if(getButtonState(button) == LOW)
      {
        lightOn(button);
        releaseKey(button);
        setButtonState(button,HIGH);
        buttonPressAction(button);
      }
    }
  }
  setButtonHwState(button,hardwareState);
}

void lightOff(int inputPin)
{
  digitalWrite(buttonToLight(inputPin), LOW);
}

void lightOn(int inputPin)
{
  digitalWrite(buttonToLight(inputPin), HIGH);  
}

void checkKnob(int knobA, int knobB)
{
  int readPosition;
  if(knobA == VOLLA || knobB == VOLLB)
  {
    readPosition = encL.read();
    
    if(readPosition > (long)(encLPosition + spinTolerance))
    {
      knobLActionCCW();
      encLPosition = readPosition;
    }
    else if(readPosition < (long)(encLPosition - spinTolerance) )
    {
      knobLActionCW();
      encLPosition = readPosition;
    }
    else
    {
      knobLActionStop();
    }
    
  }
  else
  {
    readPosition = encR.read();
    
    if(readPosition > (long)(encRPosition + spinTolerance))
    {
      knobRActionCCW();
      encRPosition = readPosition;
    }
    else if(readPosition < (long)(encRPosition - spinTolerance) )
    {
      knobRActionCW();
      encRPosition = readPosition;
    }
    else
    {
      knobRActionStop();
    }

  }
}

void knobLActionCW()
{
  debugInput("Spin",VOLLA);
  setVolLState(CW);
}

void knobLActionCCW()
{
  debugInput("Spin",VOLLB);
  setVolLState(CCW);
}

void knobRActionCW()
{
  debugInput("Spin",VOLRA);
  setVolRState(CW);
}

void knobRActionCCW()
{
  debugInput("Spin",VOLRB);
  setVolRState(CCW);
}

void knobLActionStop()
{
  setVolLState(Stop);
}

void knobRActionStop()
{
  setVolRState(Stop);
}

void setVolLState(KnobState state)
{
  //only set and send if state changes.
  if(VOLLState != state)
  {
    VOLLState = state;
    serialFromState();
  }
}

void setVolRState(KnobState state)
{
  //only set and send if state changes.
  if(VOLRState != state)
  {
    VOLRState = state;
    serialFromState();
  }
}

void buttonPressAction(int button)
{
  debugInput("Pressed",button);
  serialFromState();
}

void buttonReleaseAction(int button)
{
  debugInput("Released",button);
  serialFromState();
}

void serialFromState()
{
  //USB HID limits to 6 keys, which SDVX has 6 + 2 knobs.
  //We prioritize buttons first as the knobs can be repeatedly pressed and released.

  //Check buttons
  int buttons[7] = {BTA,BTB,BTC,BTD,FXL,FXR,START};
  int slotIndex = 2;
  for( int i = 0; i < 7; i++)
  {
    if(getButtonState(buttons[i]) == HIGH)
    {
      buf[slotIndex] = getKey(buttons[i]);
      slotIndex++;
    }
  }

  //Check knobs, if there are more spaces in the message.

  if(slotIndex < 8)
  {
    if(VOLLState == CW)
    {
      buf[slotIndex] = getKey(VOLLB);
      slotIndex++;
    }
    else if (VOLLState == CCW)
    {
      buf[slotIndex] = getKey(VOLLA);
      slotIndex++;
    }
  }

  if(slotIndex < 8)
  {
    if(VOLRState == CW)
    {
      buf[slotIndex] = getKey(VOLRB);
      slotIndex++;
    }
    else if (VOLRState == CCW)
    {
      buf[slotIndex] = getKey(VOLRA);
      slotIndex++;
    }
  }

  //Fill remainings with zeroes. This will be a key-release also.
  for(int i = slotIndex; i < 8; i++)
  {
    buf[slotIndex] = 0;
  }

  //Send the message
  Serial.write(buf, 8);
  
}

uint8_t getKey(int inputPin)
{
  switch(inputPin)
  {
    case BTA:
    {
      return keyBTA;
      break;
    }
    case BTB:
    {
      return keyBTB;
      break;
    }
    case BTC:
    {
      return keyBTC;
      break;
    }
    case BTD:
    {
      return keyBTD;
      break;
    }
    case FXL:
    {
      return keyFXL;
      break;
    }
    case FXR:
    {
      return keyFXR;
      break;
    }
    case START:
    {
      return keySTART;
      break;
    }
    case VOLLA:
    {
      return keyVOLLCCW;
      break;
    }
    case VOLLB:
    {
      return keyVOLLCW;
      break;
    }
    case VOLRA:
    {
      return keyVOLRCCW;
      break;
    }
    case VOLRB:
    {
      return keyVOLRCW;
      break;
    }
  }
}

void debugInput(String message, int inputPort)
{
  #ifdef SERIAL_DEBUGGING
  Serial.print(message);
  Serial.print(" - ");
  Serial.println(getName(inputPort));
  #endif
}

void setButtonState(int button, bool state)
{
  switch(button)
  {
    case BTA:
    {
      BTAState = state;
      break;
    }
    case BTB:
    {
      BTBState = state;
      break;
    }
    case BTC:
    {
      BTCState = state;
      break;
    }
    case BTD:
    {
      BTDState = state;
      break;
    }
    case FXL:
    {
      FXLState = state;
      break;
    }
    case FXR:
    {
      FXRState = state;
      break;
    }
    case START:
    {
      StartState = state;
      break;
    }
  }
}

bool getButtonState(int button)
{
  switch(button)
  {
    case BTA:
    {
      return BTAState;
      break;
    }
    case BTB:
    {
      return BTBState;
      break;
    }
    case BTC:
    {
      return BTCState;
      break;
    }
    case BTD:
    {
      return BTDState;
      break;
    }
    case FXL:
    {
      return FXLState;
      break;
    }
    case FXR:
    {
      return FXRState;
      break;
    }
    case START:
    {
      return StartState;
      break;
    }
  }
}

String getName(int inputPort)
{
  switch(inputPort)
  {
    case BTA:
    {
      return "BTA";
      break;
    }
    case BTB:
    {
      return "BTB";
      break;
    }
    case BTC:
    {
      return "BTC";
      break;
    }
    case BTD:
    {
      return "BTD";
      break;
    }
    case FXL:
    {
      return "FXL";
      break;
    }
    case FXR:
    {
      return "FXR";
      break;
    }
    case START:
    {
      return "START";
      break;
    }
    case VOLLA:
    {
      return "VOL-L CW";
      break;
    }
    case VOLLB:
    {
      return "VOL-L CCW";
      break;
    }
    case VOLRA:
    {
      return "VOL-R CW";
      break;
    }
    case VOLRB:
    {
      return "VOL-R CCW";
      break;
    }
  }
}

bool getButtonHwState(int button)
{
  switch(button)
  {
    case BTA:
    {
      return BTAHwState;
      break;
    }
    case BTB:
    {
      return BTBHwState;
      break;
    }
    case BTC:
    {
      return BTCHwState;
      break;
    }
    case BTD:
    {
      return BTDHwState;
      break;
    }
    case FXL:
    {
      return FXLHwState;
      break;
    }
    case FXR:
    {
      return FXRHwState;
      break;
    }
    case START:
    {
      return StartHwState;
      break;
    }
  }
}

void setButtonHwState(int button, bool state)
{
  switch(button)
  {
    case BTA:
    {
      BTAHwState = state;
      break;
    }
    case BTB:
    {
      BTBHwState = state;
      break;
    }
    case BTC:
    {
      BTCHwState = state;
      break;
    }
    case BTD:
    {
      BTDHwState = state;
      break;
    }
    case FXL:
    {
      FXLHwState = state;
      break;
    }
    case FXR:
    {
      FXRHwState = state;
      break;
    }
    case START:
    {
      StartHwState = state;
      break;
    }
  }
}

void setDebounce(int button, unsigned long value)
{
  switch(button)
  {
    case BTA:
    {
      BTADebounce = value;
      break;
    }
    case BTB:
    {
      BTBDebounce = value;
      break;
    }
    case BTC:
    {
      BTCDebounce = value;
      break;
    }
    case BTD:
    {
      BTDDebounce = value;
      break;
    }
    case FXL:
    {
      FXLDebounce = value;
      break;
    }
    case FXR:
    {
      FXRDebounce = value;
      break;
    }
    case START:
    {
      StartDebounce = value;
      break;
    }
  }
}

unsigned long getDebounce(int button)
{
  switch(button)
  {
    case BTA:
    {
      return BTADebounce;
      break;
    }
    case BTB:
    {
      return BTBDebounce;
      break;
    }
    case BTC:
    {
      return BTCDebounce;
      break;
    }
    case BTD:
    {
      return BTDDebounce;
      break;
    }
    case FXL:
    {
      return FXLDebounce;
      break;
    }
    case FXR:
    {
      return FXRDebounce;
      break;
    }
    case START:
    {
      return StartDebounce;
    }
  }
}

int buttonToLight(int button)
{
    switch(button)
  {
    case BTA:
    {
      return BTLA;
      break;
    }
    case BTB:
    {
      return BTLB;
      break;
    }
    case BTC:
    {
      return BTLC;
      break;
    }
    case BTD:
    {
      return BTLD;
      break;
    }
    case FXL:
    {
      return FXLL;
      break;
    }
    case FXR:
    {
      return FXLR;
      break;
    }
    case START:
    {
      return STARTL;
      break;
    }
  }
}

void loop() {
  checkButton(BTA);
  checkButton(BTB);
  checkButton(BTC);
  checkButton(BTD);
  checkButton(FXL);
  checkButton(FXR);
  checkButton(START);

  checkKnob(VOLLA, VOLLB);
  checkKnob(VOLRA, VOLRB);
}

void pressKey()
{
  //buf[2] = 128;
  //Serial.write(buf, 8);
}

void releaseKey(int button) 
{
  //buf[0] = 0;
 // buf[2] = 0;
 // Serial.write(buf, 8);  // Release key  
}
