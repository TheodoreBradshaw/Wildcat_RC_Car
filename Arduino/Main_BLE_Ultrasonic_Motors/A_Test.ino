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


  pinMode(FRONT_LED, OUTPUT); //LED
  //pinMode(LEFT_LED, OUTPUT); //LED
  //  pinMode(RIGHT_LED, OUTPUT); //LED
  pinMode(BACK_LED, OUTPUT); //LED

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

void read_sonar_distances() {
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
//  Serial.print("RightDistance (cm): ");
//  Serial.println(rightDistance);
//  Serial.print("LeftDistance (cm): ");
//  Serial.println(leftDistance);
}

//#define DEBUG

int define_range_category(int dist) {
  if (dist < CLOSE_THRESHOLD) {
    return CLOSE;
  }
  else if (dist < MEDIUM_THRESHOLD) {
    return MEDIUM;
  }
  return FAR;
}

struct DistanceStruct check_for_obstacles() {
  DistanceStruct distances;
  read_sonar_distances();
  distances.front = (enum DistanceValue) define_range_category(frontDistance);
  distances.right = (enum DistanceValue) define_range_category(rightDistance);
  distances.left = (enum DistanceValue) define_range_category(leftDistance);
  distances.back = (enum DistanceValue) define_range_category(backDistance);
  return distances;
}

void do_autonomous(DistanceStruct distances);

void do_manual(byte command, DistanceStruct distances);

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
    DistanceStruct distances;

    while (central.connected())
    {
      if (loopCount % 1 == 0) {
        distances = check_for_obstacles();
      }

      byte* inptr = NULL;
      byte command = 0;
      if (WildcatChar.written()) {
        inptr = (byte*) WildcatChar.value();
        if (inptr) {
          command = inptr[0];
          if (command == AUTO_INPUT) {
            isAutonomous = !isAutonomous;
          }
        }
      }
      if (isAutonomous) {
        do_autonomous(distances);
      } else {
        if (inptr) {
          do_manual(command, distances);
        }
      }

      // LED STUFF
      if (distances.front == CLOSE) {
        digitalWrite(FRONT_LED, LOW);
      } else {
        digitalWrite(FRONT_LED, HIGH);
      }
      if (distances.back == CLOSE) {
        digitalWrite(BACK_LED, LOW);
      } else {
        digitalWrite(BACK_LED, HIGH);
      }
      //if (distances.left == CLOSE) {
      //  digitalWrite(LEFT_LED, LOW);
      //} else {
      // digitalWrite(LEFT_LED, HIGH);
      //}
      //      if (distances.right == CLOSE) {
      //        digitalWrite(RIGHT_LED, LOW);
      //      } else {
      //        digitalWrite(RIGHT_LED, HIGH);
      //      }

      loopCount++;
    }

    // when the central disconnects, turn off the LED:
    //digitalWrite(4, LOW);
    brake();  // stop driving
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
