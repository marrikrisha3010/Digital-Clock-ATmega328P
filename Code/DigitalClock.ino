/*
====================================================
        Digital Clock using ATmega328P
====================================================

Microcontroller : ATmega328P-PU
Display         : 4 Digit Seven Segment Display
Clock Speed     : 16 MHz External Crystal

Features:
- 24 Hour Digital Clock
- Hour and Minute Adjustment
- Start Button
- Seconds View using Long Press
- Standalone ATmega328P Operation

Author:
Krisha Janardhan Marri

====================================================
*/


// ---------------- DISPLAY PIN CONFIGURATION ----------------

const byte segPins[7] = {2,3,4,5,6,7,8};
const byte digitPins[4] = {9,10,11,12};


// ---------------- BUTTON CONNECTIONS ----------------

const byte hourBtn   = A0;
const byte minuteBtn = A1;
const byte startBtn  = A2;


// ---------------- SEVEN SEGMENT DIGIT DATA ----------------

byte digits[10][7] =
{
  {0,0,0,0,0,0,1},   // 0
  {1,0,0,1,1,1,1},   // 1
  {0,0,1,0,0,1,0},   // 2
  {0,0,0,0,1,1,0},   // 3
  {1,0,0,1,1,0,0},   // 4
  {0,1,0,0,1,0,0},   // 5
  {0,1,0,0,0,0,0},   // 6
  {0,0,0,1,1,1,1},   // 7
  {0,0,0,0,0,0,0},   // 8
  {0,0,0,0,1,0,0}    // 9
};


// ---------------- CLOCK VARIABLES ----------------

int hours = 0;
int minutes = 0;
int secondsCounter = 0;

bool running = false;


// ---------------- TIMER VARIABLES ----------------

unsigned long previousMillis = 0;


// ---------------- SECOND DISPLAY CONTROL ----------------

bool showSeconds = false;
unsigned long showSecondsStart = 0;


// ---------------- BUTTON PRESS CONTROL ----------------

unsigned long buttonPressTime = 0;
bool buttonPreviouslyPressed = false;


// Previous button states for detecting single clicks

bool prevHourState   = HIGH;
bool prevMinuteState = HIGH;
bool prevStartState  = HIGH;



void setup()
{

  // Configure segment pins
  
  for(byte i=0; i<7; i++)
  {
    pinMode(segPins[i], OUTPUT);
  }


  // Configure digit selection pins

  for(byte i=0; i<4; i++)
  {
    pinMode(digitPins[i], OUTPUT);
  }


  // Enable internal pull-up resistors

  pinMode(hourBtn, INPUT_PULLUP);
  pinMode(minuteBtn, INPUT_PULLUP);
  pinMode(startBtn, INPUT_PULLUP);

}



void loop()
{

  // Read button inputs

  bool hourState   = digitalRead(hourBtn);
  bool minuteState = digitalRead(minuteBtn);
  bool startState  = digitalRead(startBtn);



  // =================================================
  //              CLOCK SETTING MODE
  // =================================================

  if(running == false)
  {

    if(prevHourState == HIGH && hourState == LOW)
    {
      hours++;

      if(hours >= 24)
        hours = 0;
    }


    if(prevMinuteState == HIGH && minuteState == LOW)
    {
      minutes++;

      if(minutes >= 60)
        minutes = 0;
    }


    if(prevStartState == HIGH && startState == LOW)
    {
      running = true;

      previousMillis = millis();
    }

  }



  // Store previous button values

  prevHourState   = hourState;
  prevMinuteState = minuteState;
  prevStartState  = startState;



  // =================================================
  //              CLOCK RUNNING MODE
  // =================================================

  if(running)
  {

    if(millis() - previousMillis >= 1000)
    {

      previousMillis += 1000;

      secondsCounter++;


      if(secondsCounter == 60)
      {

        secondsCounter = 0;

        minutes++;


        if(minutes == 60)
        {
          minutes = 0;
          hours++;
        }


        if(hours == 24)
        {
          hours = 0;
        }

      }

    }



    // Long press start button to display seconds

    if(digitalRead(startBtn) == LOW)
    {

      if(buttonPreviouslyPressed == false)
      {
        buttonPressTime = millis();
        buttonPreviouslyPressed = true;
      }


      if(millis() - buttonPressTime > 1500)
      {
        showSeconds = true;

        showSecondsStart = millis();
      }

    }

    else
    {
      buttonPreviouslyPressed = false;
    }



    // Automatically return to normal display

    if(showSeconds && millis()-showSecondsStart > 5000)
    {
      showSeconds = false;
    }

  }



  // Refresh display continuously

  displayNumber();

}



// =================================================
//        FUNCTION TO DISPLAY ONE DIGIT
// =================================================

void displayDigit(byte digit)
{

  for(byte i=0; i<7; i++)
  {
    digitalWrite(segPins[i], digits[digit][i]);
  }

}



// =================================================
//        FUNCTION FOR DISPLAY MULTIPLEXING
// =================================================

void displayNumber()
{

  int displayValue[4];


  if(showSeconds)
  {
    displayValue[0] = minutes / 10;
    displayValue[1] = minutes % 10;

    displayValue[2] = secondsCounter / 10;
    displayValue[3] = secondsCounter % 10;
  }

  else
  {
    displayValue[0] = hours / 10;
    displayValue[1] = hours % 10;

    displayValue[2] = minutes / 10;
    displayValue[3] = minutes % 10;
  }



  for(byte digit=0; digit<4; digit++)
  {

    // Disable all digits

    for(byte i=0; i<4; i++)
    {
      digitalWrite(digitPins[i], LOW);
    }


    displayDigit(displayValue[digit]);


    // Enable required digit

    digitalWrite(digitPins[digit], HIGH);


    delay(3);

  }

}
