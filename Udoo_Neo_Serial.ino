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
      sendLine("ard:Debug(Motion Detected)", l++);
    }
    motionDetected = true;
  }else{
    digitalWrite(LED1,LOW);
    motionDetected = false;
  }
  

  //Serial.print(l++);
  //sendLine(" From Arduino ");
  //Serial.print("From Arduino ");
  //sendLine(" ");    
  

  Serial.flush();
  delay(100); //Minimum time to wait after serial flush.
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
 * Sends contents of the buffer through the serial
 * as a line. Ending in new line.
 */
void sendLine(unsigned char *buff, int bytes){
  if(bytes > BUFFER_MAX) bytes = BUFFER_MAX;
  Serial.write(buff,bytes);
  Serial.print('\n');
}

/*
 * Sends contents of the buffer through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str,unsigned char *buff, int bytes){
  if(bytes > BUFFER_MAX) bytes = BUFFER_MAX;
  Serial.print(str);
  Serial.write(buff,bytes);
  Serial.print('\n');
}

/*
 * Sends a number through the serial
 * as a line. Ending in new line.
 */
void sendLine(int number){
  Serial.print(number);
  Serial.print('\n');
}

/*
 * Sends a string through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str){
  str += '\n';            //append newline character
  Serial.print(str);
  //byte len = str.length()+1; //calculate length of string
  //str.getBytes(buff,len);      //copy string to buffer
  //Serial.write(buff,len);      //write buffer to serial
}

/*
 * Sends a string and an integer value through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str, int value){
  str += value;           //append value to string
  str += '\n';            //append newline character
  Serial.print(str);
  //byte len = str.length() + 1; //calculate length of string
  //str.getBytes(buff,len);      //copy string to buffer
  //Serial.write(buff,len);      //write buffer to serial
}

/*
 * Sends a string and an float value through the serial
 * as a line. Ending in new line.
 */
void sendLine(String str, int value, int places){
  Serial.print(str);
  Serial.print(value,places);
  Serial.print('\n');
}

