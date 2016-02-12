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
#define BUFFER_MAX 64
#define PIR 4
#define LED1 13

byte buff[BUFFER_MAX];
unsigned char *intToStr;
boolean motionDetected = false;
long emailDelay = 60000;
long emailAcc = 0;
int l = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIR,INPUT);
  pinMode(LED1,OUTPUT);
  intToStr = (unsigned char*) malloc (sizeof(int)/sizeof(unsigned char));
  if(intToStr != NULL) memset(&intToStr,0,sizeof(int)/sizeof(unsigned char));
  memset(&buff,0,BUFFER_MAX);
}

void loop() {
  /*
  if(Serial.available()>0){
    byte bytes = readSerial(buff);
    sendLine("Ard: Bytes received: ",bytes);
    sendLine("Ard: Message received: ",buff,bytes);
  }
  */
  
  if(digitalRead(PIR)==HIGH){
    if(!motionDetected){         // One Shot
      digitalWrite(LED1,HIGH);
      if(done(emailDelay,&emailAcc)){ 
        //sendCommand(":Debug",emailAcc);
        //sendCommand(":Debug","jerullan@gmail.com");
        sendCommand(":SetEmail","jerullan@gmail.com");
        //sendCommand(":SetEmail","jerullan@yahoo.com");
        sendCommand(":Email","Motion Detected");
      }
    }
    motionDetected = true;
  }else{
    digitalWrite(LED1,LOW);
    motionDetected = false;
  }

}

//======================================================

/* 
 * Read serial data into buffer 
 * Returns quantity of bytes read
 */
byte readSerial(unsigned char *buff){
  byte bytes;
  while(Serial.available()>0){
    buff[bytes++] = Serial.read();
  }
  return bytes;
}

/*
 * Sends a string through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str){
  str += '\n';            //append newline character
  Serial.print(str);
  Serial.flush();
  delay(100);
}

/*
 * Sends a string and an integer value through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str, int value){
  str += value;           //append value to string
  str += '\n';            //append newline character
  Serial.print(str);
  Serial.flush();
  delay(100);
}

/*
 * Sends a string and an integer value through the serial
 * as a line. Ending in new line.
 */
void sendCommand(String cmd, int par){
  cmd += '(';             // :cmd(
  cmd += par;           //append value to string
  cmd += ')';             // :cmd(
  cmd += '\n';            //append newline character
  Serial.print(cmd);
  Serial.flush();
  delay(100);
}

// string par version of above.
void sendCommand(String cmd, String par){
  cmd += '(';             // :cmd(
  cmd += par;           //append value to string
  cmd += ')';             // :cmd(
  cmd += '\n';            //append newline character
  Serial.print(cmd);
  Serial.flush();
  delay(100);  
}

// Timer utility
boolean done(long timeDelay, long* accumulated){
  if(millis()-*accumulated > timeDelay){
    *accumulated = millis();
    return true;
  }
  return false;
}

