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
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

/*Power Motor */
const int POWER_DIR_PIN = 13;
const int POWER_BRAKE_PIN = 8;
const int POWER_PWM_PIN = 11;

/*Directional Motor*/
const int DIR_DIR_PIN = 9;
const int DIR_PWM_PIN = 3;

long duration;
int distance;

void setup() {
  Serial.begin(9600);    // initialize serial communication
  pinMode(4, OUTPUT);   // initialize the LED on pin 13 to indicate when a central is connected
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
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Init the power (forward/reverse) motor
  pinMode(POWER_DIR_PIN, OUTPUT);  // Channel B motor direction
  pinMode(POWER_BRAKE_PIN, OUTPUT);   // Channel B brake
  pinMode(POWER_PWM_PIN, OUTPUT);  // Channel B PWM (locked to max)
  analogWrite(POWER_PWM_PIN, 255);

  // Init the directional motor
  //pinMode(12, OUTPUT);  // Channel A motor direction
  pinMode(DIR_DIR_PIN, OUTPUT);   // Channel A break
  pinMode(DIR_PWM_PIN, OUTPUT);   // Channel A PWM (locked to max)
  analogWrite(DIR_PWM_PIN, 255);

  brake();
}

//Funciton Defs
void setReverse(bool isInReverse) {
  digitalWrite(POWER_DIR_PIN, isInReverse ? HIGH : LOW);
}

void go() {
  digitalWrite(POWER_BRAKE_PIN, LOW);     // Disable brake
}

void brake() {
  digitalWrite(POWER_BRAKE_PIN, HIGH);    // Engage brake
}

void left() {
  digitalWrite(DIR_DIR_PIN, HIGH);  
}

void right() {
  digitalWrite(DIR_DIR_PIN, LOW);  
}

void do_ultrasonic_things() {
  // Clears the trig pin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

// Sets the TRIG_PIN on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

// Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  duration = pulseIn(ECHO_PIN, HIGH);

  //Serial.print("Duration: ");
  //Serial.println(duration);
  distance = duration*0.034/2;  //distance in CM

  Serial.print("Distance (cm): ");
  Serial.println(distance);
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
    // turn on the LED to indicate the connection:
    digitalWrite(4, HIGH);

    byte bytearr[1] = {0};
    WildcatChar.setValue(bytearr, 5);  // update the heart rate measurement characteristic

    // check the heart rate measurement every 200ms
    // as long as the central is still connected:
    while (central.connected()) {
      //long currentMillis = millis();
      if (WildcatChar.written()) {
        const byte* inptr = WildcatChar.value();
        if (inptr) {
          switch (inptr[0]){
#ifdef DEBUG
            case 0:
                Serial.println("Would brake");
                break;
            case 1:
                Serial.println("Would go forward");
                break;
            case 2:
                Serial.println("Would go backward");
                break;
            case 3:
                Serial.println("Would turn left");
                break;
            case 4:
                Serial.println("Would turn right");
                break;
#else
            case 0:
                brake();
                break;
            case 1: 
                setReverse(false);
                go();
                break;
              case 2:
                setReverse(true);
                go();
                break;
              case 3:
                left();
                break;
              case 4:
                right();
                break;
#endif
          }
        }
      }
    }
    // when the central disconnects, turn off the LED:
    digitalWrite(4, LOW);
    brake();  // stop driving
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
