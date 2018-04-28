#include <Wire.h>
#include <SPI.h>
#include <HCSR04.h>

UltraSonicDistanceSensor distanceSensor(13, 12);  // Initialize sensor that uses digital pins 13 and 12.
//use the distanceSensor.measureDistanceCm() function to get the distance value

//data smoothing setup
const int numReadings = 30;
double readings_array[numReadings];
int readIndex = 0;
double total_distance = 0;
double average_distance = 0;

void setup () {
    Serial.begin(9600);  // We initialize serial connection so that we could print values from sensor.
}

void loop () {
    //set up for data smoothing
    total_distance = total_distance - readings_array[readIndex]; //substract the last reading value
    readings_array[readIndex] = distanceSensor.measureDistanceCm();

    //add the reading to the total
    total_distance = total_distance + readings_array[readIndex];
    readIndex++;

    //at the end of the array
    if(readIndex >= numReadings) readIndex = 0;

    //calculate the average
    average_distance = total_distance/numReadings;
    Serial.println(average_distance);
     
    delay(0.5);
}
