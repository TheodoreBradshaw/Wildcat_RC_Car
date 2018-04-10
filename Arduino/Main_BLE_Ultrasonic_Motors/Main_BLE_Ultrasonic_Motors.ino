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

/*Ultrasonic Sensors*/
const int FRONT_TRIG_PIN = 5;
const int FRONT_ECHO_PIN = 4;
const int BACK_TRIG_PIN = 6;
const int BACK_ECHO_PIN = 7;
const int FRONT_RIGHT_TRIG_PIN = A4; //blue
const int FRONT_RIGHT_ECHO_PIN = A5; //purple
const int FRONT_LEFT_TRIG_PIN = A2; //grey
const int FRONT_LEFT_ECHO_PIN = A3; //white

/*Power Motor */
const int POWER_DIR_PIN = 13;
const int POWER_BRAKE_PIN = 8;
const int POWER_PWM_PIN = 11;

/*Directional Motor*/
const int DIR_DIR_PIN = 12;
const int DIR_BRAKE_PIN = 9;
const int DIR_PWM_PIN = 3;

long frontDuration;
int frontDistance = 0;
long backDuration;
int backDistance = 0;
long rightDuration;
int rightDistance = 0;
long leftDuration;
int leftDistance = 0;

int state;

const int BRAKE_INPUT = 0;
const int FORWARD_INPUT = 1;
const int BACKWARDS_INPUT = 2;
const int LEFT_INPUT = 3;
const int RIGHT_INPUT = 4;
const int STRAIGHT_INPUT = 5;
const int AUTO_INPUT = 6;

bool isAutonomous = false;
bool isReverse = false;

const int CLOSE_THRESHOLD = 30;
const int MEDIUM_THRESHOLD = 100;

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
