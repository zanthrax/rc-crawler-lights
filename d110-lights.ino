// Input pins
const int throttle = 12;
const int steering = 13;

// Output pins
const int headlights = 2; // headlights
const int runningLights = 3; // running lights, should be a PWM capable pin
const int frontIndicators[2] = {4, 5}; // indicators left and right
const int rearIndicators[sizeof(frontIndicators)] = {7, 8}; // indicators left and right
const int taillights = 9; // combined tail and brake lights, should be a PWM capable pin
const int reverse = 10; // reverse lights
//const int fog = 0; // fog light, not used
const int brake = 11; // third brake light

// Settings
const int indicatorsInterval = 500; // milliseconds between switching indicators on and off
const int taillightBrightness = 40; // duty cycle for tail light brightness: 0 = 0%, 255 = 100%, brakelights are 255
const int runningLightBrightness = 30; // duty cycle for running light brightness: 0 = 0%, 255 = 100%
const bool flipThrottle = false; // flip forward and reverse
const bool flipSteering = false; // flip left and right indicators

// RC pulse length, by making the deadspot a range we filter out small fluctuations
const int rcThrottleDead = 1470;
const int throttleDeadTreshold = 100;
const int throttleDeadLow = rcThrottleDead - throttleDeadTreshold;
const int throttleDeadHigh = rcThrottleDead + throttleDeadTreshold;

const int rcSteeringDead = 1530;
const int steeringDeadTreshold = 100;
const int steeringDeadLow = rcSteeringDead - steeringDeadTreshold;
const int steeringDeadHigh = rcSteeringDead + steeringDeadTreshold;

// Store which indicators should be blinking, used to blink without using delay()
bool indicatorSwitch[sizeof(indicators)] = {false};
bool indicatorState[sizeof(indicators)] = {false};
unsigned long previousIndicatorMillis[sizeof(indicators)] = {0};


// Vars for throttle and steering positions
int throttlePosition;
bool throttleForward;
bool throttleReverse;
bool throttleDead;
int steeringPosition;
bool steeringLeft;
bool steeringRight;
bool steeringDead;

void setup() {
  pinMode(throttle, INPUT);
  pinMode(steering, INPUT);
  pinMode(headlights, OUTPUT);
  pinMode(runningLights, OUTPUT);
  // Front and rear indicators each have their own pin so we do not have to run 3 LEDs on one pin which would exceed to max. current on some Arduino board.
  pinMode(frontIndicators[0], OUTPUT); // left
  pinMode(frontIndicators[1], OUTPUT); // right
  pinMode(rearIndicators[0], OUTPUT); // left
  pinMode(rearIndicators[1], OUTPUT); // right
  pinMode(taillights, OUTPUT);
  pinMode(reverse, OUTPUT);
  //pinMode(fog, OUTPUT);
  pinMode(brake, OUTPUT);

  // Reset blinking indicators
  for (int i = 0; i < sizeof(frontIndicators); i++) {
    indicatorSwitch[i] = false;
    indicatorState[i] = false;
    previousIndicatorMillis[i] = 0;
  }

  // Init serial com for debugging
  Serial.begin(9600);
  Serial.println("Setup complete");
}

void loop() {
  unsigned long currentMillis = millis(); // for blink without delay()

  /*
     Get throttle position from RC receiver.
     pulseIn() looks for a pulse on a digital pin. RC receivers send out PWM signals, the longest possible pulse is 20milliseconds.
     A timeout in microseconds is expected by pulseIn().
  */
  throttlePosition = pulseIn(throttle, HIGH, 25000);
  if (throttlePosition) {
    throttleForward = (throttlePosition > throttleDeadHigh) != flipThrottle;
    throttleReverse = (throttlePosition < throttleDeadLow) != flipThrottle;
    throttleDead = !throttleForward && !throttleReverse;    
  } else {
    // no throttle signal, receiver not connected or not switched on? do nothing.
    throttleForward = false;
    throttleReverse = false;
    throttleDead = false;
  }


  /*
      Get steering position from RC receiver.
      Without a signal from the RC receiver the hazard light will activate.
  */
  steeringPosition = pulseIn(steering, HIGH, 25000);
  if (steeringPosition) {
    steeringLeft = (steeringPosition < steeringDeadLow) != flipSteering;
    steeringRight = (steeringPosition > steeringDeadHigh) != flipSteering;
    steeringDead = !steeringLeft && !steeringRight;
  } else {
    // no steering signal, blink all indicators
    steeringLeft = true;
    steeringRight = true;
    steeringDead = false;
  }

  /*
     Switch lights.
  */

  // The simple stuff
  analogWrite(runningLights, runningLightBrightness);
  //digitalWrite(fog, LOW);
  digitalWrite(brake, throttleDead);
  digitalWrite(reverse, throttleReverse);

  /*
     Headlights.
     Off by default, get turned on the first time driving forward.
     @TODO: turn off headlights after a certain amount of time has passed without any thottle input.
  */
  if (throttleForward) {
    digitalWrite(headlights, HIGH);
  }

  /*
     Combined tail- and brakelights.
     Dimmed by default, on full for braking.
  */
  analogWrite(taillights, throttleDead ? 255 : taillightBrightness);

  /*
      Indicators.
      Derived from the Arduino Blink Without Delay tutorial.
      indicatorSwitch knows which indicators should be blinking.
      indicatorState knows which indicator LEDs should be on.
  */
  indicatorSwitch[0] = steeringLeft;
  indicatorSwitch[1] = steeringRight;
  // For left and right side indicators
  for (int i = 0; i < sizeof(frontIndicators); i++) {
    if (indicatorSwitch[i]) {
      // Blink indicators
      if (currentMillis - previousIndicatorMillis[i] >= indicatorsInterval) {
        // Interval passed, invert indicators
        indicatorState[i] = ! indicatorState[i];
        previousIndicatorMillis[i] = currentMillis;
      }
    } else {
      // Turn off indicators, reset blink interval
      indicatorState[i] = LOW;
      previousIndicatorMillis[i] = 0;
    }
    digitalWrite(frontIndicators[i], indicatorState[i]);
    digitalWrite(rearIndicators[i], indicatorState[i]);
  }

  /*
     Debug info.
  */
  if (false) {
    // RC signal debugging
    Serial.print("T "); // Throttle
    Serial.print(String(throttlePosition));
    Serial.print(" ");
    Serial.print(throttleForward ? "1" : "0");
    Serial.print(throttleDead ? "1" : "0");
    Serial.print(throttleReverse ? "1" : "0");
    Serial.print(" S "); // Steering
    Serial.print(String(steeringPosition));
    Serial.print(" ");
    Serial.print(steeringLeft ? "1" : "0");
    Serial.print(steeringDead ? "1" : "0");
    Serial.print(steeringRight ? "1" : "0");

    // Indicator debugging
    Serial.print(" Lmili "); // Left last timeout
    Serial.print(previousIndicatorMillis[0]);
    Serial.print(" Lsw "); // Left Switch Position
    Serial.print(indicatorSwitch[0]);
    Serial.print(" Lst "); // Left LED State
    Serial.print(indicatorState[0]);

    // New line
    Serial.println("");
  }
}


