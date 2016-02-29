#ifndef NEOHOST_H
#define NEOHOST_H

#include <Arduino.h>

#define NL '0x0D'   //Required form to read the new line character from the Serial in the Neo

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
    
    //Attributes
    
  private:
    //Methods

    //Attributes
  
};

//Default constructor
NeoHost::NeoHost(){}

//Default destructor
NeoHost::~NeoHost(){}

//Initializations
void NeoHost::init(){}

//Reads a line from Serial
byte NeoHost::readLine(unsigned char *buff){
  byte bytes;
  while(Serial.available()>0){
    buff[bytes++] = Serial.read();
  }
  return bytes;
}

//Sends a line to Serial
void NeoHost::sendLine(String str){
  str += '\n';            //append newline character
  Serial.print(str);
  Serial.flush();
  delay(100);
}

//Sends a line and a value to Serial
void NeoHost::sendLine(String str, int value){
  str += value;           //append value to string
  str += '\n';            //append newline character
  Serial.print(str);
  Serial.flush();
  delay(100);
}

//Sends a command and a parameter in command syntax
template <typename Par>
void NeoHost::sendCommand(String cmd, Par par){
  cmd += '(';           // :cmd(
  cmd += par;           //append value to string
  cmd += ')';           // :cmd(
  cmd += '\n';          //append newline character
  Serial.print(cmd);
  Serial.flush();
  delay(100); 
}


#endif
