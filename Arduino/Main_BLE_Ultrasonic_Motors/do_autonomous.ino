void do_autonomous(DistanceStruct distances) {
  Serial.println("I am a real car");
  brake();
  digitalWrite(FRONT_LED, HIGH);
  digitalWrite(BACK_LED, HIGH);

  if (distances.front != CLOSE) {
    setReverse(false);
    disableBrake();
    Serial.println("Would go forward");
  }
  else {
    if (distances.back != CLOSE) {
      setReverse(true);
      disableBrake();
      Serial.println("Would go Back");
    }
  }
}
