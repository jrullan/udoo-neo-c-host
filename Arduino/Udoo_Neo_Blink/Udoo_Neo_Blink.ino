#import "neotimer.h"
#import "neohost.h"

#define LED 13

Neotimer neoTimer = Neotimer(1000);
NeoHost neo = NeoHost();
boolean flip = false;
int times = 0;

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  digitalWrite(LED,LOW);
  neo.available();   //Stay here until wakeup command is received
}

void loop() {

  if(neoTimer.done()){
    times++;
    
    String message = "Flipping ";
    message += times;
    message += flip ? " ON" : " OFF";
    neo.sendCommand(":Debug",message);
    neo.sendCommand(":Debug","Flipped...");
    flip ? digitalWrite(LED,HIGH) : digitalWrite(LED,LOW);
    flip = !flip;

    neoTimer.reset();
  }
}
