#define MIN3(a, b, c) min(a, min(b, c))

void do_autonomous(DistanceStruct distances) {
  Serial.println("I am a real car");
  disableBrake();
  digitalWrite(FRONT_LED, HIGH);
  digitalWrite(BACK_LED, HIGH);

  // RIGHT/LEFT
  /*
  if (distances.right < FAR) {
    if (isReverse) {
      right();
    }
    else {
      left();
    }
  }  else if (distances.left < FAR) {
    if (isReverse) {
      left();
    } else {
      right();
    }
  } else {
    straight();
  }
  */
  

  // FORWARD/REVERSE
  if (distances.front == FAR && !(distances.left == CLOSE && distances.right == CLOSE)) {
    //delay(1000);
    setReverse(false);
  } else if (distances.left < FAR) {
    left();
    setReverse(true);
    delay(1000);
    straight();
  } else if (distances.right < FAR) {
    right();
    setReverse(true);
    delay(1000);
    straight();
  } else {
    left();
    setReverse(true);
    delay(1000);
    straight();
  }
}
