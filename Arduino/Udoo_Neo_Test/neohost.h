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
    byte readLine();
    void sendLine(String str);
    void sendLine(String str, int value);
    template <typename Par> void sendCommand(String cmd, Par par); //par could be int or String
    template <typename Par> bool toA9(String cmd, Par par);
    void update();
    int compareText(char* text1, char* text2);
    int stringSize(char* data);
    
    //Attributes
    
  private:
    //Methods
    void send(String str);
    //Attributes
    unsigned char *_buff;
    char *_inBuff;
    String* _str;
    Neotimer timer = Neotimer(1000); //Delay before resending info.
};

//Default constructor
NeoHost::NeoHost(){}

//Default destructor
NeoHost::~NeoHost(){
  if(_buff) free(_buff);
  if(_inBuff) free(_inBuff);
}

//Initializations
void NeoHost::init(){
  if(this->_buff = (unsigned char *)malloc(NEO_MAX_BUFF+1)){
    memset(this->_buff,0,NEO_MAX_BUFF+1);
  }
  if(this->_inBuff = (char *)malloc(NEO_MAX_BUFF+1)){
    memset(this->_inBuff,0,NEO_MAX_BUFF+1);
  }  
  
  this->timer.set(1000); 
  this->timer.reset();
}

//Reads a line from Serial into _inBuff
byte NeoHost::readLine(){
  byte bytes=0;
  while((Serial.available()>0) && (bytes<NEO_MAX_BUFF)){
    _inBuff[bytes++] = Serial.read();
  }
  _inBuff[bytes] = 0; // EOL character (just in case)
  return bytes;
}

//Sends a line and a value to Serial
void NeoHost::sendLine(String str, int value){
  str += value;           //append value to string
  this->sendLine(str);
}

template <typename Par>
bool NeoHost::toA9(String cmd, Par par){
  this->sendCommand(cmd,par);
  delay(100);
  this->readLine();
  if(this->compareText(_inBuff,":7:")){
    return true;
  }else{
    return false;
  }
}

//Sends a command and a parameter in command syntax
template <typename Par>
void NeoHost::sendCommand(String cmd, Par par){
  cmd += '(';           // :cmd(
  cmd += par;           //append value to string
  cmd += ')';           // :cmd(
  this->sendLine(cmd);
}

//Sends a line to Serial
void NeoHost::sendLine(String str){
  str += '\r';            //append carriage return character
  str += '\n';            //append newline character
  this->send(str);
}

//Sends the string message to the Serial object
void NeoHost::send(String str){
  //if(this->timer.wait(100)){
    Serial.print(str);
    Serial.flush();
    //while(!this->timer.done(100));
    delay(10);
    //this->timer.reset();
  //}
}

//This shall be called in the loop section
void NeoHost::update(){
  //Check if there is any message to read
	if(Serial.available()>0){
		this->readLine();
	}

  //After reading (if anything) check if there is something to send
  
}

int NeoHost::compareText(char* text1, char* text2){
  int size1 = stringSize(text1);
  int size2 = stringSize(text2);
    
  if(size1 == size2){
    int i;
    for(i=0;i<size1;i++){
      if(text1[i] != text2[i]){
        //Different characters
        return 0;
      }
    }
    //Identical strings
    return 1;
  }
  //Different sizes
  return 0;
}

//Utility to get size of char* (char array)
int NeoHost::stringSize(char* data){
  int dataSize = 0;
  while(data[dataSize] != 0){ // end of string == 0
    dataSize++;
  }
  return dataSize;  
}
#endif
