#include <HCSR04.h>
#include <SPI.h>
#include <math.h>
#include <Wire.h>

//setup first ultrasonic sensorleft
UltraSonicDistanceSensor distanceSensor_left(13, 12);  // Initialize sensor that uses digital pins 13 and 12.
// trigger 13 echo 12

/*
* read distance function
* step the maxium ultrasonic value to 30 to avoid data distorsion
*/
int distance_left; // setup global variable for left ultrasonic sensor

double f_distance(double currentvalue)
{
  return (currentvalue > 30) ? 40 : currentvalue;
}

void setup () {
    Serial.begin(9600);  // We initialize serial connection so that we could print values from sensor.
}

void loop () {
    //temp variable to confirm the control mode, the ultrasonic function can only be activated in control mode
    int dist_temp_left;
    distance_left = int(f_distance(distanceSensor_left.measureDistanceCm()));
    // Every 500 miliseconds, do a measurement using the sensor and print the distance in centimeters.
    Serial.println(distance_left);

    if(distance_left >= 10 && distance_left <= 15)
    {
      delay(500);
      dist_temp_left = int(f_distance(distanceSensor_left.measureDistanceCm()));
      if(fabs(dist_temp_left - distance_left) <= 2)
      {
        Serial.println("control mode");
        
        while(f_distance(distanceSensor_left.measureDistanceCm()) < 35)
        {
          distance_left = int(f_distance(distanceSensor_left.measureDistanceCm()));
          if(distance_left < 7)
          {
            Serial.println("Volumedown");
            delay(200);
          }
          if(distance_left > 17)
          {
            Serial.println("Volumeup");
            delay(200); 
          }
        }
      }
    }
    
    delay(300);
}
