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
#include "neotimer.h"
 
#define BUFFER_MAX 64
#define PIR 4
#define LED1 13
#define MAX_TRIGGERS 10

byte buff[BUFFER_MAX];
unsigned char *intToStr;
boolean motionDetected = false;
int loops = 0;

NeoHost neo = NeoHost();
Neotimer neoTime = Neotimer(10000);

void setup() {
  Serial.begin(115200);
  pinMode(PIR,INPUT);
  pinMode(LED1,OUTPUT);
  digitalWrite(LED1,LOW);
  
  intToStr = (unsigned char*) malloc (sizeof(int)/sizeof(unsigned char));
  if(intToStr != NULL) memset(&intToStr,0,sizeof(int)/sizeof(unsigned char));
  memset(&buff,0,BUFFER_MAX);

  delay(1000);
  neo.sendCommand(":Email","jerullan@yahoo.com,Udoo Neo, Download Successful!");
  delay(1000);
}

void loop() {
  
  if(neoTime.done()){
    String mess = "Loop: ";
    mess += loops;
    //neo.sendCommand(":Log",mess);
    neo.sendCommand(":Debug",mess);
    loops++;
    if(digitalRead(LED1)==LOW){
      digitalWrite(LED1,HIGH);
      //neo.sendCommand(":Debug","ON");
    }else{
      digitalWrite(LED1,LOW);
      //neo.sendCommand(":Debug","OFF");
    }
    neoTime.reset();
  }
  
  /*
  if(digitalRead(PIR)==LOW){

    // Reset motionDetected
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
      
      if(!motionDetected){         // One Shot
        neo.sendCommand(":Debug","Taking photo");
        motionDetected = true;
        resetTimer(&emailDelay);
      }

      resetTimer(&timer1);
    }    
    
  }
*/
}

//======================================================



