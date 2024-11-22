//Default code present in all shipped boards.
//For programming ESP32 and serial monitor, connect a USB Type A to B cable from the board to a laptop. 
//Open Arduino Serial Monitor set Baud Rate to 115200 and observe the data log. 
//To upload new code via Arduino, change board settings to ESP32S3 Dev Module, ensure “USB CDC on Boot” is set to Enabled under Tools,
// install the relevant libraries and upload the code. 
//Jumpers J1 and J2 are provided for boot and  reset functionality respectively in-case new code becomes unresponsive.
// Vajravolt IoT Gateway VVM701 All Peripherals Connection Test
//This code is used to demonstrate connectivity of all devices such as 4G Module, Ethernet, SD Card, RTC, Digital Inputs, etc.
// Code is OTA enabled using Elegant OTA library
//FOR VVM701 PRODUCT DETAILS VISIT www.vv-mobility.com

#define TINY_GSM_MODEM_BG96  //worked for Quectel EC200
#define SerialAT Serial1
#define TINY_GSM_USE_GPRS true

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <ElegantOTA.h> //https://github.com/ayushsharma82/ElegantOTA
#include <TinyGsmClient.h>  //https://github.com/vshymanskyy/TinyGSM
#include <Wire.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>
#include <RTClib.h> //https://github.com/adafruit/RTClib
#include <Ethernet.h> // Ethernet library v2 is required for proper operation

// Pin Definitions

#define RXD1 40    //4G MODULE RXD INTERNALLY CONNECTED, Hardware Serial 1
#define TXD1 41    //4G MODULE TXD INTERNALLY CONNECTED, Hardware Serial 1
#define powerPin 42 ////4G MODULE ESP32 PIN D4 CONNECTED TO POWER PIN OF EC200 CHIPSET, INTERNALLY CONNECTED
#define RXD2 2 // RS485 or RS232, Hardware Serial2 
#define TXD2 1 // RS485 or RS232, Hardware Serial2

const int statusLED = 18; // Onboard LED
int rx = -1;
String rxString;
WebServer server(80);
unsigned long ota_progress_millis = 0;
const char* ssid = "iotgateway"; // Replace this with your SSID Password so that device can connect to WiFi. This is required for updating ESP32 code via OTA
const char* password = "iot@1234";  // minimum 8 digit

const char apn[]      = ""; //APN automatically detects for 4G SIM IN MOST CASES, IF NOT DETECTED, THEN ENTER APN

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, Serial);
TinyGsm        modem(debugger);
#else
TinyGsm        modem(SerialAT);
#endif


#define SD_CS_PIN 48  // SD Card uses HSPI pins and Ethernet uses default VSPI
#define SD_CLK_PIN 39
#define SD_MOSI_PIN 47
#define SD_MISO_PIN 38


byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// Set the static IP address to use if the DHCP fails to assign
#define MYIPADDR 192,168,0,27
#define MYIPMASK 255,255,255,0
#define MYDNS 192,168,0,1
#define MYGW 192,168,0,1
char googleServer[] = "httpbin.org";    // name address for Google (using DNS)

// For RS232/RS485
char serialRead, serialWrite;

// Initialize Objects
EthernetClient LANClient;
RTC_DS3231 rtc;
SPIClass spi2(HSPI);  //SD Card
TinyGsmClient client(modem);
String readBuffer;
unsigned long acquireData = 0;
unsigned long turnOnRelay1 = 0;
unsigned long turnOnRelay2 = 0;
bool relay1ON = 0;
bool relay2ON = 0;
void setup() {
  // Enable the 4G chipset
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, LOW);
  pinMode(statusLED, OUTPUT);
  digitalWrite(statusLED, HIGH); //turn statusLED On or Off as per your application scenario
  delay(3000);
  // Initialize Serial Communication
  Serial.begin(115200); //Default Serial Monitor
  SerialAT.begin(115200, SERIAL_8N1, RXD1, TXD1); //Serial 1 for 4G
  Serial2.begin(9600,  SERIAL_8N1, RXD2, TXD2); //Serial 2 for RS485 / RS232
  Serial0.begin(9600);  // Data Acquisition of Analog/Digital Channels
  delay(10);
  Wire.begin(5, 4);  // this is essential
  delay(100);
  // Fetch RTC Data
  if (!rtc.begin()) {
    Serial.println("RTC initialization failed!");
    //return; //CHECK RETURN FOR BOTH SD AND RTC
  }
  else  {
    Serial.println("RTC initialized.");
    DateTime now = rtc.now();
    String timestamp = now.timestamp(DateTime::TIMESTAMP_FULL);
    Serial.print("RTC Time: ");
    Serial.println(timestamp);
  }

  // Initialize SD Card
  spi2.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN, spi2, 80000000)) {
    Serial.println("Card Mount Failed");
  }
  else
  {
    Serial.println("SD card initialized.");
    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
    }
    else
    {
      Serial.print("SD Card Type: ");
      if (cardType == CARD_MMC) {
        Serial.println("MMC");
      } else if (cardType == CARD_SD) {
        Serial.println("SDSC");
      } else if (cardType == CARD_SDHC) {
        Serial.println("SDHC");
      } else {
        Serial.println("UNKNOWN");
      }
    }
    writeFile(SD, "/Hello.txt", "Hello IoT World!");  // Write Test Data
  }


  // Initialize I2C devices
  // Print I2C Device Addresses
  scanI2CDevices();
  // check WiFi...
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  checkWiFi();
  if(!MDNS.begin("iotgateway")) {
     Serial.println("Error starting mDNS");
     //return;
  }
  server.on("/", []() {
    server.send(200, "text/plain", "Hi! This is Elegant OTA on Vajravolt...");
  });

  ElegantOTA.begin(&server);    // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  server.begin();
  Serial.println("HTTP server started");
  
  // Check Ethernet Connection
  checkEthernetConnection();
  //check 4G module and Internet Connectivity
  check4Gmodule();
  Serial.println("Reset MS51 controller");
  Serial0.println("RSTDATA"); //sending RESET command to MS51 controller
  delay(1000);
  readBuffer = Serial0.readStringUntil('\n');
  Serial.println(readBuffer);
  Serial.println("Data from Serial Monitor Input Port will be sent to Serial2 RS232 or RS485");
  Serial.println("Data from Serial2 RS232 or RS485 will be displayed on Serial Monitor Port");
  Serial.println("Only RS232 or RS485 can be used at a time, depending on the DIP Jumper Selection on the PCB");
}

void loop() {
  if (Serial.available() > 0) // Data from Serial Monitor port is sent to RS232/485
  {
    serialRead = Serial.read();
    Serial2.write(serialRead);
  }
  if (Serial2.available() > 0) //read Response commands from RS232/RS485 and send to user Serial Port
  {
    serialRead = Serial2.read();
    Serial.write(serialRead);
  }
  if((millis() - acquireData) >= 3000)  //get Analog / Digital data every 3 seconds
  {
    acquireData = millis();
    Serial.println("Acquiring Analog and Digital Data");
    Serial0.println("GETDATA"); //sending GETDATA command to MS51 controller
    delay(1000);
    readBuffer = Serial0.readStringUntil('\n');
    Serial.println(readBuffer);
    if(relay1ON == 0)
    {
      relay1ON = 1;
      Serial.println("Turning on Relay 1");
      Serial0.println("RL1ON"); //Sending RL1ON to MS51 controller to turn on relay. To turn off Relay 1 send RL1OFF
      delay(1000);
    }
    if(relay2ON == 0)
    {
      relay2ON = 1;
      Serial.println("Turning on Relay 2");
      Serial0.println("RL2ON"); //Sending RL2ON to MS51 controller to turn on relay. To turn off Relay 2 send RL2OFF
      delay(1000);
    }    
  }
  server.handleClient();
  ElegantOTA.loop();
}

void scanI2CDevices() {
  byte error, address;
  int deviceCount = 0;

  Serial.println("Scanning for I2C devices like RTC, EEPROM, LCD, etc..."); // DS3231 module is present onboard. I2C address for RTC is 0x68

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.print(address, HEX);
      Serial.println(" !");
      deviceCount++;
    }
  }

  if (deviceCount == 0) {
    Serial.println("No I2C devices found.");
  } else {
    Serial.print("Found ");
    Serial.print(deviceCount);
    Serial.println(" I2C device(s).");
  }
}

void checkEthernetConnection() {
  Ethernet.init(21);        // Chip Select pin
  Serial.println("Testing Ethernet DHCP...plz wait for sometime");
  if (Ethernet.begin(mac)) { // Dynamic IP setup
    Serial.println("DHCP OK!");
  } else {
    Serial.println("Failed to configure Ethernet using DHCP, Trying Static IP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet was not found. Sorry, can't run without hardware. :(");
      return;
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
      return;
    }
    IPAddress ip(MYIPADDR);
    IPAddress dns(MYDNS);
    IPAddress gw(MYGW);
    IPAddress sn(MYIPMASK);
    Ethernet.begin(mac, ip, dns, gw, sn);
    Serial.println("STATIC OK!");
  }
  delay(5000);              // give the Ethernet shield some time to initialize
  Serial.print("Local IP : ");
  Serial.println(Ethernet.localIP());
  Serial.print("Subnet Mask : ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("DNS Server : ");
  Serial.println(Ethernet.dnsServerIP());

  Serial.println("Ethernet Successfully Initialized");
  // if you get a connection, report back via serial:
  if (LANClient.connect(googleServer, 80)) {
    Serial.println("Server Connected via Ethernet!");
    // Make a HTTP request:
    LANClient.println("GET /get HTTP/1.1");
    LANClient.println("Host: httpbin.org");
    LANClient.println("Connection: close");
    LANClient.println();
  } else {
    // if you didn't get a connection to the server:
    Serial.println("Connection to Server failed");
  }
}

void checkWiFi()
{
  WiFi.disconnect();
  delay(100);
  WiFi.reconnect();
  delay(100);
  Serial.print("Connecting to WiFi");
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
    counter++;
    if (counter >= 25)
    {
      Serial.println("No WiFi");
      WiFi.mode(WIFI_AP_STA);
      return;
    }
  }
  Serial.println("WiFi connected as");
  Serial.println(WiFi.localIP());
  delay(100);
}

void check4Gmodule()
{
  Serial.println("\nconfiguring 4G Module. Kindly wait");
  delay(5000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  DBG("Initializing modem...");
  if (!modem.init()) {
    DBG("Failed to restart modem");
    return;
  }
  // Restart takes quite some time
  // To skip it, call init() instead of restart()

  String name = modem.getModemName();
  DBG("Modem Name:", name);

  String modemInfo = modem.getModemInfo();
  DBG("Modem Info:", modemInfo);

  Serial.println("Waiting for network...");
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" success");
  delay(15000);
  if (modem.isNetworkConnected()) {
    Serial.println("Connected to Network");
  }
  else
    Serial.println("No Network");


  // GPRS connection parameters are usually set after network registration
  Serial.print(F("Connecting to 4G"));
  Serial.print(apn);
  if (!modem.gprsConnect(apn)) {
    Serial.println(" failed");
    return;
  }
  Serial.println(" success");

  if (modem.isGprsConnected()) {
    Serial.println("LTE Internet connected");
  }
  else
  {
    Serial.println("No LTE Internet");
  }

}

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

///////////////SD Card Functions////////////////////////////
void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void createDir(fs::FS &fs, const char * path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}

void removeDir(fs::FS &fs, const char * path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}

void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}

void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}

void testFileIO(fs::FS &fs, const char * path) {
  File file = fs.open(path);
  static uint8_t buf[512];
  size_t len = 0;
  uint32_t start = millis();
  uint32_t end = start;
  if (file) {
    len = file.size();
    size_t flen = len;
    start = millis();
    while (len) {
      size_t toRead = len;
      if (toRead > 512) {
        toRead = 512;
      }
      file.read(buf, toRead);
      len -= toRead;
    }
    end = millis() - start;
    Serial.printf("%u bytes read for %u ms\n", flen, end);
    file.close();
  } else {
    Serial.println("Failed to open file for reading");
  }


  file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  size_t i;
  start = millis();
  for (i = 0; i < 2048; i++) {
    file.write(buf, 512);
  }
  end = millis() - start;
  Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
  file.close();
}

///////////////////end of SD Card Functions /////////////////////////////
