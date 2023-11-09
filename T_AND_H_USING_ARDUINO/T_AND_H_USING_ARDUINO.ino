#include <dht11.h>
#define dht_apin 11 // Analog Pin sensor is connected to
dht11 dhtObject;
String AP = "xxxxxxxx";       // AP NAME
String PASS = "xxxxxx"; // AP PASSWORD
String API = "MHOBO8TB0GYACQT4";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";

// Define relay control pins
const int relayPin = 4; // Change this to your actual relay pin number

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(5000); // Set a timeout for serial communication
  Serial.println("Connecting to WiFi...");
  sendCommand("AT", "OK");
  sendCommand("AT+CWMODE=1", "OK");
  sendCommand("AT+CWJAP=\"" + AP + "\",\"" + PASS + "\"", "OK");

  // Initialize relay pin as an OUTPUT
  pinMode(relayPin, OUTPUT);
  // Initially, turn off the relay
  digitalWrite(relayPin, LOW);
}

void loop() {
  String temperatureValue = getTemperatureValue();
  String humidityValue = getHumidityValue();

  String getData = "GET /update?api_key=" + API + "&field1=" + temperatureValue + "&field2=" + humidityValue;
  sendCommand("AT+CIPMUX=1", "OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\"" + HOST + "\"," + PORT, "OK");
  sendCommand("AT+CIPSEND=0," + String(getData.length() + 4), ">");
  Serial.println(getData);
  delay(1500);
  sendCommand(getData, "OK");
  sendCommand("AT+CIPCLOSE=0", "OK");

  // Check temperature and control the relay
  int temperature = temperatureValue.toInt();
  if (temperature >= 30) { // Adjust the temperature threshold as needed
    digitalWrite(relayPin, HIGH); // Turn on the relay
  } else {
    digitalWrite(relayPin, LOW); // Turn off the relay
  }

  delay(5000); // Adjust the delay as needed
}

String getTemperatureValue() {
  dhtObject.read(dht_apin);
  Serial.print("Temperature(C)=");
  int temp = dhtObject.temperature;
  Serial.println(temp);
  delay(50);
  return String(temp);
}

String getHumidityValue() {
  dhtObject.read(dht_apin);
  Serial.print("Humidity(%)=");
  int humidity = dhtObject.humidity;
  Serial.println(humidity);
  delay(50);
  return String(humidity);
}

void sendCommand(String command, String expectedResponse) {
  Serial.print("Sending command: ");
  Serial.println(command);
  Serial.setTimeout(5000);
  Serial.println(command);

  if (waitForResponse(expectedResponse)) {
    Serial.println("Command successful");
  } else {
    Serial.println("Command failed");
  }
}

bool waitForResponse(String expectedResponse) {
  unsigned long timeout = millis() + 5000; // 5-second timeout
  String response;
  
  while (millis() < timeout) {
    if (Serial.available()) {
      response += static_cast<char>(Serial.read());
      if (response.endsWith(expectedResponse)) {
        return true;
      }
    }
  }
  
  return false;
}
