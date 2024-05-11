// This is a test program to write/read pages onto the W25M128JV flash memory. The flash has enough space
// for 65536 pages of 256 bytes each, so 16MB in total.
#include "Arduino.h"
#include <SPI.h>
#include <SerialFlash.h>

// Currently using VSPI
#define CS      4
#define CLK     18
#define MISO    19
#define MOSI    23

#define MAXPAGESIZE 256
#define MAXPAGENUMBER 500
#define MAXNAMELENGTH 14

uint64_t sampleData = 0;
uint64_t startTime = 0;
uint16_t directoryIndex = 0;
bool firstRun = true;

// Function to write data from example array to the flash
void writeToFlashTest(SerialFlashFile pages[], char pagenames[10][10], int16_t batterySOCs[], int16_t batteryVoltages[], int16_t batteryCurrents[]){

    // Open pages
    for(int i = 0; i < 10; i++){
        if(SerialFlash.create(pagenames[i], MAXPAGESIZE)){
            pages[i] = SerialFlash.open(pagenames[i]);
            Serial.println("page: " + (String)pagenames[i] + " created successfully!");
        }else{
            Serial.println("page could not be created!");
        }
    }

    // Write to the pages
    String message;
    char buffer[MAXPAGESIZE];
    for(int i = 0; i < 10; i++){
        message = (String)batterySOCs[i] + "," + (String)batteryVoltages[i] + "," + (String)batteryCurrents[i];
        //pages[i].write(buffer, entrySize);
        message.toCharArray(buffer, MAXPAGESIZE);
        pages[i].write(buffer, MAXPAGESIZE);
    }

    //Close pages
    for(int i = 0; i < 10; i++){
        pages[i].close();
    }
}

// Function to use the flash as a ringbuffer. Pages get written in chronological order till the flash is
// filled, then it will start at the beginning of the flash again by overwriting the oldest page
// pages will be named with "page" + index of the page (0-65535) + ".bin", max pagename size is thus 14 bytes
// When a page is written and there exists a page with the following index, that following page is erased,
// so there is always an empty index in the buffer which can in case of a restart be used to find the current
// position in the ring buffer.
void writeAsRingBuffer(){
    uint32_t pagesize;
    char pagename[MAXNAMELENGTH];
    String str;
    char buffer[MAXPAGESIZE];
    SerialFlashFile currentPage;
    /*
    if(directoryIndex == 499){
        return;
    }
    */
    
        // Find the current index in the ring buffer
        while(SerialFlash.readdir(pagename, sizeof(pagename), pagesize)){
            directoryIndex++;
            if(directoryIndex == MAXPAGENUMBER){
                directoryIndex = 0;
            }
        }
        
        // check if end of buffer is reached
        if(directoryIndex == MAXPAGENUMBER-1){
            directoryIndex = 0;
        
        // check if buffer is empty
        }else if(!SerialFlash.exists("page0.bin") && !SerialFlash.exists("page65535.bin")){
            directoryIndex = 0;

        // check if it is the first run but buffer is not empty
        }else if(firstRun){
            directoryIndex = directoryIndex;

        // Any normal case
        }else{
            directoryIndex++;
        }
    // Delete page with following index if it exists. Delete first so there is always at least one free 
    // index in the system
    str = "page" + (String)directoryIndex+1 + ".bin";
    str.toCharArray(pagename, MAXNAMELENGTH);
    if(SerialFlash.exists(pagename))
        SerialFlash.remove(pagename);
    while(!SerialFlash.ready()){

    }
    // Create and open a new page
    str = "page" + (String)directoryIndex + ".bin";
    str.toCharArray(pagename, MAXNAMELENGTH);
    if(SerialFlash.create(pagename, MAXPAGESIZE)){
        currentPage = SerialFlash.open(pagename);
        Serial.println("page: " + (String)pagename + " created successfully!");
    }else{
        Serial.println("page " + (String)pagename + " could not be created!");
        return;
    }

    // Write sample data
    itoa(sampleData, buffer, 10);
    currentPage.write(buffer, MAXPAGESIZE);
    firstRun = false;
}

// Function to read array data from the flash
void readFromFlash(){
    uint32_t pagesize;
    char pagename[64];
    char buffer[MAXPAGESIZE];
    SerialFlashFile dummy;
    SerialFlash.opendir();
    int counter = 0;

    // Read all pages that are stored in the flash memory
    while(SerialFlash.readdir(pagename, sizeof(pagename), pagesize)){
        dummy = SerialFlash.open(pagename);
        dummy.read(buffer, MAXPAGESIZE);
        Serial.println("page " + (String)pagename + ":");
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
  startTime = millis();
  Serial.begin(9600);
  delay(3000);
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
  SerialFlash.opendir();
  
  //Sample pages
    char pagenames[10][10] =   {"page0.bin",
                                "page1.bin",
                                "page2.bin", 
                                "page3.bin",
                                "page4.bin",
                                "page5.bin",
                                "page6.bin", 
                                "page7.bin",
                                "page8.bin",
                                "page9.bin"};
    
    SerialFlashFile page0;
    SerialFlashFile page1;
    SerialFlashFile page2;
    SerialFlashFile page3;
    SerialFlashFile page4;
    SerialFlashFile page5;
    SerialFlashFile page6;
    SerialFlashFile page7;
    SerialFlashFile page8;
    SerialFlashFile page9;
    SerialFlashFile pages[] = {page0, page1, page2, page3, page4, page5, page6, page7, page8, page9};

    // Sample data
    int16_t batterySOCs[] =     {12,  55,  0,  64,  71,  23,  49,  99, 100, 86};
    int16_t batteryVoltages[] = {101, 110, 99, 105, 105, 102, 105, 99, 100, 103};
    int16_t batteryCurrents[] = {55,  -2,  19, 0,   150, 77,  56,  20, 99,  103};
    
    eraseAllData();
    //readFromFlash();
    /*
    // Call functions
    writeToFlashTest(pages, pagenames, batterySOCs, batteryVoltages, batteryCurrents);
    delay(2000);
    Serial.println();
    readFromFlash();
    eraseAllData();
    //readFromFlash();
    */
    //Serial.end();
}

// Writes the buffer
void loop(){
    writeAsRingBuffer();
    sampleData++;
    /*
    if(millis() > startTime + 5000){
        readFromFlash();
        startTime = millis();
    }
    */
}