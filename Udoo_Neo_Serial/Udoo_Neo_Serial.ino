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
#define MAX_TRIGGERS 10

byte buff[BUFFER_MAX];
unsigned char *intToStr;
boolean motionDetected = false;


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
  timer1.time = 2000;
  emailDelay.time = 60000;
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
    if(timerDone(&emailDelay)){
      motionDetected = false;
    }
    resetTimer(&timer1);
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

