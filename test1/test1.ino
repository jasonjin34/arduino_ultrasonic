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
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_2G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_4G);
  //lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_8G);
  lsm.setupAccel(lsm.LSM9DS1_ACCELRANGE_16G);
  
  // 2.) Set the magnetometer sensitivity

  lsm.setupMag(lsm.LSM9DS1_MAGGAIN_4GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_8GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_12GAUSS);
  //lsm.setupMag(lsm.LSM9DS1_MAGGAIN_16GAUSS);
  // 3.) Setup the gyroscope
  lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_245DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_500DPS);
  //lsm.setupGyro(lsm.LSM9DS1_GYROSCALE_2000DPS);
}

//data smoothing
const int numReadings = 8;

//get the data from gyroscope 
double readings_array_z[numReadings];
double readings_array_y[numReadings];
double readings_array_click[numReadings];
int readIndex = 0;
double total_z = 0;
double total_y = 0;
double total_click = 0;
double average_z = 0;
double average_y = 0;
double average_click = 0;

//setup the botton pin
const int buttonPin = 9;
int buttonState;
int lastButtonState = LOW;

void setup(){
  Wire.begin();
  //Serial.begin(115200);
  pinMode(buttonPin, INPUT);
  //initalize all the reading to 0
  for ( int thisReading = 0; thisReading < numReadings; thisReading++)
  {
    readings_array_z[thisReading] = 0;
    readings_array_y[thisReading] = 0;
    readings_array_click[thisReading] = 0;
  }
  /*
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
  */
  setupSensor();
  Mouse.begin();
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
  total_click = total_click - readings_array_click[readIndex];
  
  //read from the sensor:
  readings_array_z[readIndex] = g.gyro.z; //get the data from gyroscope from 
  readings_array_y[readIndex] = g.gyro.x;
  readings_array_click[readIndex] = a.acceleration.y;
  
  //add the reading to the total:
  total_z = total_z + readings_array_z[readIndex];
  total_y = total_y + readings_array_y[readIndex];
  total_click = total_click + readings_array_click[readIndex];
  readIndex = readIndex + 1;

  //if we are at the end of the array
  if(readIndex >= numReadings){
    readIndex = 0; //setup to the beginning
  }

  //calculate the average
  average_z = total_z / numReadings;
  average_y = total_y / numReadings;
  average_click = total_click / numReadings;
  
  double z_arg = (average_z + 12)/10 -0.8;
  double y_arg = (average_y + 12)/10 -1.5;
  double click_avg = (average_click)*10;

  //Serial.println(click_avg);

  if(click_avg < -65)
  {
    Mouse.click();
    delay(1500);
  }


  //Serial.println(y_arg);  
  //left and right offset distance is 0.8
  //set up the mouse move distance


  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH)
  {
    //Serial.println("pressed");
    double z_mouse = z_arg*8;
    double y_mouse = -y_arg*6;
    Mouse.move(z_mouse,y_mouse);
  }
  
  /*
  Serial.print("Accel X: "); Serial.print(a.acceleration.x); Serial.print(" m/s^2");
  Serial.print("\tY: "); Serial.print(a.acceleration.y);     Serial.print(" m/s^2 ");
  Serial.print("\tZ: "); Serial.print(a.acceleration.z);     Serial.println(" m/s^2 ");

  Serial.print("Mag X: "); Serial.print(m.magnetic.x);   Serial.print(" gauss");
  Serial.print("\tY: "); Serial.print(m.magnetic.y);     Serial.print(" gauss");
  Serial.print("\tZ: "); Serial.print(m.magnetic.z);     Serial.println(" gauss");

  Serial.print("Gyro X: "); Serial.print(g.gyro.x);   Serial.print(" dps");
  Serial.print("\tY: "); Serial.print(g.gyro.y);      Serial.print(" dps");
  Serial.print("\tZ: "); Serial.print(g.gyro.z);      Serial.println(" dps");
  */
  delay(40);
}