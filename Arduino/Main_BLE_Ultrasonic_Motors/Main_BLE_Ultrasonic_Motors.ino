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

bool isFrontObstacle = false;
bool isBackObstacle = false;

const int BRAKE_INPUT = 0;
const int FORWARD_INPUT = 1;
const int BACKWARDS_INPUT = 2;
const int LEFT_INPUT = 3;
const int RIGHT_INPUT = 4;
const int STRAIGHT_INPUT = 5;


void setup() {
  Serial.begin(9600);    // initialize serial communication
  pinMode(LED_BUILTIN, OUTPUT);  // LED - Board

  //Bluetooth Stuff
  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet */
  blePeripheral.setLocalName("Wildcat_RC_Car");
  blePeripheral.setAdvertisedServiceUuid(WildcatService.uuid());  // add the service UUID
  blePeripheral.addAttribute(WildcatService);   // Add the BLE Heart Rate service
  blePeripheral.addAttribute(WildcatChar); // add the Heart Rate Measurement characteristic

  /* Now activate the BLE device.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */
  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");

  // Init Ultrasonic sensor
  pinMode(FRONT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_ECHO_PIN, INPUT);
  pinMode(BACK_TRIG_PIN, OUTPUT);
  pinMode(BACK_ECHO_PIN, INPUT);
  pinMode(FRONT_RIGHT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_RIGHT_ECHO_PIN, INPUT);
  pinMode(FRONT_LEFT_TRIG_PIN, OUTPUT);
  pinMode(FRONT_LEFT_ECHO_PIN, INPUT);

  // Init the power (forward/reverse) motor
  pinMode(POWER_DIR_PIN, OUTPUT);  // Channel B motor direction
  pinMode(POWER_BRAKE_PIN, OUTPUT);   // Channel B brake
  pinMode(POWER_PWM_PIN, OUTPUT);  // Channel B PWM (locked to max)
  analogWrite(POWER_PWM_PIN, 255);

  // Init the directional motor
  pinMode(DIR_DIR_PIN, OUTPUT);  // Channel A motor direction
  pinMode(DIR_BRAKE_PIN, OUTPUT);   // Channel A break
  pinMode(DIR_PWM_PIN, OUTPUT);   // Channel A PWM (locked to max)
  analogWrite(DIR_PWM_PIN, 255);

  brake();
}

//Funciton Defs
void setReverse(bool isInReverse) {
  digitalWrite(POWER_DIR_PIN, isInReverse ? HIGH : LOW);
}

void disableBrake() {
  digitalWrite(POWER_BRAKE_PIN, LOW);     // Disable brake
}

void brake() {
  digitalWrite(POWER_BRAKE_PIN, HIGH);    // Engage brake
}

void right() {
  digitalWrite(DIR_DIR_PIN, HIGH);
  digitalWrite(DIR_BRAKE_PIN, LOW);
}

void left() {
  digitalWrite(DIR_DIR_PIN, LOW);
  digitalWrite(DIR_BRAKE_PIN, LOW);
}

void straight() {
  digitalWrite(DIR_BRAKE_PIN, HIGH);
}

void do_ultrasonic_things() {
  // Clears the trig pins
  digitalWrite(FRONT_TRIG_PIN, LOW);
  digitalWrite(FRONT_RIGHT_TRIG_PIN, LOW);
  digitalWrite(FRONT_LEFT_TRIG_PIN, LOW);
  digitalWrite(BACK_TRIG_PIN, LOW);

  // Sets the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(FRONT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_TRIG_PIN, LOW);
  frontDuration = pulseIn(FRONT_ECHO_PIN, HIGH);
  frontDistance = frontDuration * 0.034 / 2; //frontDistance in CM
  
  digitalWrite(FRONT_RIGHT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_RIGHT_TRIG_PIN, LOW);
  rightDuration = pulseIn(FRONT_RIGHT_ECHO_PIN, HIGH);
  rightDistance = rightDuration * 0.034 / 2; //frontDistance in CM
  
  digitalWrite(FRONT_LEFT_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(FRONT_LEFT_TRIG_PIN, LOW);
  leftDuration = pulseIn(FRONT_LEFT_ECHO_PIN, HIGH);
  leftDistance = leftDuration * 0.034 / 2; //frontDistance in CM
  
  digitalWrite(BACK_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(BACK_TRIG_PIN, LOW);
  backDuration =  pulseIn(BACK_ECHO_PIN, HIGH);
  backDistance = backDuration * 0.034 / 2; //frontDistance in CM

  Serial.print("BackDistance (cm): ");
  Serial.println(backDistance);
  Serial.print("FrontDistance (cm): ");
  Serial.println(frontDistance);
  Serial.print("RightDistance (cm): ");
  Serial.println(rightDistance);
  Serial.print("LeftDistance (cm): ");
  Serial.println(leftDistance);
}

//#define DEBUG

void loop() {
  Serial.print("Looping... \n "); // print it

  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    byte bytearr[1] = {0};
    WildcatChar.setValue(bytearr, 7);  // update the heart rate measurement characteristic

    // check the heart rate measurement every 200ms
    // as long as the central is still connected:
    int loopCount = 0;
    bool isReverse = false;
    while (central.connected())
    {
      
      if (loopCount % 3000 == 0) {
        do_ultrasonic_things();
        // Front Obstacle
        if (frontDistance < 50) {
          isFrontObstacle = true;
        }
        else {
          isFrontObstacle = false;
        }
        // Back Obstacle
        if (backDistance < 50) {
          isBackObstacle = true;
        }
        else {
          isBackObstacle = false;
        }
      }

      //long currentMillis = millis();
      if (WildcatChar.written()) {
        const byte* inptr = WildcatChar.value();
        if (inptr) {
          switch (inptr[0]) {
            // Drive Motor
            case BRAKE_INPUT:
              isReverse = false;
              brake();
              Serial.println("Would stop");
              break;
            case FORWARD_INPUT:
              setReverse(false);
              isReverse = true;
              disableBrake();
              Serial.println("Would go forward");
              break;
            case BACKWARDS_INPUT:
              isFrontObstacle = false;
              isReverse = true;
              setReverse(true);
              disableBrake();
              Serial.println("Would go backward");
              break;
            
            // Direction Motor
            case LEFT_INPUT:
              left();
              Serial.println("Would turn left");
              break;
            case RIGHT_INPUT:
              right();
              Serial.println("Would turn right");
              break;
            case STRAIGHT_INPUT:
              straight();
              Serial.println("Would turn Strait");
              break;
          }
        }
      }

      if (isFrontObstacle && !isReverse) {
        // Serial.println("braking");
        brake();
      }
      if (isBackObstacle && isReverse){
        brake();
       }
      loopCount++;
    }

    // when the central disconnects, turn off the LED:
    //digitalWrite(4, LOW);
    brake();  // stop driving
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
