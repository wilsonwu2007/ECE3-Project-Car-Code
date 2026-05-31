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

uint16_t sensorValues[8];  // right -> left, 0 -> 7

const int left_nslp_pin = 31;  // nslp ==> awake & ready for PWM
const int left_dir_pin = 29;
const int left_pwm_pin = 40;

const int right_nslp_pin = 11;  // nslp ==> awake & ready for PWM
const int right_dir_pin = 30;
const int right_pwm_pin = 39;

const int LED_RF = 41;

const float speed = 170;  //150
float rightspeed;
float leftspeed;

const float kp = 0.011;  // 0.01
float preverror = 0;
const float kd = 0.15;  //0.07
float prevedgeerror = 0;

int blackcounter = 0;

// void moveforward(){
//   analogWrite(left_pwm_pin,speed);
//   analogWrite(right_pwm_pin,speed);
// }

// void moveright(){
//   analogWrite(right_pwm_pin, 0);
//   analogWrite(left_pwm_pin, speed);
// }

void turnaround() {
  //what do i write here
  // Spin in place: left motor forward, right motor backward
  digitalWrite(left_dir_pin, HIGH);  // Left motor forward
  digitalWrite(right_dir_pin, LOW);  // Right motor backward (or vice versa)

  analogWrite(left_pwm_pin, 100);
  analogWrite(right_pwm_pin, 100);

  delay(550);  // Hold until ~180Â° rotation is complete

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

  //errorval-=1000;
  float derivative = (errorval - preverror);
  float kd_term = derivative * kd;

  leftspeed = speed - (errorval * kp + kd_term);
  rightspeed = speed + (errorval * kp + kd_term);

  if (leftspeed > 255) leftspeed = speed;
  if (leftspeed < 0) leftspeed = 0;
  if (rightspeed > 255) rightspeed = speed;
  if (rightspeed < 0) rightspeed = 0;

  float edgeerror = 0;
  edgeerror += sensorValues[0] * 10;
  edgeerror += sensorValues[1] * 10;
  edgeerror += sensorValues[6] * 10;
  edgeerror += sensorValues[7] * 10;

  //  Serial.print("errorval ");
  //  Serial.println(errorval);
  //  delay(500);

  //  Serial.print("edgeerror ");
  //  Serial.println(edgeerror);
  //  delay(500);

  if (prevedgeerror == edgeerror && edgeerror >= 90000) {
    if (blackcounter == 0) {
      turnaround();
      blackcounter++;
      analogWrite(left_pwm_pin, speed);
      analogWrite(right_pwm_pin, speed);
      delay(200);
    } else if (blackcounter == 1) {
      digitalWrite(left_nslp_pin, LOW);
      digitalWrite(right_nslp_pin, LOW);
    }
  }

  analogWrite(left_pwm_pin, leftspeed);
  analogWrite(right_pwm_pin, rightspeed);

  preverror = errorval;
  prevedgeerror = edgeerror;
}