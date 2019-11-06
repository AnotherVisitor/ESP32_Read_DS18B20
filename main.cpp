/* 
Short:
   Read from DS18B20 

Long:
   Read temperature from a Dallas 18B20 and use serial interface to print it 
   in Celsius and Fahrenheit.

Hardware: ESP32 connected to a Dallas 18B20
   ESP32       18B20
   -----       -----
   GND         GND
   3.3V        +
   15          Data



Version history:
   Init 06NOV2019
   V01  
   V02

*/

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 15
#define TEMPERATURE_PRECISION 12 // Set resolution

//Prototypes
void printAddress(DeviceAddress);

// Create a oneWire instance
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to DallasTemperature. 
DallasTemperature sensors(&oneWire);

int numberOfDevices;

DeviceAddress tempDeviceAddress; // We'll use this variable to store a found device address

void setup(void)
{
  // start serial port
  Serial.begin(19200);
  Serial.println("Start...");

  // Start up the library
  sensors.begin();
  
  // Grab a count of devices on the wire
  numberOfDevices = sensors.getDeviceCount();
  
  // locate devices on the bus
  Serial.print("Locating devices...");
  
  Serial.print("Found ");
  Serial.print(numberOfDevices, DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  
  // Loop through each device, print out address
  for(int i=0;i<numberOfDevices + 1; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	  {
		  Serial.print("Found device ");
		  Serial.print(i, DEC);
		  Serial.print(" with address: ");
		  printAddress(tempDeviceAddress);
		  Serial.println();
		
		  Serial.print("Setting resolution to ");
		  Serial.println(TEMPERATURE_PRECISION, DEC);
		
		  // set the resolution to TEMPERATURE_PRECISION bit (Each Dallas/Maxim device is capable of several different resolutions)
		  sensors.setResolution(tempDeviceAddress, TEMPERATURE_PRECISION);
		
		  Serial.print("Resolution actually set to: ");
		  Serial.print(sensors.getResolution(tempDeviceAddress), DEC); 
		  Serial.println();
	  }else{
		  Serial.print("Found ghost device at ");
		  Serial.print(i, DEC);
		  Serial.print(" but could not detect address. Check power and cabling");
	  }
  }
  delay(5000);
} // end of setup

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  float tempC = sensors.getTempC(deviceAddress);
  Serial.print("Temp C: ");
  Serial.println(tempC);
  Serial.print("Temp F: ");
  Serial.println(DallasTemperature::toFahrenheit(tempC)); // Converts tempC to Fahrenheit
} // end of printTemperature

void loop(void)
{ 
  // call sensors.requestTemperatures() to issue a global temperature request to all devices on the bus
  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // Loop through each device, print out temperature data
  for(int i=0;i<numberOfDevices + 1; i++)
  {
    // Search the wire for address
    if(sensors.getAddress(tempDeviceAddress, i))
	  {
		  // Output the device ID
		  Serial.print("Temperature for device: ");
		  Serial.println(i,DEC);
		  // It responds almost immediately. Let's print out the data
		  printTemperature(tempDeviceAddress); // Use a simple function to print out the data
      delay(1000);
      Serial.flush(); 
      esp_deep_sleep_start();
    } // end of if
  } // end of for
} // end of loop

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
} // end of printAddress
