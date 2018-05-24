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
// You can also use software SPI
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_SCK, LSM9DS1_MISO, LSM9DS1_MOSI, LSM9DS1_XGCS, LSM9DS1_MCS);
// Or hardware SPI! In this case, only CS pins are passed in
//Adafruit_LSM9DS1 lsm = Adafruit_LSM9DS1(LSM9DS1_XGCS, LSM9DS1_MCS);

void setupSensor()
{
  // 1.) Set the accelerometer range
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity
  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);

  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
}

/*
 * number of readings for data smoothing
 */
const int numReadings = 8;

/*
 * define the structure and typedef struct
 * init the array
 */
struct TRANSFER_DATA_STRUCTURE
{
  bool button1;
  bool button2;
  bool releaseAllsent;
  bool left;
  bool right;
  bool up;
  bool down;
  double mouse_x;
  double mouse_y;
};

struct MOUSE_DATA_STRUCTURE
{
  double reading_gyro_z;
  double reading_gyro_y;
  double reading_acce_y; //updown
  double reading_acce_x; //left and right
};
struct MOUSE_DATA_STRUCTURE mouse_data_array[numReadings]; //define data array
/* struct define and array */


/*
 * define the MOUSE_DATA_Structure functions 
 * add, sub, divfunction
*/

void f_struct_sub(struct MOUSE_DATA_STRUCTURE* sFirst,struct MOUSE_DATA_STRUCTURE* sSecond)
{
  sFirst->reading_gyro_z -= sSecond->reading_gyro_z;
  sFirst->reading_gyro_y -= sSecond->reading_gyro_y;
  sFirst->reading_acce_y -= sSecond->reading_acce_y;
  sFirst->reading_acce_x -= sSecond->reading_acce_x; 
}

void f_struct_add(struct MOUSE_DATA_STRUCTURE* sFirst,struct MOUSE_DATA_STRUCTURE* sSecond)
{
  sFirst->reading_gyro_z += sSecond->reading_gyro_z;
  sFirst->reading_gyro_y += sSecond->reading_gyro_y;
  sFirst->reading_acce_y += sSecond->reading_acce_y;
  sFirst->reading_acce_x += sSecond->reading_acce_x; 
}

void f_struct_div(struct MOUSE_DATA_STRUCTURE* sFirst , struct MOUSE_DATA_STRUCTURE* sSecond,int num)
{
  sSecond->reading_gyro_z = sFirst->reading_gyro_z / num;
  sSecond->reading_gyro_y = sFirst->reading_gyro_y / num;
  sSecond->reading_acce_y = sFirst->reading_acce_y / num;
  sSecond->reading_acce_x = sFirst->reading_acce_x / num; 
}

/* end */

/*
 * data smooth second part
 */
int readIndex = 0;

/*
 * total mouse data init and average mouse data init
 */
struct MOUSE_DATA_STRUCTURE total;
struct MOUSE_DATA_STRUCTURE average;
/*end*/

//setup the botton pin
const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;

//setup the press button
const int buttonPin_press = 8;
int pressState;
int lastPressState = LOW;

//setup the windows key
char down = KEY_DOWN_ARROW;
char up = KEY_UP_ARROW;
char left = KEY_LEFT_ARROW;
char right = KEY_RIGHT_ARROW;

void setup(){
  Wire.begin();
  Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  pinMode(buttonPin_press, INPUT);

  /*
   * initalize all the reading to 0 with struct array
   */
  for(int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    mouse_data_array[numReadings].reading_gyro_z = 0;
    mouse_data_array[numReadings].reading_gyro_y = 0;
    mouse_data_array[numReadings].reading_acce_y = 0;
    mouse_data_array[numReadings].reading_acce_x = 0;
  }
  /* end */

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

  /* with new struct data */
  // subtract the last readings:
  Serial.println(total.reading_gyro_z);
  
  f_struct_sub(&total, mouse_data_array + readIndex); 
  
  // reading from the sensor:
  mouse_data_array[readIndex].reading_gyro_z = g.gyro.z;
  mouse_data_array[readIndex].reading_gyro_y = g.gyro.x;
  mouse_data_array[readIndex].reading_acce_y = a.acceleration.y;
  mouse_data_array[readIndex].reading_acce_x = a.acceleration.x;
  
  //add the reading to the total
  f_struct_add(&total,mouse_data_array + readIndex);
  readIndex++;
  /* end */

  //if we are at the end of the array
  if(readIndex >= numReadings){
    readIndex = 0; //setup to the beginning
  }
  //average function
  f_struct_div(&total,&average, numReadings);
  /* end */
  /*
   *  deducte the offseet value
   */
  double z_arg = (average.reading_gyro_z + 12)/10 - 0.8;
  Serial.println(z_arg);
  double y_arg = (average.reading_gyro_y + 12)/10 - 1.5;
  Serial.println(y_arg);
  double updown_avg = (average.reading_acce_y)*10;
  double leftright_avg = (average.reading_acce_x)*10;

  //click right and left key
  if(leftright_avg < -50)
  {
    Keyboard.press(right);
    delay(50);
    Keyboard.release(right);
  }
  if(leftright_avg > 50)
  {
    Keyboard.press(left);
    delay(50);
    Keyboard.release(left);
  }
     
  //click up and down key
  if(updown_avg < -50)
  {
    Keyboard.press(down);
    delay(50);
    Keyboard.release(down);
  }

  if(updown_avg > 50)
  {
    Keyboard.press(up);
    delay(50);
    Keyboard.release(up);
  }

  /*
   * press button function
  */
  pressState = digitalRead(buttonPin_press);
  if(pressState == HIGH)
  {
    Keyboard.releaseAll();
    Mouse.press();
    Serial.println("hold");
    delay(50);
    double z_mouse = z_arg*8;
    double y_mouse = -y_arg*6;
    Mouse.move(z_mouse,y_mouse);
  }
  if(pressState == LOW)
  {
    Mouse.release();
  }

  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH)
  {
    Keyboard.releaseAll();
    //Serial.println("pressed");
    double z_mouse = z_arg*8;
    double y_mouse = -y_arg*6;
    Mouse.move(z_mouse,y_mouse);
  }
  delay(10);
}

