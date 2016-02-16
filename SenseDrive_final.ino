/*
Patrick Proppe for Design Workshop 2 2015

CapSense part based on Arduino playground example
For more information see http://playground.arduino.cc/Main/CapacitiveSensor?from=Main.CapSense */

#include <Servo.h>
#include <CapacitiveSensor.h>

// Constants
// ##################################################
int SERVO_POS_LOW_FRONT = 115; // Lowest Servo Position
int SERVO_POS_HIGH_FRONT = 90; // Highest Servo Position

int SERVO_POS_LOW_REAR = 45; // Lowest Servo Position
int SERVO_POS_HIGH_REAR = 25; // Highest Servo Position
int AWAKE = 0;
int REVERSE = 1;
int NEUTRAL = 2;
int FORWARD = 3;
int PARKING = 4;

int CAPACITIVE_THRESHOLD = 165; // Sensitivity for touch input
int TOUCH_THRESHOD = 1111; // time before touch will be processed
// ##################################################

// Pin Mapping
// ##################################################
int ServoPinFront = 46;
int ServoPinRear = 44;

int VibraFrontLeft = 2;
int VibraFrontRight = 3;
int VibraMiddleLeft = 4;
int VibraMiddleRight = 5;
int VibraRearLeft = 6;
int VibraRearRight = 7;
int VibraCenter = 8;

int sPin = 9;
int rPin1 = 10; // hinten
int rPin2 = 11;
int rPin3 = 12; // vorne
int rPin4 = 13;
// ##################################################


String inputString = "";         // a string to hold incoming serial data
boolean stringComplete = false;  // whether the string is complete

String gesture = ""; // a string for the recognized gesture
boolean touchGesture = false;
String lastGesture = "";
long lastTouchTime = -1;
boolean touched = false;
int lastTouch = -1;
int currentTouch = -1;

Servo frontServo;  // create servo object to control a servo
Servo rearServo;  // create servo object to control a servo



CapacitiveSensor   cs1 = CapacitiveSensor(sPin, rPin1);   // create capSense obbject #1
//CapacitiveSensor   cs2 = CapacitiveSensor(sPin, rPin2);   // create capSense obbject #2
CapacitiveSensor   cs3 = CapacitiveSensor(sPin, rPin3);   // create capSense obbject #3
//CapacitiveSensor   cs4 = CapacitiveSensor(sPin, rPin4);   // create capSense obbject #4

// the setup function runs once when you press reset or power the board
void setup() {
  // reserve 200 bytes for the inputString:
  inputString.reserve(200);
  Serial.begin(9600);

  pinMode(VibraFrontLeft, OUTPUT);  // initialize left front vibration motor pin as an output.
  pinMode(VibraFrontRight, OUTPUT);  // initialize right front vibration motor pin as an output.
  pinMode(VibraMiddleLeft, OUTPUT);  // initialize left middle vibration motor pin as an output.
  pinMode(VibraMiddleRight, OUTPUT);  // initialize right middle vibration motor pin as an output.
  pinMode(VibraRearLeft, OUTPUT);  // initialize left rear vibration motor pin as an output.
  pinMode(VibraRearRight, OUTPUT);  // initialize right rear vibration motor pin as an output.
  pinMode(VibraCenter, OUTPUT);  // initialize center vibration motor pin as an output.

  frontServo.attach(ServoPinFront);  // attaches the servo on pin ServoPinFront to a servo object
  rearServo.attach(ServoPinRear); // attaches the servo on pin ServoPinRear to a servo object

  cs1.set_CS_AutocaL_Millis(0xFFFFFFFF);   // turn off autocalibrate sensor 1 ...
  //  cs2.set_CS_AutocaL_Millis(0xFFFFFFFF);   // turn off autocalibrate sensor 2 ...
  cs3.set_CS_AutocaL_Millis(0xFFFFFFFF);   // turn off autocalibrate sensor 3 ...
  //  cs4.set_CS_AutocaL_Millis(0xFFFFFFFF);  // turn off autocalibrate sensor 4 ...

  shift(PARKING);

  Serial.println("AVAILABLE COMMANDS:");
  Serial.println("################################################");
  Serial.println("FL = Front Left       |   FR   = Front Right");
  Serial.println("ML = Middle Left      |   MR   = Middle Right");
  Serial.println("RL = Rear Left        |   RR   = Rear Right");
  Serial.println("F =  Forward vibrate  |   R = Reverse vibrate");
  Serial.println("ALL = All vibrate     |   TEST = System Test");
  Serial.println("C  = Center           |   SN = Shift Neutral");
  Serial.println("SF = Shift Forward    |   SR = Shift Reverse");
  Serial.println("################################################");
  Serial.println("");
}


// the loop function runs over and over again forever
void loop() {

  touchListener();
  // Detect Touch Events
  if (isTouched()) {
    int temp = 0;

    if (millis() - lastTouchTime > TOUCH_THRESHOD && lastTouchTime != -1) {
      if ((temp = touchListener()) != lastTouch && lastTouch != -1) {
        // Gesture detected
      } else {
        // both pressed
        // check for lastTouch != -1

      }
      lastTouch = touchListener();

    }

    lastTouchTime = millis();
  }



  // Serial Events
  if (stringComplete) {
    handleSerialCommands(inputString);
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  

}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {

  while (Serial.available()) {

    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;

    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

boolean isTouched() {

  long total1 =  cs1.capacitiveSensor(20);
  //  long total2 =  cs2.capacitiveSensor(20);
  //  long total3 =  cs3.capacitiveSensor(20);
  long total2 =  0; // disable cs2
  long total3 =  cs3.capacitiveSensor(20);
  long total4 =  0;
  boolean isTouched = false;

  if (total1 > CAPACITIVE_THRESHOLD) {
    isTouched = true;
  }

  if (total2 > CAPACITIVE_THRESHOLD) {
    isTouched = true;
  }

  if (total3 > CAPACITIVE_THRESHOLD) {
    isTouched = true;
  }

  if (total4 > CAPACITIVE_THRESHOLD) {
    isTouched = true;
  }



  return isTouched;


}



int touchListener() {

  return -1;
}



boolean handleSerialCommands(String cmd) {
  cmd.toUpperCase();
  Serial.print("Serial Command: ");
  cmd = cmd.substring(0, cmd.length() - 1);
  Serial.print(cmd);
  Serial.println(" received.");
  boolean retVar = true;
  int vibrationDuration = 1200;
  int pulsesLow = 3;
  int pulsesHigh = 5;

  if (cmd.equals("TEST")) {
    testHardware();
  } else if (cmd.equals("FL") || cmd.equals("FRONTLEFT")) {
    pulseVibrate(VibraFrontLeft, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("FR") || cmd.equals("FRONTRIGHT")) {
    pulseVibrate(VibraFrontRight, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("ML") || cmd.equals("MIDDLELEFT")) {
    pulseVibrate(VibraMiddleLeft, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("MR") || cmd.equals("MIDDLERIGHT")) {
    pulseVibrate(VibraMiddleRight, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("RL") || cmd.equals("REARLEFT")) {
    pulseVibrate(VibraRearLeft, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("RR") || cmd.equals("REARRIGHT")) {
    pulseVibrate(VibraRearRight, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("C") || cmd.equals("CENTER")) {
    pulseVibrate(VibraCenter, vibrationDuration, HIGH, pulsesLow);
  } else if (cmd.equals("SN") || cmd.equals("NEUTRAL"))  {
    // shift(NEUTRAL);
     shift(PARKING);
  } else if (cmd.equals("SF") || cmd.equals("FORWARD")) {
    shift(NEUTRAL);
    delay(50);
    shift(FORWARD);
  } else if (cmd.equals("SR") || cmd.equals("REVERSE")) {
    shift(NEUTRAL);
    delay(50);
    shift(REVERSE);
  } else if (cmd.equals("F")) {
    Serial.println("F Command");
    vibrateDirect(VibraFrontLeft, HIGH);
    vibrateDirect(VibraFrontRight, HIGH);
    delay(200);
    vibrateDirect(VibraFrontLeft, LOW);
    vibrateDirect(VibraFrontRight, LOW);
    delay(200);
    vibrateDirect(VibraFrontLeft, HIGH);
    vibrateDirect(VibraFrontRight, HIGH);
    delay(200);
    vibrateDirect(VibraFrontLeft, LOW);
    vibrateDirect(VibraFrontRight, LOW);
    delay(200);
    vibrateDirect(VibraFrontLeft, HIGH);
    vibrateDirect(VibraFrontRight, HIGH);
    delay(200);
    vibrateDirect(VibraFrontLeft, LOW);
    vibrateDirect(VibraFrontRight, LOW);

  }  else if (cmd.equals("R") ) {
    Serial.println("R Command");
    vibrateDirect(VibraRearLeft, HIGH);
    vibrateDirect(VibraRearRight, HIGH);
    delay(200);
    vibrateDirect(VibraRearLeft, LOW);
    vibrateDirect(VibraRearRight, LOW);
    delay(200);
    vibrateDirect(VibraRearLeft, HIGH);
    vibrateDirect(VibraRearRight, HIGH);
    delay(200);
    vibrateDirect(VibraRearLeft, LOW);
    vibrateDirect(VibraRearRight, LOW);
    delay(200);
    vibrateDirect(VibraRearLeft, HIGH);
    vibrateDirect(VibraRearRight, HIGH);
    delay(200);
    vibrateDirect(VibraRearLeft, LOW);
    vibrateDirect(VibraRearRight, LOW);

  }  else if (cmd.equals("ON") ) {
    Serial.println("ON Command");
    
     awake();
  }  else if (cmd.equals("OFF") ) {
    Serial.println("Off Command");
    shift(PARKING);
    



  }  else if (cmd.equals("ALL") ) {
    Serial.println("ALL Command");
    testHardware();
  }
  else if (cmd.equals("AWAKE") ) {
    Serial.println("AWAKE Command");
    awake();
  }  else {
    Serial.println("Not a valid command!");
    retVar = false;
  }
  return retVar;


}


boolean shift(int gear) {

  if (gear == NEUTRAL) {


    frontServo.write(SERVO_POS_LOW_FRONT);
    rearServo.write(SERVO_POS_LOW_REAR);
    Serial.println("Shifting Neutral");
    return true;
  } else if (gear == FORWARD) {


    frontServo.write(SERVO_POS_HIGH_FRONT);
    rearServo.write(SERVO_POS_LOW_REAR);
    Serial.println("Shifting Forward");
    return true;
  } else if (gear == REVERSE) {
    frontServo.write(SERVO_POS_LOW_FRONT);
    rearServo.write(SERVO_POS_HIGH_REAR);
    Serial.println("Shifting Reverse");
    return true;

  } else if (gear == AWAKE) {
    frontServo.write(SERVO_POS_LOW_FRONT);
    rearServo.write(SERVO_POS_LOW_REAR);
    delay(80);
    frontServo.write(SERVO_POS_HIGH_FRONT);
    rearServo.write(SERVO_POS_HIGH_REAR);
    Serial.println("Awake");
    return true;
  }
  else if (gear == PARKING) {
    frontServo.write(SERVO_POS_HIGH_FRONT);
    rearServo.write(SERVO_POS_HIGH_REAR);

    Serial.println("Shifting Parking");
    return true;
  }  else {
    Serial.println("Wrong gear selected!");
    return false;
  }

}


void vibrate(int motor, int duration, int force) {

  digitalWrite(motor, force);
  delay(duration);
  digitalWrite(motor, LOW);

}

void vibrateAll(int time) {

  vibrateDirect(VibraFrontLeft, 255);
  vibrateDirect(VibraFrontRight, 255);
  vibrateDirect(VibraMiddleLeft, 255);
  vibrateDirect(VibraMiddleRight, 255);
  vibrateDirect(VibraRearLeft, 255);
  vibrateDirect(VibraRearRight, 255);
  delay(time);
  vibrateDirect(VibraFrontLeft, LOW);
  vibrateDirect(VibraFrontRight, LOW);
  vibrateDirect(VibraMiddleLeft, LOW);
  vibrateDirect(VibraMiddleRight, LOW);
  vibrateDirect(VibraRearLeft, LOW);
  vibrateDirect(VibraRearRight, LOW);
  delay(time);

}

void vibrateDirect(int motor, int force) {
  digitalWrite(motor, force);
}

void vibrateDirectPWM(int motor, int force) {
  analogWrite(motor, force);
}

void pulseVibrate(int motor, int duration, int force, int pulses) {

  Serial.print("Pulse Vibrate Motor ");
  Serial.print(motor);
  Serial.print(" for ");
  Serial.print(duration);
  Serial.print("millis with ");
  Serial.print(pulses);
  Serial.println(" pulses");

  for (int i = 0; i < pulses; i++) {
    digitalWrite(motor, force);
    delay(duration / pulses / 2);
    digitalWrite(motor, LOW);
    delay(duration / pulses / 2);
  }
  digitalWrite(motor, LOW);

}



void pulseVibrateAll(int duration, int force, int pulses) {

  Serial.print("Pulse Vibrate all Motors for ");
  Serial.print(duration);
  Serial.print("millis with ");
  Serial.print(pulses);
  Serial.println(" pulses");

  for (int i = 0; i < pulses; i++) {
    digitalWrite(VibraFrontLeft, force);
    digitalWrite(VibraFrontRight, force);
    digitalWrite(VibraMiddleLeft, force);
    digitalWrite(VibraMiddleRight, force);
    digitalWrite(VibraRearLeft, force);
    digitalWrite(VibraRearRight, force);
    delay(duration / pulses / 2);
    digitalWrite(VibraFrontLeft, LOW);
    digitalWrite(VibraFrontRight, LOW);
    digitalWrite(VibraMiddleLeft, LOW);
    digitalWrite(VibraMiddleRight, LOW);
    digitalWrite(VibraRearLeft, LOW);
    digitalWrite(VibraRearRight, LOW);
    delay(duration / pulses / 2);
  }
  digitalWrite(VibraFrontLeft, LOW);
  digitalWrite(VibraFrontRight, LOW);
  digitalWrite(VibraMiddleLeft, LOW);
  digitalWrite(VibraMiddleRight, LOW);
  digitalWrite(VibraRearLeft, LOW);
  digitalWrite(VibraRearRight, LOW);

}

void awake() {
  vibrateAll(42);
}


void testHardware() {
  Serial.println("Demo");
  Serial.println();

  int vibrationDuration = 1200;
   int vibrationDurationLOW = 800;
  int pulsesLow = 3;
  int pulsesHigh = 5;
  int breakTime = 3500;

  vibrateAll(42);
  delay(1000);

  Serial.println("Front Right");
  pulseVibrate(VibraFrontRight, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);

  Serial.println("Middle Left");
  pulseVibrate(VibraMiddleLeft, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);

  Serial.println("Rear Right");
  pulseVibrate(VibraRearRight, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);

  Serial.println("Front Left");
  pulseVibrate(VibraFrontLeft, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);

  Serial.println("Rear Left");
  pulseVibrate(VibraRearLeft, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);

  Serial.println("Middle Right");
  pulseVibrate(VibraMiddleRight, vibrationDuration, HIGH, pulsesLow);
  delay(breakTime);




}

