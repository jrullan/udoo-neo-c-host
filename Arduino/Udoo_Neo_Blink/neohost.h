#ifndef NEOHOST_H
#define NEOHOST_H

#include <Arduino.h>
#include "neotimer.h"

#define NL '0x0D'   //Required form to read the new line character from the Serial in the Neo
#define NEO_MAX_BUFF 64
#define NEO_SERIAL_DELAY 10  //Apparently it's the minimum time that does not make it fail.

class NeoHost{
  public:
    //Methods
    NeoHost();    //Constructor
    ~NeoHost();   //Destructor
    boolean available();
    void init();  //Initializations
    int readLine();
    void sendLine(String str);
    void sendLine(String str, int value);
    template <typename Par> void sendCommand(String cmd, Par par); //par could be int or String
    void update();
    
    //Attributes
    
  private:
    //Methods
    boolean send(String str);
    boolean buffEquals(String str);
    
    //Attributes
    unsigned char *host = (unsigned char*)":wakeup:";
    char *_buff;
    String* _str;
    Neotimer timer = Neotimer(NEO_SERIAL_DELAY); //Delay before resending info.
};

/*
 * Constructor
 */
NeoHost::NeoHost(){}

/*
 * Destructor
 */
NeoHost::~NeoHost(){
  if(_buff) free(_buff);
}

/*
 * Checks if the Host is available by checking the
 * serial port for incoming bytes.
 */ 
boolean NeoHost::available(){
  int i=0;
  int bytes;
  while(true){
    delay(100);

    if((bytes = this->readLine()) > 0){
      for(i=0;i<bytes;i++){
        if(_buff[i] != host[i]) break;
        //this->sendLine(_buff);
        if(i == bytes-1) return true;
      }
    }
    
    /*
    while((Serial.available()>0) && (bytes<NEO_MAX_BUFF)){
      _buff[bytes++] = Serial.read();
    }*/

    /*
    for(i=0;i<bytes;i++){
      if(_buff[i] != host[i]) break;
      if(i == bytes-1) return true;
    }*/
  }
}

/*
 * Initializations
 */
void NeoHost::init(){
  if(this->_buff = (char *)malloc(NEO_MAX_BUFF+1)){
    memset(this->_buff,0,NEO_MAX_BUFF+1);
  }
  //this->timer.set(1000); 
  this->timer.reset();
}

/*
 * Reads a line from Serial
 */
int NeoHost::readLine(){
  int bytes;

  while((Serial.available()>0) && (bytes<NEO_MAX_BUFF)){
    this->_buff[bytes] = Serial.read();
    bytes++;
  }
  
  return bytes;
}

/*
 * Compares read buffer to a String
 */
boolean NeoHost::buffEquals(String str){        //:wakeup:
  int i;
  byte len = str.length();                      //8
  for(i=0;i<len;i++){                           //0-7
    if((char)_buff[i] != str[i]) return false; //
  }
  return true;
}

/*
 * Sends a line to Serial
 */
void NeoHost::sendLine(String str){
  str += '\n';            //append newline character
  this->send(str);
}

/*
 * Sends a line and a value to Serial
 */
void NeoHost::sendLine(String str, int value){
  str += value;           //append value to string
  str += '\n';            //append newline character
  this->send(str);
}

/*
 * Sends a command to Serial
 */
template <typename Par>
void NeoHost::sendCommand(String cmd, Par par){
  cmd += '(';           // :cmd(
  cmd += par;           //append value to string
  cmd += ')';           // :cmd(
  cmd += '\n';          //append newline character
  if(this->send(cmd)){
    delay(10);
    int bytes = this->readLine();
  }
}

/*
 * Perform the actual send
 */
boolean NeoHost::send(String str){
  if(this->timer.done()){
    Serial.print(str);
    Serial.flush();
    this->timer.reset();
    return true;
  }
  return false;
}



//This shall be called in the loop section
void NeoHost::update(){
  //Check if there is any message to read
  if(Serial.available()>0){
    this->readLine();
  }

  //After reading (if anything) check if there is something to send
  
}


#endif
