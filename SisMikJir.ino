#include <Wire.h>

#include<LiquidCrystal_I2C.h>


// LCD pin definitions
LiquidCrystal_I2C lcd(0x3F, 16, 2);  // Address 0x3F, 16 columns, 2 rows

// Pin definitions
const int buzzerPin = 6;
const int button1Pin = 4;
const int button2Pin = 3;
const int button3Pin = 2;

// Button debounce time (ms)
const unsigned long debounceDelay = 50;

// Global variables
byte hours = 00, minutes = 0, seconds = 0;
byte timerHours = 0, timerMinutes = 0, timerSeconds = 0;
byte alarmHours = 6, alarmMinutes = 0;
bool alarmEnabled = false;
bool alarmTriggered = false;

enum State
{
  CLOCK,
  SET_TIME,
  TIMER,
  ALARM
};

enum Set
{
  HOURS_SET,
  MINUTES_SET
};

State currentState = CLOCK;
Set setState = HOURS_SET;

volatile int timercount = 0;
volatile int timercount_enable = 0;

void setup() {
  lcd.init();            // initialize the LCD 
  lcd.begin(16, 2);     // Initialize the LCD with 16 columns and 2 rows
  lcd.backlight();       // Turn on the backlight
  
  pinMode(buzzerPin, OUTPUT);
  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(button3Pin), changeState, FALLING);
  
  TCCR1A = 0;             // Set TCCR1A register ke 0
  TCCR1B = 0b00000100; // Prescaler = 256
  TCNT1 = 0x0BDC;              // Set counter timer ke 0
  TIMSK1 = (1 << TOIE1);  // Enable overflow interrupt
}

ISR(TIMER1_OVF_vect) {
  seconds++;
  if (seconds == 60) {
    seconds = 0;
    minutes++;
    if (minutes == 60) {
      minutes = 0;
      hours++;
      if (hours == 24) {
        hours = 0;
      }
    }
  }
  TCNT1 = 0xBDC;
}
void loop() {
  displayTimeOnLCD(hours, minutes, seconds);
}

void changeState() {
  switch (currentState) {
    case CLOCK:
      currentState = SET_TIME;
      break;
    case SET_TIME:
      currentState = TIMER;
    break;
    case TIMER:
      currentState = ALARM;
    break;
    case ALARM:
      currentState = CLOCK; 
    break;
  }
}

void displayTimeOnLCD(byte displayHours, byte displayMinutes, byte displaySeconds) {
  lcd.setCursor(0, 0);
  lcd.print("CLOCK :>");
  lcd.setCursor(0, 1);
  if (displayHours < 10) {
    lcd.print("0");
  }
  lcd.print(displayHours);
  lcd.print(":");
  if (displayMinutes < 10) {
    lcd.print("0");
  }
  lcd.print(displayMinutes);
  lcd.print(":");
  if (displaySeconds < 10){
    lcd.print("0");
  }
  lcd.print(displaySeconds);
}

void setTime(){
  lcd.setCursor(0,0);
  lcd.print("SET CLOCK!");
  if (digitalRead(button2Pin) == LOW){
    delay(debounceDelay);
    if (digitalRead(button2Pin) == LOW){
      switch(setState){
        case HOURS_SET:
          setState = MINUTES_SET;
          break;
        case MINUTES_SET:
          setState = HOURS_SET;
          break;
      }
    }
  }
  if(digitalRead(button1Pin) == LOW){
    delay(debounceDelay);
    if(digitalRead(button1Pin) == LOW){
      if(setState == HOURS_SET){
        if (hours < 23){
          hours++;
        }
        else{
          hours = 0;
        }
      }
      else if(setState == MINUTES_SET){
        if(minutes < 59){
          minutes++;
        }
        else{
          minutes = 0;
        }
      }
    }
  }
  displayTimeOnLCD(hours, minutes, seconds);
}

void runTimer(){
  lcd.setCursor(0,0);
  lcd.print("STOPWATCH ^O^");
  if (digitalRead(button2Pin) == LOW){
    delay(debounceDelay);
    if (digitalRead(button2Pin) == LOW){
      timercount = 0;
    }
  }
  if(digitalRead(button1Pin) == LOW){
    delay(debounceDelay);
    if(digitalRead(button1Pin) == LOW){
      if(timercount_enable == 0){
          timercount_enable = 1;
      }
      else{
          timercount_enable = 0;
      }
    }
  } 
  displayTimeOnLCD(timercount/3600, (timercount%3600)/60, (timercount%3600)%60); 
}

void setAlarm(){
  lcd.setCursor(0,0);
  lcd.print("ALARM -_-");
  if (digitalRead(button2Pin) == LOW){
    delay(debounceDelay);
    if (digitalRead(button2Pin) == LOW){
      timercount = 0;
    }
  }
  if(digitalRead(button1Pin) == LOW){
    delay(debounceDelay);
    if(digitalRead(button1Pin) == LOW){
      if(timercount_enable == 0){
          timercount_enable = 1;
      }
      else{
          timercount_enable = 0;
      }
    }
}
}