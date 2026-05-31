#include <ECE3.h>

// Base code.
// 
// *  NOTE: this code will do only three things:
// *    --rotate one wheel, and 
// *    --blink the right front mainboard LED.
// *    
// *  You will need to add more code to
// *  make the car do anything useful. 
// 

uint16_t sensorValues[8]; // right -> left, 0 -> 7

const int left_nslp_pin=31; // nslp ==> awake & ready for PWM
const int left_dir_pin=29;
const int left_pwm_pin=40;

const int right_nslp_pin=11; // nslp ==> awake & ready for PWM
const int right_dir_pin=30;
const int right_pwm_pin=39;

#include <ECE3.h>
uint16_t sensorValues[8];  // right -> left, 0 -> 7

const int left_nslp_pin = 31;  // nslp ==> awake & ready for PWM
const int left_dir_pin = 29;
const int left_pwm_pin = 40;

const int right_nslp_pin = 11;  // nslp ==> awake & ready for PWM
const int right_dir_pin = 30;
const int right_pwm_pin = 39;

const int LED_RF = 41;

const float speed = 30;
float rightspeed;
float leftspeed;

const float kp = 0.005;  //we can adjust to fix jittering!!!!
float preverror = 0;
float prevrighterror = 0;
float prevlefterror = 0;
float prevmiderror = 0;
const float kd = 0.0035;

int blackcounter = 0;
int intersectionCount = 0;

bool turning = false;
unsigned long turnStartTime = 0;
const unsigned long TURN_DURATION = 350;

bool turning2 = false;
unsigned long turn2StartTime = 0;
const unsigned long TURN2_DURATION = 1000;  // tune this

bool otherSide = false;

bool turningRight = false;
bool inLoop = false;

unsigned long turnRightStartTime = 0;
const unsigned long TURN_RIGHT_DURATION = 2000;

bool turning3 = false;
bool turning4 = false;

bool turning5 = false;
bool turning6 = false;
bool turning7 = false;
bool turning8 = false;

bool donewith4 = false;
bool donewith8 = false;

void turnaround() {
  //what do i write here
  // Spin in place: left motor forward, right motor backward
  digitalWrite(left_dir_pin, HIGH);  // Left motor forward
  digitalWrite(right_dir_pin, LOW);  // Right motor backward (or vice versa)

  analogWrite(left_pwm_pin, 100);
  analogWrite(right_pwm_pin, 100);

  delay(600);  // Hold until ~180Â° rotation is complete

  // Restore forward direction for both motors
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin, LOW);
}

// void adjusterror(double errorvalue){
//   float leftspeed = 50 + errorval * kp;
//   float rightspeed = 50 - errorval * kp;
//   //we need to check magnitude to decide how long we move right/left
// }

///////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  pinMode(left_nslp_pin, OUTPUT);
  pinMode(left_dir_pin, OUTPUT);
  pinMode(left_pwm_pin, OUTPUT);

  pinMode(right_nslp_pin, OUTPUT);
  pinMode(right_dir_pin, OUTPUT);
  pinMode(right_pwm_pin, OUTPUT);

  digitalWrite(left_dir_pin, LOW);
  digitalWrite(left_nslp_pin, HIGH);

  digitalWrite(right_dir_pin, LOW);
  digitalWrite(right_nslp_pin, HIGH);

  pinMode(LED_RF, OUTPUT);

  ECE3_Init();

  rightspeed = speed;
  leftspeed = speed;

  // set the data rate in bits/second for serial data transmission
  Serial.begin(9600);
  delay(2000);  //Wait 2 seconds before starting
}

void loop() {
  // put your main code here, to run repeatedly:


  ECE3_read_IR(sensorValues);
  float errorval = 0;
  errorval += sensorValues[0] * -15;
  errorval += sensorValues[1] * -14;
  errorval += sensorValues[2] * -12;
  errorval += sensorValues[3] * -8;
  errorval += sensorValues[4] * 8;
  errorval += sensorValues[5] * 12;
  errorval += sensorValues[6] * 14;
  errorval += sensorValues[7] * 15;
  errorval = errorval / 4.0;
  errorval -= 2200;
  float derivative = (errorval - preverror);
  float kd_term = derivative * kd;

  float lefterrorval = 0;
  lefterrorval += sensorValues[6];
  lefterrorval += sensorValues[7];

  float righterrorval = 0;
  righterrorval += sensorValues[0];
  righterrorval += sensorValues[1];

  float middlesixsensorval = 0;
  middlesixsensorval += sensorValues[1];
  middlesixsensorval += sensorValues[2];
  middlesixsensorval += sensorValues[3];
  middlesixsensorval += sensorValues[4];
  middlesixsensorval += sensorValues[5];
  middlesixsensorval += sensorValues[6];

  leftspeed = speed - (errorval * kp + kd_term);
  rightspeed = speed + (errorval * kp + kd_term);

  // String s = "Error: " + String(errorval) + " Left Error: " + String(lefterrorval) + " Right Error: " + String(righterrorval);
  // Serial.println(s);
  // delay(500);

  if (!otherSide) {
    if (lefterrorval > 4000 && intersectionCount == 0 && !turning) {
      turning = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
    } else if ((lefterrorval > 4000 & righterrorval > 2000) && intersectionCount == 1 && !turning2 && !turning) {
      turning2 = true;
      turn2StartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
      inLoop = true;
    } else if ((abs(righterrorval - prevrighterror) < 600) && righterrorval > 4000 && intersectionCount == 2 && !turning2 && !turning3) {
      turning3 = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
      inLoop = false;
    } else if ((lefterrorval > 4000 & righterrorval > 2000) && intersectionCount == 3 && !turning2 && !turning && !turning3 && !turning4) {
      turning4 = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
    }

    //turn 2
    //turn 1
    if (turning) {
      if (millis() - turnStartTime < TURN_DURATION) {
        analogWrite(left_pwm_pin, leftspeed + 20);
        analogWrite(right_pwm_pin, 0);
      } else {
        turning = false;
        digitalWrite(LED_RF, LOW);
      }
    } else {
      if (inLoop) {
        if (leftspeed < 0) {
          digitalWrite(left_dir_pin, HIGH);
          leftspeed = abs(leftspeed);
        } else {
          digitalWrite(left_dir_pin, LOW);
        }
      }
      analogWrite(left_pwm_pin, leftspeed);
      analogWrite(right_pwm_pin, rightspeed);
    }
    if (turning2) {
      if (millis() - turn2StartTime < 600) {
        // bias right — left motor faster
        digitalWrite(left_dir_pin, HIGH);
        analogWrite(left_pwm_pin, 30);
        analogWrite(right_pwm_pin, rightspeed + 50);  // tune the +30
      } else {
        turning2 = false;
        digitalWrite(LED_RF, LOW);
        // normal PD takes over after turn
        digitalWrite(left_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
      }
    }


    //speed - 200
    // kp - 0.07
    //kd - 0.7667
    if (turning3) {
      if (millis() - turnStartTime < 400) {
        // bias right — left motor faster
        digitalWrite(left_dir_pin, HIGH);
        analogWrite(right_pwm_pin, rightspeed + 50);
        analogWrite(left_pwm_pin, 20);
        digitalWrite(76, HIGH);
      } else {
        turning3 = false;
        digitalWrite(LED_RF, LOW);
        digitalWrite(76, LOW);
        // normal PD takes over after turn
        digitalWrite(left_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
      }
    }
    //turn 4
    if (turning4) {
      if (millis() - turnStartTime < 500) {
        // bias right — left motor faster
        digitalWrite(right_dir_pin, HIGH);
        analogWrite(left_pwm_pin, leftspeed + 50);
        analogWrite(right_pwm_pin, 20);  // tune the +30
      } else {
        turning4 = false;
        digitalWrite(LED_RF, LOW);
        // normal PD takes over after turn
        digitalWrite(right_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
        donewith4 = true;
      }
    }
    preverror = errorval;
    prevrighterror = righterrorval;
    prevlefterror = lefterrorval;
    prevmiderror = middlesixsensorval;
  }


  // OTHER SIDE CODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  if (otherSide) {
    if (righterrorval > 4000 && intersectionCount == 4 && !turning5) {  //record new sensor values
      turning5 = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
    } else if ((lefterrorval > 4000 & righterrorval > 2000) && intersectionCount == 5 && !turning6 && !turning5) {
      turning6 = true;
      turn2StartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
      inLoop = true;
    } else if (lefterrorval > 4000 && intersectionCount == 6 && !turning6 && !turning7) {
      turning7 = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
      inLoop = false;
    } else if ((lefterrorval > 4000 & righterrorval > 2000) && intersectionCount == 7 && !turning6 && !turning5 && !turning7 && !turning8) {
      turning8 = true;
      turnStartTime = millis();
      intersectionCount++;
      digitalWrite(LED_RF, HIGH);
    }

    if (turning5) {
      if (millis() - turnStartTime < TURN_DURATION) {
        digitalWrite(left_dir_pin, HIGH);
        analogWrite(left_pwm_pin, 10);
        analogWrite(right_pwm_pin, rightspeed + 30);
      } else {
        turning5 = false;
        digitalWrite(LED_RF, LOW);
        digitalWrite(left_dir_pin, LOW);
      }
    } else {
      if (inLoop) {
        if (rightspeed < 0) {
          digitalWrite(right_dir_pin, HIGH);
          rightspeed = abs(rightspeed);
        } else {
          digitalWrite(right_dir_pin, LOW);
        }
      }
      analogWrite(left_pwm_pin, leftspeed);
      analogWrite(right_pwm_pin, rightspeed);
    }
    if (turning6) {
      if (millis() - turn2StartTime < 600) {
        // bias right — left motor faster
        digitalWrite(right_dir_pin, HIGH);
        analogWrite(left_pwm_pin, leftspeed + 50);
        analogWrite(right_pwm_pin, 30);  // tune the +30
      } else {
        turning6 = false;
        digitalWrite(LED_RF, LOW);
        // normal PD takes over after turn
        digitalWrite(right_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
      }
    }
    if (turning7) {
      if (millis() - turnStartTime < 400) {
        // bias right — left motor faster
        digitalWrite(right_dir_pin, HIGH);
        analogWrite(right_pwm_pin, 30);
        analogWrite(left_pwm_pin, leftspeed + 50);
        digitalWrite(76, HIGH);
      } else {
        turning7 = false;
        digitalWrite(LED_RF, LOW);
        digitalWrite(76, LOW);
        // normal PD takes over after turn
        digitalWrite(right_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
      }
    }
    if (turning8) {
      if (millis() - turnStartTime < 500) {
        // bias right — left motor faster
        digitalWrite(left_dir_pin, HIGH);
        analogWrite(left_pwm_pin, 30);
        analogWrite(right_pwm_pin, rightspeed + 50);  // tune the +30
      } else {
        turning8 = false;
        digitalWrite(LED_RF, LOW);
        // normal PD takes over after turn
        digitalWrite(left_dir_pin, LOW);
        analogWrite(left_pwm_pin, leftspeed);
        analogWrite(right_pwm_pin, rightspeed);
        donewith8 = true;
      }
    }

    preverror = errorval;
    prevrighterror = righterrorval;
    prevlefterror = lefterrorval;
    prevmiderror = middlesixsensorval;
  }

  if ((intersectionCount == 4 || intersectionCount == 8) && donewith4 && errorval == preverror && middlesixsensorval >= 14000) {
    //Serial.println("Full Black Detected");
    if (blackcounter == 0) {
      turnaround();
      analogWrite(left_pwm_pin, speed);
      analogWrite(right_pwm_pin, speed);
      blackcounter++;
      otherSide = true;
    } else if (blackcounter == 1 && donewith8) {
      digitalWrite(left_nslp_pin, LOW);
      digitalWrite(right_nslp_pin, LOW);
    }
  }
}