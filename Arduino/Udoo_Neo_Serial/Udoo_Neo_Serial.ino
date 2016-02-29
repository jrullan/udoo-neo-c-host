/*
 * Test program to talk to a C program on the Linux side
 * of the Udoo NEO.
 * 
 * Notes:
 * 1. Use Serial.print or Serial.write, don't use Serial.println
 * 2. Implement all writing functions, using above note, to add a newline
 *    character to a string use str += '\n' notation. Like sendLine()
 * 3. At the end of the loop() routine, use Serial.flush() 
 * 4. Provide a delay of about 100ms before writing again. 
 *    The C program has a poll interval time of about 50ms.
 */

#include "neohost.h"

 
#define BUFFER_MAX 64
#define PIR 4
#define LED1 13
#define MAX_TRIGGERS 10

byte buff[BUFFER_MAX];
unsigned char *intToStr;
boolean motionDetected = false;

NeoHost neo = NeoHost();


typedef struct myTimer{
  long time;
  long last;
  boolean done;
};

struct myTimer timer1 { .time = 5000, .last = 0, .done = false};
struct myTimer emailDelay { .time = 60000, .last = 0, .done = false};

void setup() {
  Serial.begin(115200);
  pinMode(PIR,INPUT);
  pinMode(LED1,OUTPUT);
  
  timer1.time = 4000;
  emailDelay.time = 60000;
  
  intToStr = (unsigned char*) malloc (sizeof(int)/sizeof(unsigned char));
  if(intToStr != NULL) memset(&intToStr,0,sizeof(int)/sizeof(unsigned char));
  memset(&buff,0,BUFFER_MAX);
}

void loop() {
  
  /*
  if(Serial.available()>0){
    byte bytes = readSerial(buff);
    neo.sendLine("Ard: Bytes received: ",bytes);
    neo.sendLine("Ard: Message received: ",buff,bytes);
  }
  */
<<<<<<< HEAD
  
  if(digitalRead(PIR)==HIGH){
    digitalWrite(LED1,HIGH);    

    if(!motionDetected){         // One Shot
      sendCommand(":Debug","Taking photo");
      //sendCommand(":Webcam","foto00.jpeg");
      //sendCommand(":EmailPhoto","jerullan@yahoo.com,Security Alert,Motion triggered alarm!");
      motionDetected = true;
      resetTimer(&emailDelay);
    }
    
    if(timerDone(&timer1)){ 
      String message = "Motion Detected ";
      message += (millis()-emailDelay.last)/1000;
      sendCommand(":Debug",message);
      resetTimer(&timer1);
    }
    
  }else{
    digitalWrite(LED1,LOW);
=======

  if(digitalRead(PIR)==LOW){

    // Reset motionDetected
>>>>>>> 8359a8c6758f5eea5e18e3ca3695f22eae9e38f5
    if(timerDone(&emailDelay)){
      motionDetected = false;
    }
    digitalWrite(LED1,LOW);
    resetTimer(&timer1);

  }else{

    digitalWrite(LED1,HIGH);     

    if(timerDone(&timer1)){
      int seconds = (millis()-emailDelay.last) / 1000;
      String message = "Motion Detected ";
      message += seconds;
      neo.sendCommand(":Debug",message);
      neo.sendCommand(":Debug",seconds);
      
      if(!motionDetected){         // One Shot
        neo.sendCommand(":Debug","Taking photo");
        //neo.sendCommand(":Webcam","foto00.jpeg");
        //neo.sendCommand(":EmailPhoto","jerullan@yahoo.com,Security Alert,Motion triggered alarm!");
        motionDetected = true;
        resetTimer(&emailDelay);
      }

      resetTimer(&timer1);
    }    
  }

}

//======================================================


// Timer utilities functions

boolean timerDone(struct myTimer* timer){
  if( (millis()-timer->last) >= timer->time){
    timer->done = true;
    return true;
  }
  return false;
}

void resetTimer(struct myTimer* timer){
  timer->last = millis();
  timer->done = false;
}

