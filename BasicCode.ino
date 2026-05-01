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

const int LED_RF = 41;

const int speed = 50;

void moveforward(){
  analogWrite(left_pwm_pin,speed);
  analogWrite(right_pwm_pin,speed);
}

void moveright(){
  analogWrite(right_pwm_pin, 0);
  analogWrite(left_pwm_pin, speed);
}

void moveleft(){
  analogWrite(left_pwm_pin, 0);
  analogWrite(right_pwm_pin,speed);
}

void adjusterror(double errorvalue){
  //we need to check amnitude to decide how long we move right/left
  if(errorvalue > 0){
    moveleft();
  }
  else {
    moveright();
  }
}

///////////////////////////////////
void setup() {
// put your setup code here, to run once:
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);

  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);

  pinMode(LED_RF, OUTPUT);
  
  ECE3_Init();

// set the data rate in bits/second for serial data transmission
  Serial.begin(9600); 
  delay(2000); //Wait 2 seconds before starting 
  
}

void loop() {
  // put your main code here, to run repeatedly: 

  ECE3_read_IR(sensorValues);
  double errorval = 0;
  errorval+=sensorValues[0]*-8;
  errorval+=sensorValues[1]*-4;
  errorval+=sensorValues[2]*-2;
  errorval+=sensorValues[3]*-1;
  errorval+=sensorValues[4]*1;
  errorval+=sensorValues[5]*2;
  errorval+=sensorValues[6]*-4;
  errorval+=sensorValues[7]*8;
  errorval = errorval/4.0;

  adjusterror(errorval);

  //if erroval is positive we go x way / if neg we go y
  //magnitude says by how much

  //analogWrite(left_pwm_pin,leftSpd);

// 
  
//  ECE3_read_IR(sensorValues);

//  digitalWrite(LED_RF, HIGH);
//  delay(250);
//  digitalWrite(LED_RF, LOW);
//  delay(250);
    
  }
