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
 *    The C program has a poll interval time of about 0.1mS
 */

#include "neohost.h"
#include "neotimer.h"
 
#define LED1 13

int loops = 0;
int logQuantity = 10;
NeoHost neo = NeoHost();
Neotimer neoTime = Neotimer(10000);

void setup() {
  Serial.begin(115200);
  pinMode(LED1,OUTPUT);
  digitalWrite(LED1,LOW);

  neo.toA9(":Debug", "Setup completed!");
}

void loop() {
  static char count = 0;
  
  if(neoTime.wait(1000)){
    String mess = "Loop: ";
    mess += loops;
    
    if(neo.toA9(":Debug",mess)){
      digitalWrite(LED1,HIGH);
      delay(100);
      digitalWrite(LED1,LOW);
    }

    loops++;
  }
   
  if(loops >= 1000) loops = 0;

}

//======================================================



