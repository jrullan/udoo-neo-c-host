#ifndef NEOHOST_H
#define NEOHOST_H

#include <Arduino.h>
#include "neotimer.h"

#define NL '0x0D'   //Required form to read the new line character from the Serial in the Neo
#define NEO_MAX_BUFF 64

class NeoHost{
  public:
    //Methods
    NeoHost();    //Constructor
    ~NeoHost();   //Destructor
    void init();  //Initializations
    byte readLine(unsigned char *buff);
    void sendLine(String str);
    void sendLine(String str, int value);
    template <typename Par> void sendCommand(String cmd, Par par); //par could be int or String
    void update();
    
    //Attributes
    
  private:
    //Methods
    void send(String str);
    //Attributes
    unsigned char *_buff;
    String* _str;
    Neotimer timer = Neotimer(1000); //Delay before resending info.
};

//Default constructor
NeoHost::NeoHost(){}

//Default destructor
NeoHost::~NeoHost(){
  if(_buff) free(_buff);
}

//Initializations
void NeoHost::init(){
  if(this->_buff = (unsigned char *)malloc(NEO_MAX_BUFF+1)){
    memset(this->_buff,0,NEO_MAX_BUFF+1);
  }
  this->timer.set(1000); 
  this->timer.reset();
}

//Reads a line from Serial
byte NeoHost::readLine(unsigned char *buff){
  byte bytes;
  while((Serial.available()>0) && (bytes<NEO_MAX_BUFF)){
    buff[bytes++] = Serial.read();
  }
  return bytes;
}

//Sends a line to Serial
void NeoHost::sendLine(String str){
  str += '\n';            //append newline character
  this->send(str);
}

//Sends a line and a value to Serial
void NeoHost::sendLine(String str, int value){
  str += value;           //append value to string
  str += '\n';            //append newline character
  this->send(str);
}

//Sends a command and a parameter in command syntax
template <typename Par>
void NeoHost::sendCommand(String cmd, Par par){
  cmd += '(';           // :cmd(
  cmd += par;           //append value to string
  cmd += ')';           // :cmd(
  cmd += '\n';          //append newline character
  this->send(cmd);
}

//Sends the string message to the Serial object
void NeoHost::send(String str){
  //if(this->timer.done()){
    Serial.print(str);
    Serial.flush();
    delay(100);
    //this->timer.reset();
  //}
}

//This shall be called in the loop section
void NeoHost::update(){
  //Check if there is any message to read
	if(Serial.available()>0){
		this->readLine(_buff);
	}

  //After reading (if anything) check if there is something to send
  
}


#endif
