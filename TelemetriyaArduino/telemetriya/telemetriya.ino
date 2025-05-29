#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <TimeLib.h>
#include <DS1302.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPL3115A2.h>


#define teamID 3138
#define GPS Serial2
#define XBEE Serial1
#define SD_CS BUILTIN_SDCARD

Adafruit_MPL3115A2 mp = Adafruit_MPL3115A2(); //initializing altitude sensor in I2C mode

File canFile;
uint16_t currentTime;
double refaltit, altit, spd, gps[3];
int secs, hours, mins, seconds, r, res, done, refpackage, status, photo = 0, package = 0, limit = 9, c, n, d, p, light;



void setup() {
  GPS.begin(9600); //open Serial port for GPS
  XBEE.begin(9600); //open serial port for xbee
  Serial.begin(9600); //open serial port for USB read
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH); //indicates if SONGHE works, FIGURE OUT THIS 13
  pinMode(D9, OUTPUT); //buzzer
  res = EEPROM.read(10);
  digitalWrite(D9, HIGH);
  setupMP(); //check if there is connection with smp sensor
  indicateStatus(); //check if SONGHE has been reseted
  setupSDcard(); //check if there is connection with SD card
  setupCamera(); 
  delay(1000);
  setupTime(); //set DS1307 to valid time
  digitalWrite(D9, LOW);
  Serial.print("refaltit: ");
  Serial.println(refaltit);
}

void loop() {
  
Serial.print("status: "); 
Serial.println(status);

  if(status != 3){
  
  digitalWrite(D9, !digitalRead(D9)); //digitalRead HAL HAZIRDA SENSORUN voltagesinin valuesini qaytarir (ex. LOW, HIGH)

  uint64_t prevTime = millis();
  
  if(status != 2) //measure light only if the model has not been released
  // senseLight(); PARASHUT ACILMASI
  //DIQQET!
  getTime(); //read seconds
  getAltitude(); //read altitude data

  if(status == 2 ){ //after release begin to send GPS data
  getGpsData(); //get GPS data  
  }

  if(c==0){
    
  // takePhoto(); DIQQET
  c++;
  
  }

  else if(c==limit)
  c = 0;
  
  else c++;
  
  if(status) //after command begin to write to SD card

  writeToSD();

  delay(20);

  sendtoXBee();

  
  done = 0; //set read flag for GPS data again
  photo = 0;
  package++;

  delay(50);

  if(package > 255){
    
  p = package/255;
  EEPROM.write(18, p);
    
  }

  EEPROM.write(16, package - p*255);
  delay(50);

  
  //if(status == 2 /*&& !d */&& (altit - refaltit) < 0.5){ //if it is landing and difference 
  //bewteen reference and real altitude is 0, then it is on ground
  //status = 3;
  //EEPROM.write(15, status);
  //}
  
  uint64_t currTime = millis();
  int del = 1000 - (currTime - prevTime);

  if(del > 0)
  delay(del);

  else
  delay(1);
  }


  else if(status == 3)  //if it is already on ground send sound signal
  soundSignal();
  
}
//DEYISMELIDI
time_t getTeensyTime(){

  return Teensy3Clock.get();
  
}

//PIN DEQIQLESDIR SILMEK LAZIMDIR?
void setupCamera(){
  
  pinMode(29, OUTPUT);
  delay(500);
  digitalWrite(29, HIGH); //camera trigger pin needs to be pulled up when not taking pictures
  delay(3000);
  digitalWrite(29, LOW);
}

void setupSDcard(){
  //SD PINININ ADINI YAZ
  if(!SD.begin(SD_CS)){

    Serial.println("SD card failed");
 
  }
  
}

void setupTime(){

   

  //setSyncProvider(getTeensyTime); //activate internal RTC MUTLEQ NEZERDEN KECIR

  //setTime(hours, mins, seconds, day(), month(), year()); //reset only clock
   rtc.begin();
   rtc.setDateTime(hours, mins, seconds); //CHECK THIS
}

void setupMP(){

  if (!mp.begin()) {
    Serial.println("Could not find a valid MPL3115A2 sensor, check wiring!");
  }

}

//ASK THIS
void indicateStatus(){
  
  if(res){

  delay(50);
  hours = EEPROM.read(11); //if reseted then read time and other reference data from EEPROM
  mins  = EEPROM.read(12);
  seconds = EEPROM.read(13);
  refaltit = EEPROM.read(14);
  status = EEPROM.read(15);
  delay(10);

  package = EEPROM.read(18)*255 + EEPROM.read(16);
  refpackage = EEPROM.read(20)*255 + EEPROM.read(17);

  delay(50);
  
  }

  else{
  //if no, then indicate that teensy have been turned on for the first time and write reference altitude
  EEPROM.write(10, 1);
  
  }

}

//BIZDE PARASHUT OZ OZUNE ACILACAQ DEYE BUNA EHTIYAC YOXDUR
// void senseLight(){

//   light = analogRead(A15);
  
//   if(light > 800 && status == 1 ){
    
//   status = 2; //change status to "enir"
//   c = 0;
//   EEPROM.write(15, status); //update status in EEPROM
  
//   }
  
// }

//MUTLEQ BAX
void getTime(){

  if(r){
    
    rtc.setDateTime(hours, mins, seconds); //reset clock in each 5 mins
    r = 0;
    
  }
  
  hours = hour();
  mins = minute();
  seconds = second();
  
  secs = hours*3600 + mins*60 + seconds;

  delay(10);
  EEPROM.write(11, hours);
  EEPROM.write(12, mins);
  EEPROM.write(13, seconds+1);
  delay(10);
  
  if(secs % 299 == 0 && secs != 0){ //after each 5 mins take last clock values to reset the clock again
    
  r = 1;
  seconds++;
  
  }
  
  
}

void getAltitude(){

  uint16_t prevTime = currentTime;
  
  currentTime = millis();
  double prevAltit = altit;
   altit = mp.readAltitude(1007); //current pressure should be here BELE DEYISDIM BAXMAQ LAZIMDIR
  // altit = Bmp.readAltitude(1007); //current pressure should be here

  spd = (prevAltit - altit) * 1000/(currentTime - prevTime);
  
}


void getGpsData() {

  int i = 0;
  bool readData = false;
  char rawData, readerBuffer[100];

  while (done != 1) {
    
       if (done == 1) //done indicates if GPGGA data is received, if yes, exit from reading GPS data
       break;

  uint64_t t = millis();

    while (!GPS.available()) {

      if (millis() - t > 500) { //500 ms is timout limit
        
        done = 1; //if there is no GPS data in 500 ms exit from loop, return 0
        break;
        
      }
      
    };//wait if no data is ready

  rawData = GPS.read(); //read char from data register

  if(rawData == '$'){ //if new data block
    
      readerBuffer[0] = '\0'; //clear all array
      i = 0;
      readerBuffer[i] = '$';
      readData = true; //begin to read the rest
      i++;
      
  }

  else if(rawData == '*'){ //if data block ends
    
      readData = false; //stop reading
      parseData(readerBuffer);
      
 }

  else if(readData){ //contuinue to collect chars to String
    
      readerBuffer[i] = rawData;
      i++;
    
    }
    
  }

}

void parseData(char* data) {

  if (data[3] == 'G' && data[4] == 'G') { //if geographic data is received

    getData(data); //extract longitude and latitude from GPGGA data block
    gps[0] = convertData(gps[0]); //convert raw data to readable form
    gps[1] = convertData(gps[1]);
    done = 1; //indicate operation is complete
    
  }



}


float convertData(float num) {

  //convert DD:MM to degree
  num = (int)num / 100 + fmod(num, 100) / 60;
  return num;

}

void getData(char *data) {
  
  //this method collects gps latitude and longitude
  const char delimit[2] = ","; //comma is delimiter between each 2 data
  char *token;
  int i = 0;
  int count = 0;
  token = strtok(data, delimit);

  while (token != NULL) {

    if (count == 2 || count == 4) { //longitude stands in 3rd position while latitude in 4th
      gps[i] = atof(token);
      i++;
    }

    if (count == 4) //after reading latitude exit from loop
      break;   
    
    count++;
    token = strtok(NULL, delimit);

  }

}

void writeToSD(){

  canFile = SD.open("7071.csv", FILE_WRITE);

  if(canFile){

    canFile.print(teamID);
    canFile.print(",");
    canFile.print(secs);
    canFile.print(",");
    canFile.print(package - refpackage);
    canFile.print(",");
    //BATEREYA VOLTAGE KODU YAZMALIYIQ
    if(status)
    canFile.print(altit - refaltit);
    else
    canFile.print(altit);
    if(status == 2){
    canFile.print(",");
    canFile.print(spd);
    canFile.print(",");
    canFile.print(gps[0], 6);
    canFile.print(",");
    canFile.print(gps[1], 6);
    canFile.print(",");
    //DASIYICIDAN AYRILDIGI VAXT KODU YAZMALIYIQ
    canFile.println(photo);
    //VIDEOGORUNTUNUN MUDDETININ KODUNU YAZMALIYIQ
   }

    
  // else{
    //  if(status != 0)
    //canFile.println();
   //}
  
   canFile.close();
  }



  else {
    // if the file didn't open, print an error:
   // Serial.println("error opening test.txt");
  }
}

void sendtoXBee(){

//YUXARIDAKILARIN EYNISINI BURDA DA ETMEK LAZIMDIR !CANSTELLAR!
    XBEE.print("<"+teamID+">");
    XBEE.print(",");
    XBEE.print(secs);
    XBEE.print(",");
    XBEE.print(package - refpackage);
    XBEE.print(",");
   // XBEE.print(light);
   // XBEE.print(",");
    
    if(status)
    XBEE.print(altit - refaltit);
    else
    XBEE.print(altit);
    //XBEE.println(altit);

    //if(status == 2){
      
    XBEE.print(",");
    XBEE.print(spd);
    XBEE.print(",");
    XBEE.print(gps[0], 6);
    XBEE.print(",");
    XBEE.print(gps[1], 6);
    XBEE.print(",");
    XBEE.println(photo);
    //XBEE.print(",");
   // XBEE.println(status);
    
  //  }

  // else {
  // if(status != 0)
   // XBEE.println();   
  // }
    delay(10);
}

void takePhoto(){

  digitalWrite(29, HIGH); //take trigger pin to low for 100 ms to take a picture 
  delay(100);
  digitalWrite(29, LOW);
  photo = 1;
  
}

void soundSignal(){
//buzzer
  digitalWrite(D9, HIGH);
  delay(500);
  digitalWrite(D9, LOW);
  delay(300);
  
}


void serialEvent1(){

char c =  XBEE.read();

if( c == 'r' || c == 'R'){
  
   status = 1; //indicate that it is raising
   delay(10);
   EEPROM.write(15, status); //update status
   delay(10);
   setTime(0, 0, 0, day(), month(), year()); //reset time
   refpackage = package; //take the last package as reference
   refaltit = altit; //take altitude as reference
   delay(10);
   EEPROM.write(14, refaltit); //write references to EEPROM
    //XBEE.print("refaltit");
   // XBEE.print(altit);
   if(refpackage > 255){
 
  EEPROM.write(20, refpackage/255);
    
  }
  
   EEPROM.write(17, refpackage - (int)(refpackage/255)*255 );
   delay(10);
   
}

else if(c == 'c' || c == 'C'){

  for ( unsigned int i = 0 ; i < EEPROM.length() ; i++ )
    EEPROM.write(i, 0);
  
}
}