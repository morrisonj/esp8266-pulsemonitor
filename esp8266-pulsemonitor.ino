#include <ESP8266WiFi.h>

// WiFi settings here
const char* ssid     = "xxx";
const char* password = "xxx";

// InfluxDB Server Host
const char* host = "xxx";

int sensorValue = 0;
unsigned long lastPulseSent = 0;

// Connect to WiFi
void setup() {
  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Slow it down a bit.
  delay(30);
  // Check how bright the LED is.
  sensorValue = analogRead(A0);
  // Display to Serial for tweaking.
  Serial.println(sensorValue);

    // This sensor value check may need tweaking depending
    // on how bright the LED is on your meter. Increase it
    // if you get false positives or decrease it if it
    // doesn't trigger. 
    if (sensorValue > 512) {
      //Serial.println(sensorValue);
      delay(100);

      Serial.print("connecting to ");
      Serial.println(host);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;
      const int httpPort = 8086;
      if (!client.connect(host, httpPort)) {
        Serial.println("connection failed");
        return;
      }

      // We now create a URI for the request
      // Change his to your InFlux DB name
      String url = "/write?db=home";

      Serial.print("Requesting URL: ");
      Serial.println(url);

      // This will send the request to the server
      // This post request will need modifying depending on your
      // InfluxDB setup. Autorization / post body should be
      // set to your InfluxDB.
      client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + ":8086\r\n" +
                   "Authorization: Basic xxx\r\n" +
                   "User-Agent: ESP8266/1.0\r\n" +
                   "Connection: close\r\n" +
                   "Content-Type: text/plain\r\n" +
                   "Content-Length: 30\r\n\r\n" +
                   "energyusage,host=house value=1\r\n");
      unsigned long timeout = millis();
      while (client.available() == 0) {
        if (millis() - timeout > 5000) {
          Serial.println(">>> Client Timeout !");
          client.stop();
          return;
        }
      }

      // Read all the lines of the reply from server and print them to Serial
      while (client.available()) {
        String line = client.readStringUntil('\r');
        Serial.print(line);
      }

      Serial.println();
      Serial.println("closing connection");

    }
}
