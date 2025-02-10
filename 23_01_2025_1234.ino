// C++ code
//
#include <Servo.h>
#include <SoftwareSerial.h>
#include <FlexyStepper.h>
#include <FastLED.h>
#define UPPER_SERVO_PIN 9
#define LOWER_SERVO_PIN 10
#define BUTTON_1_PIN A0
#define BUTTON_2_PIN A1
#define BUTTON_3_PIN A2
#define BUTTON_4_PIN A3
#define LED_PIN 13
#define COUNT_OF_LEDS 10
SoftwareSerial bt_serial(11, 12);

//Servo upper_servo;
Servo lower_servo;

FlexyStepper motor_1;
FlexyStepper motor_2;
FlexyStepper motor_3;
CRGB leds[COUNT_OF_LEDS];
const float drinks_recipes[4][3] = {{0, 0, 0}, {0, 150, 50}, {100, 50, 50}, {100, 0, 100}};
//const int angles[] = {30, 60, 100, 130};
const float mls_in_one_revolution = 50;
int receipts_numbers[] = {0, 0, 0, 0};
int lower_servo_position;
int give_out_position;
int pour_position;
int starting_position;
int orders;
int current_cup;
int angle_between_cups;
void set_led_color(int color) {
  for (int i = 0; i < COUNT_OF_LEDS; ++i) {
    if (color == 1) {
      leds[i] = CRGB::Red;
    }
    else if (color == 2) {
      leds[i] = CRGB::Orange;
    }
    else if (color == 3) {
      leds[i] = CRGB::Green;
    }
  }
  FastLED.show();
}
//void move_upper_servo(int angle) {
//  upper_servo.write(angle);
//}

void move_lower_servo(int angle) {
  lower_servo.write(angle);
}
bool get_button_state(int button_number) {
  if (button_number == 0) {
    return digitalRead(BUTTON_1_PIN);
  }
  else if (button_number == 1) {
    return digitalRead(BUTTON_2_PIN);
  }
  else if (button_number == 2) {
    return digitalRead(BUTTON_3_PIN);
  }
  else if (button_number == 3) {
    return digitalRead(BUTTON_4_PIN);
  }
  return false;
}
void motor_control(int motor_number, long steps) {
  bool can_move = true;
  long remained_steps;
  can_move = get_button_state(motor_number);
  if (motor_number == 0) {
    motor_1.setTargetPositionInSteps(steps);
    while (!motor_1.motionComplete() && can_move){
      motor_1.process_movement();
      can_move = get_button_state(motor_number);
    }
    if (can_move == false){
      remained_steps = steps - motor_1.getCurrentPositionInSteps();
      while (!can_move){
        can_move = get_button_state(motor_number);
      }
      motor_control(motor_number, remained_steps);
    }
  }
  else if (motor_number == 1) {
    motor_2.setTargetPositionInSteps(steps);
    while (!motor_2.motionComplete() && can_move){
      motor_2.process_movement();
      can_move = get_button_state(motor_number);
    }
    if (can_move == false){
      remained_steps = steps - motor_2.getCurrentPositionInSteps();
      while (!can_move){
        can_move = get_button_state(motor_number);
      }
      motor_control(motor_number, remained_steps);
    }
    
  }
  else if (motor_number == 2) {
    motor_3.setTargetPositionInSteps(steps);
    while (!motor_3.motionComplete() && can_move){
      motor_3.process_movement();
      can_move = get_button_state(motor_number);
    }
    if (can_move == false){
      remained_steps = steps - motor_3.getCurrentPositionInSteps();
      while (!can_move){
        can_move = get_button_state(motor_number);
      }
      motor_control(motor_number, remained_steps);
    }
    
  }
}

void pour(int receipt_number, int drink_number) {
  if (get_button_state(drink_number)) {
    Serial.print("pour to glass ");
    Serial.println(drink_number);
    set_led_color(2);
    lower_servo_position = pour_position + (drink_number * angle_between_cups);
    move_lower_servo(lower_servo_position);
    for (int i = 0; i < 3; ++i) {
      float steps = drinks_recipes[receipt_number][i] / mls_in_200_steps;
      motor_control(i, steps);
    }
    set_led_color(1);
  }
  else {
    Serial.print("No glass to pour! ");
    Serial.println(drink_number);
  }
}

void give_out(int drink_number) {
  bool not_empty = true;
  Serial.print("give out drink ");
  Serial.println(drink_number);
  lower_servo_position = give_out_position + (drink_number * angle_between_cups);
  move_lower_servo(lower_servo_position);
  set_led_color(3);
  while (not_empty) {
    not_empty = get_button_state(drink_number);
    delay(25);
  }
  set_led_color(1);
  delay(500);
}

bool get_info() {
  int info;
  if (bt_serial.available()) {
    info = bt_serial.parseInt();
    if (info >= 0 and info <= 9) {
      receipts_numbers[3] = info % 10;
      receipts_numbers[2] = 0;
      receipts_numbers[1] = 0;
      receipts_numbers[0] = 0;
      orders = 1;
    }
    if (info >= 10 and info <= 99) {
      receipts_numbers[3] = info % 10;
      receipts_numbers[2] = (info % 100) / 10;
      receipts_numbers[1] = 0;
      receipts_numbers[0] = 0;
      orders = 2;
    }
    if (info >= 100 and info <= 999) {
      receipts_numbers[3] = info % 10;
      receipts_numbers[2] = (info % 100) / 10;
      receipts_numbers[1] = (info % 1000) / 100;
      receipts_numbers[0] = 0;
      orders = 3;
    }
    if (info >= 1000 and info <= 9999) {
      receipts_numbers[3] = info % 10;
      receipts_numbers[2] = (info % 100) / 10;
      receipts_numbers[1] = (info % 1000) / 100;
      receipts_numbers[0] = info / 1000;
      orders = 4;
    }
    return true;
  }
  else {
    return false;
  }
}
void send_info(String info) {
  bt_serial.println(info);
}
void setup()
{
  lower_servo_position = 0;
  give_out_position = 210;
  pour_position = 180;
  angle_between_cups = 40;
  pinMode(BUTTON_1_PIN, INPUT);
  pinMode(BUTTON_2_PIN, INPUT);
  pinMode(BUTTON_3_PIN, INPUT);
  pinMode(BUTTON_4_PIN, INPUT);
  //upper_servo.attach(UPPER_SERVO_PIN);
  lower_servo.attach(LOWER_SERVO_PIN);
  motor_1.connectToPins(5, 2);
  motor_2.connectToPins(6, 3);
  motor_3.connectToPins(7, 4);

  motor_1.setStepsPerRevolution(200);
  motor_1.setSpeedInStepsPerSecond(200);
  motor_1.setAccelerationInStepsPerSecondPerSecond(60);

  motor_2.setStepsPerRevolution(200);
  motor_2.setSpeedInStepsPerSecond(200);
  motor_2.setAccelerationInStepsPerSecondPerSecond(60);

  motor_3.setStepsPerRevolution(200);
  motor_3.setSpeedInStepsPerSecond(200);
  motor_3.setAccelerationInStepsPerSecondPerSecond(60);

  FastLED.addLeds<WS2812, LED_PIN>(leds, COUNT_OF_LEDS);

  Serial.begin(9600);
  bt_serial.begin(9600);
  //move_upper_servo(0);
  move_lower_servo(0);
}

void loop()
{
  bool get_info_to_work = false;
  while (get_info_to_work == false) {
    get_info_to_work = get_info();
    delay(25);
  }
  current_cup = 3;
  for (int i = 0; i < orders; ++i){
    pour(receipts_numbers[current_cup], current_cup);
    give_out(current_cup);
    current_cup--;
  }
  lower_servo_position = 0;
  move_lower_servo(lower_servo_position);
}
