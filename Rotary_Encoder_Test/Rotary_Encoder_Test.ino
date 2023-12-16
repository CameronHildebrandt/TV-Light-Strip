#define encoderOutputA 7
#define encoderOutputB 9
#define button 3
 
int aState;
int aLastState;
int LastEncoderTrueOutput = -1; // To stop duplicate outputs

  
void setup() { 
  Serial.begin(9600);
  pinMode(encoderOutputA,INPUT);
  pinMode(encoderOutputB,INPUT);
  pinMode(button,INPUT);

  // Reads the initial state of the encoderOutputA
  aLastState = digitalRead(encoderOutputA); 
} 

int listenForEncoder() {
  /* Returns:
  *  0 - False
  *  1 - Up
  *  2 - Down
  *  3 - Button Pressed
  */

  int returnValue = 0;

  aState = digitalRead(encoderOutputA); // Reads the "current" state of the encoderOutputA
  // If the previous and the current state of the encoderOutputA are different, that means a Pulse has occured
  if (aState != aLastState){
    // If the encoderOutputB state is different to the encoderOutputA state, that means the encoder is rotating clockwise
    if (digitalRead(encoderOutputB) != aState) {
      returnValue = 1;
    } else {
      returnValue = 2;
    }
  }
  aLastState = aState; // Updates the previous state of the encoderOutputA with the current state

  if(!digitalRead(button)) {
    returnValue = 3;
  }


  // Don't return double outputs
  if(LastEncoderTrueOutput == returnValue) {
    returnValue = 0;
    LastEncoderTrueOutput = -1;
  } else if(returnValue != 0) {
    LastEncoderTrueOutput = returnValue;

    //Debug
     if(returnValue == 1) {
       Serial.println("Down");
     }
     else if(returnValue == 2) {
       Serial.println("Up");
     }
     else if(returnValue == 3) {
       Serial.println("Select");
     }
    //EndDebug
  }

  return returnValue;
}

 
void loop() { 
  listenForEncoder();
}
