/*************************************************************
  Motor Shield 1-Channel DC Motor Demo
  by Randy Sarafan

  For more information see:
  https://www.instructables.com/id/Arduino-Motor-Shield-Tutorial/

*************************************************************/
#include <CurieBLE.h>
BLEPeripheral blePeripheral;       // BLE Peripheral Device (the board you're programming)
BLEService WildcatService("FFFF"); // BLE WildCat Service
BLECharacteristic WildcatChar("2A37",  // standard 16-bit characteristic UUID
                              BLERead | BLENotify | BLEWrite | BLEIndicate,  4);
const int TRIG_PIN = 6;
const int ECHO_PIN = 7;

long duration;
int distance;

void setup() {
  Serial.begin(9600);

  pinMode(12, OUTPUT);  // Channel A motor direction
  pinMode(9, OUTPUT);   // Channel A break
  pinMode(3, OUTPUT);   // Channel A PWM (locked to max)
  analogWrite(3, 255);

  pinMode(13, OUTPUT);  // Channel B motor direction
  pinMode(8, OUTPUT);   // Channel B break
  pinMode(11, OUTPUT);  // Channel B PWM (locked to max)
  analogWrite(11, 255);

  //Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);  // LED - Board
  pinMode(4, OUTPUT);   // LED - Top

  blePeripheral.setLocalName("Wildcat_RC_Car");
  blePeripheral.setAdvertisedServiceUuid(WildcatService.uuid());  // add the service UUID
  blePeripheral.addAttribute(WildcatService);   // Add the BLE WildCat service
  blePeripheral.addAttribute(WildcatChar);  // add the WildCat Measurement characteristic

  blePeripheral.begin();
  Serial.println("Bluetooth device active, waiting for connections...");
}

void setReverse(bool isInReverse) {
  digitalWrite(13, isInReverse ? HIGH : LOW);
}

void go() {
  digitalWrite(8, LOW);     // Disable break
}

void stop() {
  digitalWrite(8, HIGH);    // Engage break
}

void Left() {
  digitalWrite(9, HIGH);  
}

void Right() {
  digitalWrite(9, LOW);  
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

  //delay(100);

}

void loop() {
  // listen for BLE peripherals to connect:
BLECentral central = blePeripheral.central();

//do_ultrasonic_things();


if (!central) {
  Serial.print("Waiting for connection \n");
  delay(500);
}

  // if a central is connected to peripheral:
if (central) {
      Serial.print("Connected to central: ");
      Serial.println(central.address());  // print the central's MAC address:
  while(central.connected()){
    // turn on the LED to indicate the connection:
    digitalWrite(4, HIGH);
    delay(1000);
    digitalWrite(4, LOW);
    delay(1000);
  }
  Serial.print("Exiting connection: ");

}

#if 0
    // Go forward
    setReverse(false);
    go();
    delay(3000);

    // Stop
    stop();
    delay(3000);

    // Go backward
    setReverse(true);
    go();
    delay(3000);

    // Stop
    stop();
    delay(3000);
#endif
  }
