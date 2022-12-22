#include <Arduino.h>

#include <BinBuffer.hpp>

#include <SPI.h>
#include <SD.h>

File myFile;
const int chipSelect = BUILTIN_SDCARD;

// Created a buffer with 4 segments with 512 bytes each
BinBuffer buffer(4,512);

uint16_t code;
uint32_t timestamp;
uint32_t report_interval = 1000;
char report[80];

void setup() {
  //SD card
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");
  char filename[19];
  sprintf(filename, "data.dat", rndval);

  Serial.print("Opening file ");
  Serial.println(filename);
  myFile = SD.open(filename, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (!myFile) {
    // if the file didn't open, print an error:
    Serial.print("error opening file: ");
    Serial.println(myFile);
    while (1);
  }
}

void loop() {
  if(timestamp - millis() > report_interval) {
    timestamp = millis();
    // Pass a variable by reference
    buffer.append(timestamp);
    sprintf(report, "We write something into a char array: %lu", micros());
    // Pass a pointer and a length
    buffer.append(report, strlen(report));
  }

  // Once a segment is full, it can be written and will be freed automatically
  if(buffer.is_ready()) { // Loop over this if writing all buffer is highest priority, else yield to rest of loop after one segment.
    const unsigned char* sd_arr = const_cast<const unsigned char*>(buffer.pop_ready_segment());
    myFile.write(sd_arr, buffer.seg_size);
    myFile.flush();
  }
}
