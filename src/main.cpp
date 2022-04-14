#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 2

// Define the array of leds
CRGB leds[NUM_LEDS];

const char* ssid = "/dev/lol";
const char* password = "4dprinter";

String cookie = "";
unsigned long delay_ms = 300;
unsigned int unlocked = 0;

//unsigned long lastTime = 0;
//unsigned long timerDelay = 10000;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  WiFi.begin(ssid, password);
}

void login() {
  Serial.println("login");
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();

  // Your Domain name with URL path or IP address with path
  http.begin(client, "https://192.168.8.7/login");
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  // Data to send with HTTP POST
  String httpRequestData = "username=&password=";
  http.setReuse(true);

  const char* keys[] = {"Set-Cookie"};
  http.collectHeaders(keys, 1);

  // Send HTTP POST request
  int httpResponseCode = http.POST(httpRequestData);

  cookie = http.header("Set-Cookie");

  // Free resources
  http.end();
}

void unlock() {
  Serial.println("unlock");
  WiFiClientSecure client;
  HTTPClient http;

  client.setInsecure();

  // Your Domain name with URL path or IP address with path
  http.begin(client, "https://192.168.8.7/cmd/unlock");
  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.addHeader("Cookie", cookie);
  // Send HTTP POST request
  int httpResponseCode = http.GET();

  // Free resources
  http.end();

  unlocked = 1;
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    // wait for wifi connection
    leds[0] = CRGB::Yellow;
    delay_ms = 200;
 
    Serial.print(".");
  } else if (WiFi.status() == WL_CONNECTED && cookie.length() == 0) {
    // login (blocking)
    leds[0] = CRGB::Orange;
    delay_ms = 400;
    FastLED.show();

    login();
  } else if (WiFi.status() == WL_CONNECTED && !unlocked) {
    //unlock
    leds[0] = CRGB::Red;
    delay_ms = 600;
    FastLED.show();

    unlock();
  } else {
    delay_ms = 600;
    leds[0] = CRGB::Green;
  }

  FastLED.show();
  delay(delay_ms);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(200);
}
