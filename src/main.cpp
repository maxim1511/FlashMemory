// This is a test program to write/read files onto the flash
#include "Arduino.h"
#include <SPI.h>
#include <SerialFlash.h>

// Currently using VSPI
#define CS      4
#define CLK     18
#define MISO    19
#define MOSI    23

#define MAXFILESIZE 128

// Function to write array data to the flash
void writeToFlash(SerialFlashFile files[], char filenames[10][10], int16_t batterySOCs[], int16_t batteryVoltages[], int16_t batteryCurrents[]){

    
    // Open files
    for(int i = 0; i < 10; i++){
        if(SerialFlash.create(filenames[i], MAXFILESIZE)){
            files[i] = SerialFlash.open(filenames[i]);
            Serial.println("File" + (String)filenames[i] + " created successfully!");
        }else{
            Serial.println("File could not be created!");
        }
    }

    // Write to the files
    String message;
    char buffer[MAXFILESIZE];
    for(int i = 0; i < 10; i++){
        message = (String)batterySOCs[i] + "," + (String)batteryVoltages[i] + "," + (String)batteryCurrents[i];
        //files[i].write(buffer, entrySize);
        message.toCharArray(buffer, MAXFILESIZE);
        files[i].write(buffer, MAXFILESIZE);
    }

    //Close files
    for(int i = 0; i < 10; i++){
        files[i].close();
    }
}

// Function to read array data from the flash
void readFromFlash(char filenames[10][10]){
    uint32_t filesize;
    char filename[64];
    char buffer[MAXFILESIZE];
    SerialFlashFile dummy;
    SerialFlash.opendir();
    int counter = 0;

    // Read all files that are stored in the flash memory
    while(SerialFlash.readdir(filename, sizeof(filename), filesize)){
        dummy = SerialFlash.open(filename);
        dummy.read(buffer, MAXFILESIZE);
        Serial.println("File " + (String)filename + ":");
        Serial.println(buffer);
        counter++;
    }
    Serial.println("Number of pages read: " + (String)counter);
}

// Function to delete all the data from the flash
void eraseAllData(){
    SerialFlash.eraseAll();

    // Wait till it is all erased
    while(!SerialFlash.ready()){
        Serial.println("Erasing Chip...");
        delay(1000);
    }
    Serial.println("Flash erased succesfully!");
}

// Main function
void setup(){

  Serial.begin(9600);
  Serial.println();
  //Set up SPI
  //SPI.begin(CLK, MISO, MOSI, CS);

// Start the SPI communication
 if (!SerialFlash.begin(CS)) {
    while(true) {
      Serial.println("Unable to access SPI Flash chip");
      delay(1000);
    }
  }else{
    Serial.println("Successfully connected to SPI Flash chip");
  }

  //Sample Files and values to test the writing and reading
    char filenames[10][10] =   {"file0.bin",
                                "file1.bin",
                                "file2.bin", 
                                "file3.bin",
                                "file4.bin",
                                "file5.bin",
                                "file6.bin", 
                                "file7.bin",
                                "file8.bin",
                                "file9.bin"};
    
    SerialFlashFile file0;
    SerialFlashFile file1;
    SerialFlashFile file2;
    SerialFlashFile file3;
    SerialFlashFile file4;
    SerialFlashFile file5;
    SerialFlashFile file6;
    SerialFlashFile file7;
    SerialFlashFile file8;
    SerialFlashFile file9;
    SerialFlashFile files[] = {file0, file1, file2, file3, file4, file5, file6, file7, file8, file9};
    int16_t batterySOCs[] =     {12,  55,  0,  64,  71,  23,  49,  99, 100, 86};
    int16_t batteryVoltages[] = {101, 110, 99, 105, 105, 102, 105, 99, 100, 103};
    int16_t batteryCurrents[] = {55,  -2,  19, 0,   150, 77,  56,  20, 99,  103};
    
    // Call functions
    writeToFlash(files, filenames, batterySOCs, batteryVoltages, batteryCurrents);
    delay(2000);
    Serial.println();
    readFromFlash(filenames);
    eraseAllData();
    //readFromFlash(filenames);
    
    Serial.end();
}

// Does nothing
void loop(){
}