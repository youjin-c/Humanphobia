#include <NewPing.h>

#define SONAR_NUM 1      // Number of sensors.
#define MAX_DISTANCE 300 // Maximum distance (in cm) to ping.
#define TRIGGER0 4
#define ECHO0 7

int led0 = 3;
int led1 = 5;
int led2 = 6;
int led3 = 9;
int led4 = 10; //and 11 -PWM pins
int leds[] = {3,5,6,9,10};
long randNum;


NewPing sonar[SONAR_NUM] = {   // Sensor object array.
  NewPing(TRIGGER0, ECHO0, MAX_DISTANCE) // Each sensor's trigger pin, echo pin, and max distance to ping.
};
String bufferString;
int wall0; //direct input from the sensor
int temp0; //this is the distance from sonar sensor (smoothed)

//unsigned long previousMillis = 0;
int ledState = LOW;
int order = 0;

////////IDLE FUNCTION/////////
float h = 5; //heartrate
int hinc = 1;
int inc = 1;
int brightness = 0;
//////////////////////////////

unsigned long smoothingTimer = 0;
unsigned long idleTimer = 0;
unsigned long freakingTimer = 0;
unsigned long strobingTimer = 0;
unsigned long orderTimer = 0;

int dis0,dis1,dis2,dis3;

void setup() {
  Serial.begin(9600); // Open serial monitor at 9600 baud to see ping results.
  pinMode(TRIGGER0,OUTPUT);
  pinMode(ECHO0,INPUT);

  pinMode(led0, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);


  randomSeed(analogRead(0));
  
  dis0 = 20;
  dis1 = 50;
  dis2 = 80;
  dis3 = 150;
}

void loop() {
  int distance = smoothing(10);
  Serial.println(distance);
   if(distance<dis0){
    ledsOn(led0, led1, led2, led3, led4);
   }else if(distance>=dis0 && distance<dis1){ 
    inOrder(led0, led1, led2, led3, led4, distance);
   }else if(distance>=dis1 && distance<dis2){ 
    strobe(led0, led1, led2, led3, led4, distance);
   }else if(distance>=dis2 && distance<dis3){ 
    freaking(distance);
   }else if(distance>=dis3){ 
    idle(led0, led1, led2, led3, led4, distance); 
   }
}

void inOrder(int a,int b,int c,int d,int e, int h){
  float val = map(h,580,400,10,50);
  if (millis() - orderTimer >= val){
      for(int i=0; i<5; i++){
        if(i==order){
          analogWrite(leds[i],255);
        } else{
          analogWrite(leds[i],0);
          }
      }
     orderTimer = millis();
     order++;
     order %= 5;
  }
}

void idle(int a,int b,int c,int d,int e,int heartrate){
  if(millis() - idleTimer > h){
    analogWrite(a,brightness);
    analogWrite(b,brightness);
    analogWrite(c,brightness);
    analogWrite(d,brightness);
    analogWrite(e,brightness);
    brightness += inc;
    if(brightness > 130 || brightness <= 0){
      inc *= -1;       
    }
    if(brightness > 130){
      //h = map(heartrate,80,20,0,7);
      h += hinc;
      if(h >=  7|| h <= 0){ //heartrate
          hinc *= -1;
      }
    }
    
  idleTimer = millis();
//  Serial.print("h: ");
//  Serial.println(h);
  }
}


void ledsOn(int a,int b,int c,int d,int e){
    digitalWrite(a,HIGH);
    digitalWrite(b,HIGH);
    digitalWrite(c,HIGH);
    digitalWrite(d,HIGH);
    digitalWrite(e,HIGH);
}

void strobe(int a,int b,int c,int d,int e, int heartrate){
  float val = map(heartrate,400,240,0,100);
  if (millis() - strobingTimer >= val){
    
      if(ledState == LOW){
          ledState = HIGH;
        }else{
          ledState = LOW;
        }
      digitalWrite(a,ledState);
      digitalWrite(b,ledState);
      digitalWrite(c,ledState);
      digitalWrite(d,ledState);
      digitalWrite(e,ledState);
      strobingTimer = millis();
    }
}

void freaking(int dis){
  float val = map (dis, 240, 80, 0, 30);
  randNum = random(0,255);
  if (millis() - freakingTimer >= val){ //random(0,20)
      analogWrite(leds[random(0,4)],randNum);
      freakingTimer = millis();
  }
}


///Sonar sensor caculates the distance and weed out 0 values.
int smoothing(int del){
  if (millis() - smoothingTimer >= del){
      wall0 = sonar[0].ping_cm();
      if(wall0!=0){
        temp0 = wall0; 
        return wall0;
      }else
        return temp0;
      smoothingTimer = millis();
    }
}
