#include <Keyboard.h>
#include <Mouse.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM9DS1.h>
#include <SPI.h>

// i2c
Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1();
#define LSM9DS1_SCK A5
#define LSM9DS1_M6ISO 12
#define LSM9DS1_MOSI A4
#define LSM9DS1_XGCS 
#define LSM9DS1_MCS 5

void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

//data smoothing
const int numReadings = 8;

//get the data from gyroscope 
double readings_array_z[numReadings];
double readings_array_y[numReadings];
double readings_array_updown[numReadings];
double readings_array_leftright[numReadings];

int readIndex = 0;
double total_z = 0;
double total_y = 0;
double total_updown = 0;
double total_leftright = 0;
double average_z = 0;
double average_y = 0;
double average_updown = 0;
double average_leftright = 0;

//setup the botton pin
const int buttonPin = 7;
int buttonState;
int lastButtonState = LOW;

//setup the press button
const int buttonPin_press = 6;
int pressState;
int lastPressState = LOW;

//setup the mode button
const int buttonPin_mode = 8;
int modeState;
int lastModeState = LOW;
int button_num[2] = {0,0};
int button_count = 0;
int button_mode = 0;

//setup the button for car racing game
const int button_f_game = 9;
int racing_f_button;

//setup the windows key
char down = KEY_DOWN_ARROW;
char up = KEY_UP_ARROW;
char left = KEY_LEFT_ARROW;
char right = KEY_RIGHT_ARROW;     
bool alt_press = false;

//global variable for the game mode
boolean right_state = false;
boolean left_state = false;

//GUI pin mode indicater
int game_pin = 15;
int win_pin = 14;

void setup(){
  Wire.begin();
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin_press, INPUT);
  pinMode(game_pin, OUTPUT);
  pinMode(win_pin, OUTPUT);
  
  //initalize all the reading to 0
  for ( int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings_array_z[thisReading] = 0;
    readings_array_y[thisReading] = 0;
    readings_array_updown[thisReading] = 0;
    readings_array_leftright[thisReading] = 0;
  }
    
  while (!Serial) {
    delay(1); // will pause Zero, Leonardo, etc until serial console opens
  }
  
  //Serial.println("1");
  
  // Try to initialise and warn if we couldn't detect the chip
  if (!lsm.begin())
  {
    //Serial.println("Oops ... unable to initialize the LSM9DS1. Check your wiring!");
    while (1);
  }
  //Serial.println("Found LSM9DS1 9DOF");

  // helper to just set the default scaling we want, see above!
 
  setupSensor();
  Mouse.begin();
  Keyboard.begin();
}

void loop() 
{
  lsm.read();  /* ask it to read in the data */ 

  /* Get a new sensor event */ 
  sensors_event_t a, m, g, temp;
  lsm.getEvent(&a, &m, &g, &temp);  

  //subtract the last reading:
  total_z = total_z - readings_array_z[readIndex];
  total_y = total_y - readings_array_y[readIndex];
  total_updown = total_updown - readings_array_updown[readIndex];
  total_leftright = total_leftright - readings_array_leftright[readIndex];
  
  //read from the sensor:
  readings_array_z[readIndex] = g.gyro.z; //get the data from gyroscope from 
  readings_array_y[readIndex] = g.gyro.x;
  readings_array_updown[readIndex] = a.acceleration.y;
  readings_array_leftright[readIndex] = a.acceleration.x;
  
  //add the reading to the total:
  total_z = total_z + readings_array_z[readIndex];
  total_y = total_y + readings_array_y[readIndex];
  total_updown = total_updown + readings_array_updown[readIndex];
  total_leftright = total_leftright + readings_array_leftright[readIndex];
  readIndex = readIndex + 1;

  //if we are at the end of the array
  if(readIndex >= numReadings){
    readIndex = 0; //setup to the beginning
  }

  //calculate the average
  average_z = total_z / numReadings;
  average_y = total_y / numReadings;
  average_updown = total_updown / numReadings;
  average_leftright = total_leftright / numReadings;
  
  double z_arg = (average_z + 12)/10 -0.8; //deducte the offset value
  double y_arg = (average_y + 12)/10 -1.5;
  //Serial.println(z_arg);
  //Serial.println(y_arg);

  double updown_avg = (average_updown)*10;
  double leftright_avg = average_leftright*10;

  if(button_mode == 1)
  { 
    digitalWrite(win_pin,LOW);
    digitalWrite(game_pin,HIGH);
    /*
     * normal mode
    //click right and left key
    if(leftright_avg < -50)
    {
      Keyboard.press(right);
      delay(2);
      Keyboard.release(right);
      }
    if(leftright_avg > 50)
    {
      Keyboard.press(left);
      delay(2);
      Keyboard.release(left);
    }
    //click up and down key
    if(updown_avg < -50)
    {
      Keyboard.press(down);
      delay(2);
      Keyboard.release(down);
    }
    if(updown_avg > 50)
    {
      Keyboard.press(up);
      delay(2);
      Keyboard.release(up);
    */
    /*
     * game mode for car racing game
     */

     /*
      * test leftright_avg
      * leftright -30 -45
      * 25 left 
      * -25 right
      */
    Serial.println(updown_avg);
    /*
     * for speical operation function for car driving
     */
    //click right and left key
    if(leftright_avg < -50)
    {
      Keyboard.press(up);
      delay(2);
      Keyboard.release(up);
      }
    if(leftright_avg > 50)
    {
      Keyboard.press(down);
      delay(2);
      Keyboard.release(down);
    }
    //click up and down key
    if(updown_avg < -20 || (leftright_avg < -30 && leftright_avg > -45 && updown_avg < -25)) // right turn
    { 
      right_state = true;
      if(right_state && left_state)
      {
        delay(2);
        Keyboard.release(left);
        left_state = false;
      }
      Keyboard.press(right);
    }

    if(updown_avg > -20 && updown_avg < 20)
    {
      Keyboard.releaseAll();
    }
    
    if(updown_avg > 20 || (leftright_avg < -30 && leftright_avg > -45 && updown_avg > 25) ) // left turn
    {
      left_state = true;
      if(right_state && left_state)
      {
        delay(2);
        Keyboard.release(right);
        right_state = false;
        
      }
      Keyboard.press(left);
    }
    racing_f_button = digitalRead(button_f_game);
    if(racing_f_button == HIGH)
    {
      Keyboard.press('f');
      delay(100);
      Keyboard.releaseAll();
    }     
  }

/*
 * check app function win + tab function
 */
 if(button_mode == 2)
 {
   digitalWrite(win_pin,HIGH);
   digitalWrite(game_pin,LOW);
   if(leftright_avg < -90)
  {
    if(!alt_press)                                                                                             
    {
      Keyboard.press(KEY_LEFT_ALT);  
      alt_press = true;
    }
    Keyboard.press(KEY_TAB);
    delay(500);
    Keyboard.release(KEY_TAB);
    delay(500);
  }
  if(leftright_avg > -90)
  {
    if(alt_press) Keyboard.release(KEY_LEFT_ALT);
    alt_press = false;
  }

  if(leftright_avg > 80)
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_ESC);  
    delay(500);
    
    Keyboard.releaseAll();
  }   
 }

 if(button_mode == 0) //On Screen Keyboard showup
 {
  digitalWrite(win_pin,LOW);
  digitalWrite(game_pin,LOW);
  if( updown_avg > 60 && updown_avg < 90) 
  {
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press('i');
    delay(500);
    
    Keyboard.releaseAll();
  }
  if( updown_avg < -70) // close function
  {
    Keyboard.press(KEY_LEFT_ALT);
    delay(100);
    Keyboard.press(KEY_F4);
    Keyboard.releaseAll();
    delay(1000); 
  }
 }

 /*
  * cursor move function
  */
 if(digitalRead(buttonPin))
 {
    Keyboard.releaseAll();
    double z_mouse = z_arg*8;
    double y_mouse = -y_arg*6;
    Mouse.move(z_mouse,y_mouse);
 }

 /*
  * click and drag function
  */

  pressState = digitalRead(buttonPin_press);
  if(pressState == HIGH)
  {
    Keyboard.releaseAll();
    Mouse.press();
    //Serial.println("hold");
    delay(20);
    double z_mouse = z_arg*8;
    double y_mouse = -y_arg*6;
    Mouse.move(z_mouse,y_mouse);
  }
  if(pressState == LOW)
  {
    Mouse.release();
  }
  
  /*
   * set the button mode for differnt gui 
   * 0 means nothing
   * 1 move cursor
   * 2 control up down left right
   * 3 special function like win + tab
   */
  modeState = digitalRead(buttonPin_mode);
  if(modeState == HIGH)
  {
    //button_num status 
    button_num[0] = button_num[1];
    button_num[1] = 1;
  }
  if(modeState == LOW)
  {
    button_num[0] = button_num[1];
    button_num[1] = 0;
  }

  //calculate the number of press
  if(button_num[0] + button_num[1] == 1) button_count++;

  if(int(button_count/2) == 3)
  {
    button_mode = 0;
    button_count = 0;
  }
  else
  {
    button_mode = int(button_count/2);
  }
  
  //Serial.println(button_mode);
  
  delay(10);
}
