/*************************************************************
  Written completely from scratch by Teddy. No help from Internet at all.

*************************************************************/
/*Bluetooth Low Energy Pre Setup*/
#include <CurieBLE.h>

BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService WildcatService("180D"); // BLE Heart Rate Service

// BLE Heart Rate Measurement Characteristic"
BLECharacteristic WildcatChar("2A37",  BLERead | BLENotify | BLEWrite, 2); // standard 16-bit characteristic UUID

// remote clients will be able to get notifications if this characteristic changes
// the characteristic is 2 bytes long as the first field needs to be "Flags" as per BLE specifications
// https://developer.bluetooth.org/gatt/characteristics/Pages/CharacteristicViewer.aspx?u=org.bluetooth.characteristic.heart_rate_measurement.xml

int oldHeartRate = 0;  // last heart rate reading from analog input
long previousMillis = 0;  // last time the heart rate was checked, in ms
/*************************************************************
  Pin constant assignments
*************************************************************/
/*Ultrasonic Sensors*/
const int FRONT_TRIG_PIN = A3;
const int FRONT_ECHO_PIN = A2;
const int BACK_TRIG_PIN = 3;
const int BACK_ECHO_PIN = 2;
const int FRONT_RIGHT_TRIG_PIN = 6; //A1
const int FRONT_RIGHT_ECHO_PIN = 5; //A0
const int FRONT_LEFT_TRIG_PIN = A5;
const int FRONT_LEFT_ECHO_PIN = A4;

/*LEDs*/
//const int FRONT_LED = 6;  // FIRST
//const int LEFT_LED = 5;    // SECOND
const int FRONT_LED = 4;   // THIRD
const int BACK_LED = 7;    // FOURTH

/*Power Motor */
const int POWER_DIR_PIN = 13;
const int POWER_BRAKE_PIN = 8;
const int POWER_PWM_PIN = 11;

/*Directional Motor*/
const int DIR_DIR_PIN = 12;
const int DIR_BRAKE_PIN = 9;
const int DIR_PWM_PIN = 3;

/*************************************************************
MISC GLOBAL VARS
*************************************************************/
/*SonarVars*/
long frontDuration;
int frontDistance = 0;
long backDuration;
int backDistance = 0;
long rightDuration;
int rightDistance = 0;
long leftDuration;
int leftDistance = 0;

// int state; // Unknown Var

/*Android to Arduino Input Commands*/
const int BRAKE_INPUT = 0;
const int FORWARD_INPUT = 1;
const int BACKWARDS_INPUT = 2;
const int LEFT_INPUT = 3;
const int RIGHT_INPUT = 4;
const int STRAIGHT_INPUT = 5;
const int AUTO_INPUT = 6;

/*State Booleans*/
bool isAutonomous = false;

/*Distance Threshold Vars & Structs*/
const int CLOSE_THRESHOLD = 30;   // cm
const int MEDIUM_THRESHOLD = 100; // cm

enum DistanceValue {
  CLOSE, 
  MEDIUM,
  FAR,
};

typedef struct DistanceStruct {
  enum DistanceValue front = FAR;
  enum DistanceValue right = FAR;
  enum DistanceValue left = FAR;
  enum DistanceValue back = FAR;
} DistanceStruct;
