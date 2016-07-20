int segmentA = 2;
int segmentB = 6;
int segmentC = 9;
int segmentD = 11;
int segmentE = 12;
int segmentF = 3;
int segmentG = 8;

int digitPos[4] = {A1, 4, 5, 7};
int potPin = A5;
int buzzPin = 10;
int buttonPin = A0;

// settings
const int tickDuration = 50; // duration of tick in ms
const int tempoChangeWait = 500; // how long to wait until apply tempo change
const int tempoDisplayDelay = 700; // delay to display current tempo

int curTempo = 95;
int lastTempo;
long showDisplayTime;
int delayInTicks;
int isEnabled = 0;
long time = 0;

int tickState = 0;
long lastTickTime = 0;
int tickNo = 0; // tick number
int animateFrame = 0;
boolean isOn = false;
int lastButtonState = 0;
int buttonState = 0;
long buttonPressedTime;

byte numbers[13] = 
{
  B1000000, // 0
  B1111001, // 1
  B0100100, // 2
  B0110000, // 3
  B0011001, // 4
  B0010010, // 5
  B0000010, // 6
  B1111000, // 7
  B0000000, // 8
  B0010000,  // 9
  // symbols for animation
  B1001111,
  B0111111,
  B1111001
};

void setup() {
  pinMode(segmentA,OUTPUT);
  pinMode(segmentB,OUTPUT);
  pinMode(segmentC,OUTPUT);
  pinMode(segmentD,OUTPUT);
  pinMode(segmentE,OUTPUT);
  pinMode(segmentF,OUTPUT);
  pinMode(segmentG,OUTPUT);

  pinMode(digitPos[0],OUTPUT);
  pinMode(digitPos[1],OUTPUT);
  pinMode(digitPos[2],OUTPUT);
  pinMode(digitPos[3],OUTPUT);
  pinMode(13,OUTPUT);
  pinMode(buzzPin,OUTPUT);
  pinMode(buttonPin, INPUT);

  Serial.begin(9600);
}

void displayDigit(int pos, int digit) {
   int number =  numbers[digit];
   
   digitalWrite(digitPos[0],LOW);
   digitalWrite(digitPos[1],LOW);
   digitalWrite(digitPos[2],LOW);
   digitalWrite(digitPos[3],LOW);
   
   digitalWrite(digitPos[pos],LOW);
   digitalWrite(segmentA, bitRead(number, 0) );
   digitalWrite(segmentB, bitRead(number, 1) );
   digitalWrite(segmentC, bitRead(number, 2) );
   digitalWrite(segmentD, bitRead(number, 3) );
   digitalWrite(segmentE, bitRead(number, 4) );
   digitalWrite(segmentF, bitRead(number, 5) );
   digitalWrite(segmentG, bitRead(number, 6) );
   digitalWrite(digitPos[pos],HIGH);
};

void displayNumber(int num) {
  int dig1 = num/1000;
  num = num - dig1 * 1000;
  int dig2 = num / 100;
  num = num - dig2 * 100;
  int dig3 = num / 10;
  int dig4 = num - dig3 * 10;
  
  // display only one digit per frame
  if (time%3==0 && dig2>0) displayDigit(1,dig2);
  if (time%3==1) displayDigit(2,dig3);
  if (time%3==2) displayDigit(3,dig4);
};

void tick(int isStart) {
  if (isStart==0) {
    tickState = 0;
    analogWrite(buzzPin,0);
    digitalWrite(13,LOW);
  }
  else {
    tickState = 1;
    analogWrite(buzzPin, (tickNo==0 ? 150 : 2));
    digitalWrite(13,HIGH);
    lastTickTime = millis();   
    tickNo++;
    if (tickNo>3) tickNo = 0;
    if (tickNo % 2 == 0) animateFrame = 0;
  }
  
};

void animate() {
  if (animateFrame>9) animateFrame = 0;
  switch (animateFrame) {
   case 0: displayDigit(0,10); break;
   case 1: displayDigit(0,11); break;
   case 2: displayDigit(1,11); break;
   case 3: displayDigit(2,11); break;
   case 4: displayDigit(3,11); break;
   case 5: displayDigit(3,12); break;
   case 6: displayDigit(3,11); break;
   case 7: displayDigit(2,11); break;
   case 8: displayDigit(1,11); break;
   case 9: displayDigit(0,11); break;
  }
}

void reset() {
  lastTickTime = animateFrame = tickNo = 0;
  analogWrite(buzzPin,0);
}

void loop() {
  time = millis();
  
  // handle button (avoiding fake signals)
  buttonState = digitalRead(buttonPin);
  if (time-buttonPressedTime > 200 && lastButtonState!=buttonState) {
    if (buttonState==1) isOn = !isOn;
    if (isOn==false) reset(); // reset all counters
  }
  if (buttonState!=lastButtonState) {
    buttonPressedTime = time;
    lastButtonState = buttonState;
  }
  
  
  digitalWrite(13, isOn);

  curTempo = map(analogRead(potPin),0,1023,60,200);

  if (abs(lastTempo-curTempo)>2) { // show tempo on display, if delta = 3 or more
    lastTempo = curTempo;
    showDisplayTime = time;  
  }
  
  delayInTicks = 60000 / curTempo;

  if (isOn) {
    if ((time - lastTickTime) > delayInTicks && tickState == 0) tick(1);
    if ((time - lastTickTime) > tickDuration && tickState == 1) tick(0); 
    
    // animate display
    if ((time - lastTickTime) + delayInTicks * (tickNo % 2)  > delayInTicks / 5 * (animateFrame+1) && animateFrame<9) {
        animateFrame++;
    }
  }
  
  if (!isOn || time - showDisplayTime < tempoDisplayDelay) displayNumber(curTempo);
  else animate(); 

}
