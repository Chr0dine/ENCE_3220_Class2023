#include <Arduino_FreeRTOS.h>

#define BUTTON_1  2
#define BUTTON_2  3
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER    6

#define DATA      9
#define LATCH     8
#define CLOCK     7

#define DIGIT_4   10
#define DIGIT_3   11
#define DIGIT_2   12
#define DIGIT_1   13

volatile int gTime=0;
const int DEBOUNCE_DELAY_MS = 50;
bool lastButtonState = HIGH;
volatile unsigned char firstDigit =0;
volatile unsigned char secondDigit =0;
volatile unsigned char thirdDigit =0;
volatile unsigned char fourthDigit =0;
volatile int count = 387;
static unsigned char hexZero = 0x3f;
static unsigned char hexOne = 0x06;
static unsigned char hexTwo = 0x5b;
static unsigned char hexThree = 0x4f;
static unsigned char hexFour = 0x66;
static unsigned char hexFive = 0x6d;
static unsigned char hexSix = 0x7d;
static unsigned char hexSeven = 0x07;
static unsigned char hexEight = 0x7f;
static unsigned char hexNine = 0x6f;
volatile bool buttonTwoFlag = false;

void setup() {
  Serial.begin(9600);
  // LEDs Pins
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  // Button Pins
  pinMode(BUTTON_1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_1), Button_1_ISR, RISING);
  pinMode(BUTTON_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_2), Button_2_ISR, RISING);

  // Buzer Pins
  pinMode(BUZZER, OUTPUT);

  // 7-Seg Display
  pinMode(DIGIT_1, OUTPUT);
  pinMode(DIGIT_2, OUTPUT);
  pinMode(DIGIT_3, OUTPUT);
  pinMode(DIGIT_4, OUTPUT);

  // Shift Register Pins
  pinMode(LATCH, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(DATA, OUTPUT);

  disp_off();
  // Timer initialization
  noInterrupts();           // disable all interrupts
  //timer 1 initialization
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 62500;            // compare match register 16MHz/256
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= (1 << CS12);    // 256 prescaler 
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt

//timer 2 initialization
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A = 62500;            // compare match register 16MHz/256
  TCCR2B |= (1 << WGM12);   // CTC mode
  TCCR2B |= (1 << CS22) | (1 <<CS20); //1024 prescaler
  TIMSK2 |= (1 << OCIE2A);  // enable timer compare interrupt
  interrupts(); 
}

void disp_off(){
  digitalWrite(DIGIT_1,HIGH);
  digitalWrite(DIGIT_2,HIGH);
  digitalWrite(DIGIT_3,HIGH);
  digitalWrite(DIGIT_4,HIGH);  
}

unsigned char numToHEX(int num){
 unsigned char outputNum = 0;
if(num==0){
    outputNum = hexZero;
  }else if(num==1){
    outputNum = hexOne;
  }else if(num==2){
    outputNum = hexTwo;
  }else if(num==3){
    outputNum = hexThree;
  }else if(num==4){
    outputNum = hexFour;
  }else if(num==5){
    outputNum = hexFive;
  }else if(num==6){
    outputNum = hexSix;
  }else if(num==7){
    outputNum = hexSeven;
  }else if(num==8){
    outputNum = hexEight;
  }else if(num==9){
    outputNum = hexNine;
  }
  return outputNum;
}

void Display(unsigned char num){
    digitalWrite(LATCH, LOW);
  shiftOut(DATA, CLOCK, MSBFIRST, num);
  digitalWrite(LATCH, HIGH);
 
}

int countToTime(int count){
  int minutes = 100 * int((count/60)%60);
  int seconds = int(count%60);
  return minutes + seconds;
}

void Button_1_ISR()
{
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
    
  buttonState = digitalRead(BUTTON_1);
  if(buttonState && !buttonTwoFlag){
     count++;
  }
}

void Button_2_ISR()
{ 
  unsigned int buttonState = 0;  // variable for reading the pushbutton status
  
  buttonState = digitalRead(BUTTON_2);
  if(buttonState){
    buttonTwoFlag=true;
    digitalWrite(RED_LED,LOW);
  }else{
  buttonTwoFlag = false;   
  digitalWrite(RED_LED,HIGH);
  }
    digitalWrite(GREEN_LED, HIGH); 
  
}

ISR(TIMER1_COMPA_vect)          // timer compare interrupt service routine
{
  if(buttonTwoFlag && count>0){
  count--;
  }
  
//  Serial.println(buttonTwoFlag);  
}

ISR(TIMER2_COMPA_vect)          // timer compare interrupt service routine
{
    
  gTime = countToTime(count);
  if(count==0){
    digitalWrite(BUZZER,HIGH);
    digitalWrite(RED_LED,HIGH);
    digitalWrite(GREEN_LED,LOW);    
    digitalWrite(BUZZER,LOW); 
  }else{
  firstDigit = (int)(gTime%10);
  Display(numToHEX(firstDigit));
  digitalWrite(DIGIT_4,LOW);
  disp_off();
  secondDigit = (int)(gTime/10)%10;
  Display(numToHEX(secondDigit));
  digitalWrite(DIGIT_3,LOW);
  disp_off();
  thirdDigit = (int)((gTime/100)%10);
  Display(numToHEX(thirdDigit)| (1<<7));
  digitalWrite(DIGIT_2,LOW);
  disp_off();
  fourthDigit = (int)((gTime/1000)%10);
  Display(numToHEX(fourthDigit));
  digitalWrite(DIGIT_1,LOW);
  disp_off();
  }
}

void loop() {
  // IDLE
}