#ifndef NEOTIMER_H
#define NEOTIMER_H

#include <Arduino.h>

class Neotimer{
  public:
    //Methods
    Neotimer(long _t);      //Constructor
    ~Neotimer();            //Destructor
    void init();            //Initializations
    boolean done();         //Indicates time has elapsed
    boolean wait(long _t);  //Indicates time has elapsed
    void reset();           //Resets timer to zero
    //boolean wait();
    void set(long t);

  private:

    typedef struct myTimer{
      long time;
      long last;
      boolean done;
    };

    struct myTimer _timer;
    boolean _waiting;
};

//Default constructor
Neotimer::Neotimer(long _t){
  this->_timer.time = _t;
}

//Default destructor
Neotimer::~Neotimer(){
  
}

//Initializations
void Neotimer::init(){
  this->_waiting = false;
}


// Timer utilities functions

//Checks if timer has elapsed
boolean Neotimer::wait(long _t){
  this->_timer.time = _t;
  return this->done();
}


//Checks if timer has elapsed
boolean Neotimer::done(){
  if(this->_timer.done) this->reset();
  
  if( (millis()-this->_timer.last) >= this->_timer.time){
    this->_timer.done = true;
    return true;
  }
  
  return false;
}


void Neotimer::set(long t){
  this->_timer.time = t;
}

void Neotimer::reset(){
  this->_timer.last = millis();
  this->_timer.done = false;
  this->_waiting = false;
}


//Restarts timer and wait until it is finished.
/*
boolean Neotimer::wait(){
  if(!this->_waiting){
    this->_timer.last = millis();  
    this->_waiting = true;
  }else{
    if(this->done()){
      return true;
    }
  }
  return false;
}
*/

#endif
