#include <toneAC.h>           //Library to allow us to alter the volume of the speaker output. Uses pin 9 & 10 for for speaker output. 
#include <SFE_BMP180.h>       //Library to allow us to use the digital barometer. 
#include <Wire.h>             // Library used by the capacitive Sensor library. 
#include <CapacitiveSensor.h> //Library used to allow us to use the capacitive sensors for the piano component. 

#define COMMON_PIN       2       //Ground pin for sensors
#define NUM_OF_SAMPLES   10      //The number of readings from the capacitive sensors that will be taken to verify that a key has actually been pressed
#define CAP_THRESHOLD    150     //The value of capacitance which is required to "trip the switch"     
#define CS(Y)            CapacitiveSensor(2, Y)    //Allows us to shorthand the scale array. 
#define PRES_THRESHOLD   985     //The threshold of pressure over which will cause a note to play. Must do testing to decide this value. 

SFE_BMP180 pressureSensor;      //Creates an instance of the SFE_BMP180 object. 

int scalec[] = {523, 587, 659, 698, 783, 880, 988, 1046};  //C Major Scale. 
CapacitiveSensor keys[] = {CS(3), CS(4), CS(5), CS(6), CS(7), CS(8), CS(11), CS(12)}; //Each sensor's corresponding pin.

void setup(){
  //Turns autocalibration of the keys off.
  for(int i = 0; i < 8; ++i) {
    keys[i].set_CS_AutocaL_Millis(0xFFFFFFFF);
  }  
}

void loop(){
  double pressure = readPressure();

  //This boolean keeps track of whether a key is being pressed down or not. 
  boolean keypress = false;

  //The keys will only be checked if the pressure in the bottle exceeds a certain value.
  while(pressure > PRES_THRESHOLD){

    //Iterates through each key and checks if it's being pressed.
    for(int i = 0; i < 8; i++){

      //This loop repeats so long as a key is being pressed down. 
      do{

        //Checks if the capacitance of the key exceeds the threshod value. 
        if(keys[i].capacitiveSensor(NUM_OF_SAMPLES) > CAP_THRESHOLD){

          keypress = true;

          pressure = readPressure(); //Calls the readPressure function, which returns a double. 
          int pressureMap = map(pressure, 985, 1000, 1, 10); //Maps the barometer value to comply with the parameters of the ToneAC function.

          //Plays a square wave with parameters: (frequency, volume, note length, play in background or foreground.)
          toneAC(scalec[i], pressureMap, 0, true);
        }
        else{
          keypress = false; //Since the key is no onger being pressed, the loop will terminate.
        }
      }
      while(keypress);
    } 
    noToneAC(); //Stops the speaker from emitting sound. 
  }
}  


//Returns the pressure value as read by the barometer. 
double readPressure(void){
  char status;     //Stores the status of the barometer. 
  double T = 21.0; //Stores the temperature value (which has been set to an assumed constant.)
  double P;        //Stores the pressure value

  //status returns 1 if barometer is functioning, 0 if not.
  status = pressureSensor.startPressure(3);

  if(status != 0){
    //delay long enough to get a reading from the barometer. 
    delay(status);

    //Get a pressure value and store it in P. (Dependant on temperature, which we've assumed is constant. 
    status = pressureSensor.getPressure(P, T);

    //If the reading is valid, return P, pressure. 
    if (status != 0){
      return(P);
    }

    else{
      return(0);
    }
  }
  else{
    return(0);
  }
}




