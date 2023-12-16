#include <FastLED.h>

#define NUM_LEDS 10
#define DATA_PIN 5

#define ENCOUT_A 7 // Encoder Output A
#define ENCOUT_B 9 // Encoder Output B
#define ENCBTN 3

int aState;
int aLastState;
int LastEncoderTrueOutput = -1; // To stop duplicate outputs

int Mode = 4; // 0 = Menu, 1 = Wave, 2 = Solid, 3 = Cycle, 4 = Sparkle, 5 = Jump
int NumberOfSubMenus = 5; // 0th = Wave, 1st = Solid, 2nd = Cycle, 3rd = Sparkle. 4th = Jump
unsigned long LastTimeButtonPressed = 0;
bool ExitMenu = false;

bool VerboseOutput = false;

int Speed = 50; // Amount of delay between LED Updates (50-400)
int SolidColor = 0;
int SolidColorSparkle[10] = {0,0,0,0,0,0,0,0,0,0};

// Define the array of leds
CRGB leds[NUM_LEDS];


void setup() {
  Serial.begin(9600);

  //Initialize Rotary Encoder
  pinMode(ENCOUT_A ,INPUT);
  pinMode(ENCOUT_B ,INPUT);
  pinMode(ENCBTN ,INPUT);

  // Reads the initial state of the encoderOutputA
  aLastState = digitalRead(ENCOUT_A);

  //Initialize LED Strip
  FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, NUM_LEDS);

}

void displayColor(int color, int index) {
  /* color - a numeric value between 0 and 1530
   */

  // Ensure that the color is within the bounds
  color % 1530;

  if(color < 0) {
    color = 0;
  }

  // Set the color
  if(color >= 0 && color <= 255) {
    // setColor(255, 0, color);
    leds[index].setRGB(255, 0, color);
  }
  else if(color > 255 && color <= 510) {
    // setColor(510-color, 0, 255);
    leds[index].setRGB(510-color, 0, 255);
  }
  else if(color > 510 && color <= 765) {
    // setColor(0, color-510, 255);
    leds[index].setRGB(0, color-510, 255);
  }
  else if(color > 765 && color <= 1020) {
    // setColor(0, 255, 1020-color);
    leds[index].setRGB(0, 255, 1020-color);
  }
  else if(color > 1020 && color <= 1275) {
    // setColor(color-1020, 255, 0);
    leds[index].setRGB(color-1020, 255, 0);
  }
  else if(color > 1275 && color <= 1530) {
    // setColor(255, 1530-color, 0);
    leds[index].setRGB(255, 1530-color, 0);
  }

  FastLED.show();
}


int listenForEncoder() {
  /* Returns:
  *  0 - False
  *  1 - Up
  *  2 - Down
  *  3 - Button Pressed
  */

  int returnValue = 0;

  aState = digitalRead(ENCOUT_A); // Reads the "current" state of the encoderOutputA
  // If the previous and the current state of the encoderOutputA are different, that means a Pulse has occured
  if (aState != aLastState){
    // If the encoderOutputB state is different to the encoderOutputA state, that means the encoder is rotating clockwise
    if (digitalRead(ENCOUT_B) != aState) {
      returnValue = 1;
    } else {
      returnValue = 2;
    }
  }
  aLastState = aState; // Updates the previous state of the encoderOutputA with the current state

  if(!digitalRead(ENCBTN)) {
    returnValue = 3;
  }


  // Don't return double outputs
  if(LastEncoderTrueOutput == returnValue) {
    returnValue = 0;
    LastEncoderTrueOutput = -1;
  }

  else if(returnValue != 0) {
    LastEncoderTrueOutput = returnValue;

    // Debug
    if(VerboseOutput) {
      if(returnValue == 1) {
       Serial.println("Down");
      }
      else if(returnValue == 2) {
       Serial.println("Up");
      }
      else if(returnValue == 3) {
       Serial.println("Select");
      }
    }

  }

  return returnValue;
}


bool handleInputMain(int inputToHandle) {
  // Returns true if the button was pressed.

  // If the Button is pressed, enter the menu.
  if(inputToHandle == 3 && LastTimeButtonPressed + 500 < millis()) {
    LastTimeButtonPressed = millis(); // Record button press to avoid duplicate inputs
    Mode = 0;

    // Animation
    int animationSpeed = 40;

    leds[4].setRGB(0, 0, 0);
    leds[5].setRGB(0, 0, 0);
    FastLED.show();
    delay(animationSpeed);

    leds[3].setRGB(0, 0, 0);
    leds[6].setRGB(0, 0, 0);
    FastLED.show();
    delay(animationSpeed);

    leds[2].setRGB(0, 0, 0);
    leds[7].setRGB(0, 0, 0);

    leds[0].setRGB(0, 0, 75); // Fade in blue ends (becuase we start on wave)
    leds[9].setRGB(0, 0, 75);

    FastLED.show();
    delay(animationSpeed);

    leds[1].setRGB(0, 0, 0);
    leds[8].setRGB(0, 0, 0);

    leds[0].setRGB(0, 0, 150); // Fade in blue ends (becuase we start on wave)
    leds[9].setRGB(0, 0, 150);

    FastLED.show();
    delay(animationSpeed);

    leds[0].setRGB(0, 0, 225); // Fade in blue ends (becuase we start on wave)
    leds[9].setRGB(0, 0, 225);

    FastLED.show();
    delay(animationSpeed);

    return true;
  }

  return false;

  // This is the listener running when the lights are doing their thing,
  // so ignore all other inputs
}


int handleInputMenu(int inputToHandle, int currSubMenu) {

  // Encoder is spun
  if(inputToHandle == 1) {
    currSubMenu++;
    Serial.println("INC");
  }
  else if(inputToHandle == 2) {
    currSubMenu--;
    Serial.println("DEC");
  }

  // Button pressed, enter menu
  else if(inputToHandle == 3 && LastTimeButtonPressed + 500 < millis()) {
    LastTimeButtonPressed = millis(); // Button pressed again, update time

    Serial.println("NEXT MENU");
    ExitMenu = true; // Break out of the loop so we can enter the next menu

  }

  // Handle overflow
  while(currSubMenu > NumberOfSubMenus) {
    currSubMenu -= NumberOfSubMenus;

    // Debug
    if(VerboseOutput) {
      Serial.println("HANDLING OVERFLOW A");
    }
  }

  while(currSubMenu < 0) {
    currSubMenu += NumberOfSubMenus;

    // Debug
    if(VerboseOutput) {
      Serial.println("HANDLING OVERFLOW B");
    }
  }

  // Debug
  if(VerboseOutput) {
    Serial.print("Sub Menu: ");
    Serial.println(currSubMenu);
  }

  return currSubMenu;
}


void handleInputSpeed(int inputToHandle) {
  // Adjust speed from ~50 to ~400
  // There is some error, but it shouldn't be noticable

  // Update the LEDS
  for(int i = 1; i < NUM_LEDS - 1; i++) { // Turn off middle LEDS in case the value decreases
    leds[i].setRGB(0, 0, 0);
  }

  int numLEDStoShow = ceil(-0.01714*Speed + 6.9);

  if(numLEDStoShow > 6) { // Take care of overflow
    numLEDStoShow = 6;
  }

  // Coarse
  for(int i = 0; i < numLEDStoShow; i++) { // Turn on the specific level of LEDS
    leds[i+2].setRGB(255, 255, 255);
  }

  // Update the last one
  // This is tricky, lets just ignore for rn

  // Serial.print("Num LEDS: ");
  // Serial.print(numLEDStoShow);
  // Serial.print("  -  Speed: ");
  // Serial.println(Speed);

  FastLED.show();

  // Encoder is spun
  if(inputToHandle == 1 && Speed < 390) {
    Speed += 25;
  }
  else if(inputToHandle == 2 && Speed > 60) {
    Speed -= 25;
  }

  // Button pressed, enter menu
  else if(inputToHandle == 3 && LastTimeButtonPressed + 500 < millis()) {
    LastTimeButtonPressed = millis(); // Button pressed again, update time

    Serial.print("EXIT MENU IN MODE: ");
    Serial.println(Mode);
    ExitMenu = true; // Break out of the loop so we can enter the next menu

    // Exit Animation
    if(Mode == 4) { // Sparkle Mode Exit animation
      int animationSpeed = 80;
      SolidColor = 232; // Ensures that we don't have some garbage number

      // Set the starting color of the sparkle mode so we don't get that nasty jump
      for(int i = 0; i < NUM_LEDS; i++) {
        SolidColorSparkle[i] = SolidColor;
      }

      displayColor(SolidColor, 0);
      displayColor(SolidColor, 9);
      delay(animationSpeed);

      displayColor(SolidColor, 1);
      displayColor(SolidColor, 8);
      delay(animationSpeed);

      displayColor(SolidColor, 2);
      displayColor(SolidColor, 7);
      delay(animationSpeed);

      displayColor(SolidColor, 3);
      displayColor(SolidColor, 6);
      delay(animationSpeed);

      displayColor(SolidColor, 4);
      displayColor(SolidColor, 5);
      delay(animationSpeed);
    }

    else if(Mode == 3) { // Cycle Mode Exit animation
      int animationSpeed = 80;
      SolidColor = 1020; // Ensures that we don't have some garbage number

      displayColor(SolidColor, 0);
      displayColor(SolidColor, 9);
      delay(animationSpeed);

      displayColor(SolidColor, 1);
      displayColor(SolidColor, 8);
      delay(animationSpeed);

      displayColor(SolidColor, 2);
      displayColor(SolidColor, 7);
      delay(animationSpeed);

      displayColor(SolidColor, 3);
      displayColor(SolidColor, 6);
      delay(animationSpeed);

      displayColor(SolidColor, 4);
      displayColor(SolidColor, 5);
      delay(animationSpeed);
    }

    else if(Mode == 5) { // Jump Mode Exit animation
      int animationSpeed = 80;
      SolidColor = 922; // Ensures that we don't have some garbage number

      displayColor(SolidColor, 0);
      displayColor(SolidColor, 9);
      delay(animationSpeed);

      displayColor(SolidColor, 1);
      displayColor(SolidColor, 8);
      delay(animationSpeed);

      displayColor(SolidColor, 2);
      displayColor(SolidColor, 7);
      delay(animationSpeed);

      displayColor(SolidColor, 3);
      displayColor(SolidColor, 6);
      delay(animationSpeed);

      displayColor(SolidColor, 4);
      displayColor(SolidColor, 5);
      delay(animationSpeed);
    }
  }
}


void handleInputColor(int inputToHandle) {
  // Adjust color from ~0 to ~1530
  // There is some error, but it shouldn't be too noticable

  // Encoder is spun
  if(inputToHandle == 1) {
    SolidColor += 25;
  }
  else if(inputToHandle == 2) {
    SolidColor -= 25;
  }

  // Handle overflow
  if(SolidColor > 1530) {
    SolidColor -= 1530;
  } else if(SolidColor < 0) {
    SolidColor += 1530;
  }

  // Button pressed, enter menu
  else if(inputToHandle == 3 && LastTimeButtonPressed + 500 < millis()) {
    LastTimeButtonPressed = millis(); // Button pressed again, update time

    Serial.println("EXIT MENU");
    ExitMenu = true; // Break out of the loop so we can enter the next menu

    // Exit animation
    displayColor(SolidColor, 1);
    displayColor(SolidColor, 8);

    delay(80);

    displayColor(SolidColor, 0);
    displayColor(SolidColor, 9);
  }

  // Update the LEDS
  for(int i = 2; i < NUM_LEDS - 2; i++) { // Turn on the specific level of LEDS
    displayColor(SolidColor, i);
  }

  // Serial.print("  -  Color: ");
  // Serial.println(SolidColor);
}


void menu() {
  int currSubMenu = 0;

  for(int i = 1; i < NUM_LEDS - 1; i++) { // Ensure middle LEDS are off
    leds[i].setRGB(0, 0, 0);
  }
  FastLED.show();

  // First stage of menu
  while(!ExitMenu) {
    switch(currSubMenu) {
      case 0:
        // Wave
        leds[0].setRGB(0, 0, 255);
        leds[9].setRGB(0, 0, 255);
        FastLED.show();
        break;

      case 1:
        // Solid
        leds[0].setRGB(255, 0, 0);
        leds[9].setRGB(255, 0, 0);
        FastLED.show();
        break;

      case 2:
        // Cycle
        leds[0].setRGB(0, 255, 0);
        leds[9].setRGB(0, 255, 0);
        FastLED.show();
        break;

      case 3:
        // Sparkle
        leds[0].setRGB(255, 0, 232);
        leds[9].setRGB(255, 0, 232);
        FastLED.show();
        break;
      case 4:
        // Jump
        leds[0].setRGB(0, 255, 98);
        leds[9].setRGB(0, 255, 98);
        FastLED.show();
        break;

      default:
        // Some Error Occured, Set subMenu to Wave
        currSubMenu = 0;
        Serial.println("ERROR");
        break;
    }

    // Placed after switch so that the LEDS in the picker don't get changed.
    currSubMenu = handleInputMenu(listenForEncoder(), currSubMenu);
  }

  ExitMenu = false; // We exited the menu, lets fix this variable.
  Mode = currSubMenu + 1; // We add 1 because mode 0 is the menu, and we don't include that in submenu


  // Debug
  if(true) { //VerboseOutput
    switch(Mode) {
      case 1:
        // Wave
        Serial.println("WAVE MODE SUB MENU ENTERED");
        break;

      case 2:
        // Solid
        Serial.println("SOLID MODE SUB MENU ENTERED");
        break;

      case 3:
        // Cycle
        Serial.println("CYCLE MODE SUB MENU ENTERED");
        break;
      case 4:
        // Sparkle
        Serial.println("SPARKLE MODE SUB MENU ENTERED");
        break;
      case 5:
        // Jump
        Serial.println("JUMP MODE SUB MENU ENTERED");
        break;

      default:
        break;
    }
  }

  // Second stage of menu (picker)
  while(!ExitMenu) {
    if(Mode == 1 || Mode == 3 || Mode == 4 || Mode == 5) {
      handleInputSpeed(listenForEncoder());

    }
    else if(Mode == 2) {
      handleInputColor(listenForEncoder());
    }
    else {
      Serial.println("Error, exit menu and leave speed/color alone");
      ExitMenu = true;
    }
  }

  ExitMenu = false; // We exited the menu, lets fix this variable.
}

void waveMode() {
  int randomColor = random(0,1530);

  // Forward Wave
  for(int i = 0; i < NUM_LEDS; i++) {
    // This is a long loop, so to keep the UX snappy, listen more
    if(handleInputMain(listenForEncoder())) {
      return;
    }

    displayColor(randomColor, i);
    delay(Speed);
  }

  // Reverse Wave
  randomColor = random(0,1530);
  for(int i = NUM_LEDS; i >= 0; i--) {
    // This is a long loop, so to keep the UX snappy, listen more
    if(handleInputMain(listenForEncoder())) {
      return;
    }

    displayColor(randomColor, i);
    delay(Speed);
  }
}


void cycleMode() {
  // Update the color (Skip more stops if you go faster)
  int incrementSpeed = ceil(-0.05429*Speed + 22.7);

  // Serial.print("incSpd: ");
  // Serial.print(incrementSpeed);
  // Serial.print("  -  Speed: ");
  // Serial.println(Speed);

  SolidColor += incrementSpeed;

  // Handle overflow
  if(SolidColor > 1530) {
    SolidColor -= 1530;
  }

  for(int i = 0; i < NUM_LEDS; i++) {
    displayColor(SolidColor, i);
  }
}


void sparkleMode() {
  float solidColorAccurate[10] = {0,0,0,0,0,0,0,0,0,0};
  int destColor[10] = {0,0,0,0,0,0,0,0,0,0};
  float numberChangePerStep[10] = {0,0,0,0,0,0,0,0,0,0};
  int delta[10] = {0,0,0,0,0,0,0,0,0,0};
  bool verboseOutputSparkleMode = false;

  if(verboseOutputSparkleMode){
    Serial.println("CYCLE START");
  }

  for(int i = 0; i < NUM_LEDS; i++) {
    solidColorAccurate[i] = float(SolidColorSparkle[i]); // Sparkle needs to keep track of partial color incriments
    destColor[i] = random(0,1530); // Get the endpoints
  }

  // Get the delta for each LED
  for(int i = 0; i < NUM_LEDS; i++) {
    delta[i] = abs(SolidColorSparkle[i] - destColor[i]);
  }

  if(verboseOutputSparkleMode){
    Serial.print("Deltas: [");
  }

  // Determine the step for each LED
  for(int i = 0; i < NUM_LEDS; i++) {
    // Dest is above start
    if(destColor[i] > SolidColorSparkle[i]) {
      if(delta[i] > 765) { // Faster to go backwards (1530/2 = 765)
        numberChangePerStep[i] = float((delta[i] - 1530))/float(Speed);

        if(verboseOutputSparkleMode){
          Serial.print((delta[i] - 1530));
          Serial.print(", ");
        }

      } else { // Go forwards
        numberChangePerStep[i] = float(delta[i])/float(Speed);

        if(verboseOutputSparkleMode){
          Serial.print(delta[i]);
          Serial.print(", ");
        }
      }
    }

    // Dest is below start
    if(destColor[i] <= SolidColorSparkle[i]) {
      if(delta[i] > 765) { // Faster to go forwards (1530/2 = 765)
        numberChangePerStep[i] = float((1530 - delta[i]))/float(Speed);

        if(verboseOutputSparkleMode){
          Serial.print((1530 - delta[i]));
          Serial.print(", ");
        }

      } else { // Go backwards
        numberChangePerStep[i] = float(-delta[i])/float(Speed);
        if(verboseOutputSparkleMode){
          Serial.print(-delta[i]);
          Serial.print(", ");
        }
      }
    }
  }

  if(verboseOutputSparkleMode){
    Serial.println("]");

    Serial.print("Increments: [");
    for(int i = 0; i < NUM_LEDS; i++) {
      Serial.print(numberChangePerStep[i]);
      Serial.print(", ");
    }
    Serial.println("]");

    Serial.print("Start Colors: [");
    for(int i = 0; i < NUM_LEDS; i++) {
      Serial.print(SolidColorSparkle[i]);
      Serial.print(", ");
    }
    Serial.println("]");

    Serial.print("Dest Colors: [");
    for(int i = 0; i < NUM_LEDS; i++) {
      Serial.print(destColor[i]);
      Serial.print(", ");
    }
    Serial.println("]");
  }


  // Main Cycle
  for(int i = 0; i < Speed + 1; i++) {
    // This is a long loop, so to keep the UX snappy, lets just listen more
    if(handleInputMain(listenForEncoder())) {
      break;
    }

    if(verboseOutputSparkleMode){ Serial.print("solidColorAccurate: ["); }

    for(int j = 0; j < NUM_LEDS; j++) {
      solidColorAccurate[j] += numberChangePerStep[j];
      SolidColorSparkle[j] = ceil(solidColorAccurate[j]);

      // Handle overflow
      if(SolidColorSparkle[j] > 1530) {
        SolidColorSparkle[j] -= 1530;
      } else if(SolidColorSparkle[j] < 0) {
        SolidColorSparkle[j] += 1530;
      }

      if(verboseOutputSparkleMode){
        Serial.print(solidColorAccurate[j]);
        Serial.print(", ");
      }
    }

    if(verboseOutputSparkleMode){ Serial.print("]"); }

    if(verboseOutputSparkleMode){
      Serial.print("  -  SolidColors:  [");
      for(int j = 0; j < NUM_LEDS; j++) {
        Serial.print(SolidColorSparkle[j]);
        Serial.print(", ");
      }
      Serial.println("]");
    }


    for(int j = 0; j < NUM_LEDS; j++) {
      displayColor(SolidColorSparkle[j], j);
    }

    // delay(1); This mode has enough math that the delay() makes it too slow
  }

  // Next step!
  SolidColor = destColor; // Should already be the case, but just to be sure

  if(verboseOutputSparkleMode){
    Serial.println("CYCLE END");
    Serial.println("");
    Serial.println("");
  }
}


void jumpMode() {
  // Pick a random LED and Colour
  int randomLED = random(0,10);
  SolidColor = random(0,1530);

  displayColor(SolidColor, randomLED);

  delay(Speed);
}



void loop() {
  // Listener
  handleInputMain(listenForEncoder());

  // Have a state machine here, where we run a function responsible for each state
  switch(Mode) {
    case 0:
      // Menu
      menu();
      break;

    case 1:
      // Wave
      waveMode();
      break;

    case 2:
      // Solid
      // You literally don't have to do anything. This mode is as simple as it gets
      break;

    case 3:
      // Cycle
      cycleMode();
      break;

    case 4:
      // Sparkle
      sparkleMode();
      break;
    case 5:
      // Jump
      jumpMode();
      break;

    default:
      // Some Error Occured, Set Mode to Wave
      Mode = 1;
      break;
  }
}
