/*************************************************************
  Written completely from scratch by Teddy. No help from Internet at all.

*************************************************************/
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
  Serial.begin(9600);

  // Init the directional motor
  //pinMode(12, OUTPUT);  // Channel A motor direction
  pinMode(DIR_DIR_PIN, OUTPUT);   // Channel A break
  pinMode(DIR_PWM_PIN, OUTPUT);   // Channel A PWM (locked to max)
  analogWrite(DIR_PWM_PIN, 255);

  // Init the power (forward/reverse) motor
  pinMode(POWER_DIR_PIN, OUTPUT);  // Channel B motor direction
  pinMode(POWER_BRAKE_PIN, OUTPUT);   // Channel B brake
  pinMode(POWER_PWM_PIN, OUTPUT);  // Channel B PWM (locked to max)
  analogWrite(POWER_PWM_PIN, 255);

  // Init Ultrasonic sensor
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);


  pinMode(LED_BUILTIN, OUTPUT);  // LED - Board
  pinMode(4, OUTPUT);   // LED - Top
}

void setReverse(bool isInReverse) {
  digitalWrite(POWER_DIR_PIN, isInReverse ? HIGH : LOW);
}

void go() {
  digitalWrite(POWER_BRAKE_PIN, LOW);     // Disable brake
}

void brake() {
  digitalWrite(POWER_BRAKE_PIN, HIGH);    // Engage brake
}

void Left() {
  digitalWrite(DIR_DIR_PIN, HIGH);  
}

void Right() {
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

void loop() {
  // listen for BLE peripherals to connect:
  //do_ultrasonic_things();
