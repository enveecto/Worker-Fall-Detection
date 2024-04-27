#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define BUZZER_PIN 0 // GPIO pin connected to the buzzer
#define THRESHOLD_FREE_FALL 2.5 // Threshold value for free fall detection (adjust as needed)
#define FREE_FALL_DURATION_THRESHOLD 50 // Minimum duration of free fall in milliseconds (0.5 seconds)

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

unsigned long freeFallStartTime = 0; // Variable to store the start time of free fall

const char* ssid = "HP";
const char* password = "12345678";

AsyncWebServer server(80);

bool fallDetected = false; // Flag to indicate if fall is detected

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  if (!accel.begin()) {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("WiFi connected.");
  
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String html = "<html><head>";
    html += "<style>body{font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; text-align: center; background-color: #000; color: #fff;}"
            ".container {margin: auto; width: 60%; border: 3px solid #fff; padding: 20px; border-radius: 15px;}"
            "h1{font-size: 30px; margin-bottom: 20px;}"
            "p{font-size: 22px; margin-bottom: 10px;}"
            ".important{color: red;}"
            ".safe{color: green;}"
            ".button {background-color: #4CAF50; /* Green */border: none;color: white;padding: 15px 32px;text-align: center;text-decoration: none;display: inline-block;font-size: 24px;margin: 10px 10px;cursor: pointer; border-radius: 8px;}"
            ".button2 {background-color: #008CBA; /* Blue */ border-radius: 8px;}"
            ".footer{position: fixed; bottom: 10px; width: 100%; color: #fff; font-size: 18px;}"
            "</style>";
    html += "<meta http-equiv='refresh' content='1'></head><body>";
    html += "<div class='container'>";
    html += "<h1>Worker Safety Monitoring System</h1>";

    // Display emergency message if fall is detected
    if (fallDetected) {
      html += "<p class='important'>EMERGENCY - FALL DETECTED!</p>";
      html += "<p class='important'>Follow these steps:</p>";
      html += "<ol>";
      html += "<li>Assess Safety: Ensure the area is safe to approach.</li>";
      html += "<li>Check Responsiveness: Gently call out and assess their response.</li>";
      html += "<li>Airway: Ensure the airway is clear.</li>";
      html += "<li>Breathing: Look, listen, and feel for breathing.</li>";
      html += "<li>Pulse: Check for a pulse on the neck.</li>";
      html += "<li>External Injuries: Assess for visible injuries.</li>";
      html += "<li>Neurological Assessment: Check pupil size, limb movement, and sensation.</li>";
      html += "<li>Maintain Immobilization: Keep them still, especially if suspecting a spinal injury.</li>";
      html += "<li>Comfort and Reassurance: Provide comfort and keep them calm.</li>";
      html += "<li>Monitor Vital Signs: Continuously monitor consciousness, breathing, and pulse.</li>";
      html += "</ol>";

      // Add buttons for emergency services
      html += "<a class='button' href='https://www.tnfrs.tn.gov.in/contact/'>Call Fire Department</a>";
      html += "<a class='button button2' href='https://www.cmcvellorechittoorcampus.ac.in/emergency-care/'>Call Medical Services</a>";
      
      // Turn on the buzzer
      activateBuzzer();
    } else {
      html += "<p class='safe'>Worker is Safe</p>";
    }

    html += "</div>";
    html += "<div class='footer'>Created by Team AutoBots, VIT</div>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });


  server.begin();
}

void loop() {
  // Check for fall condition
  float acceleration = getAcceleration();
  if (acceleration < THRESHOLD_FREE_FALL) {
    unsigned long freeFallDuration = millis() - freeFallStartTime;
    if (freeFallDuration >= FREE_FALL_DURATION_THRESHOLD) {
      fallDetected = true; // Set fall detection flag
    }
  } else {
    freeFallStartTime = millis(); // Start or update the free fall timer
  }

  delay(100); // Small delay to reduce CPU usage
}

float getAcceleration() {
  sensors_event_t event;
  accel.getEvent(&event);
  return sqrt(pow(event.acceleration.x, 2) + pow(event.acceleration.y, 2) + pow(event.acceleration.z, 2));
}

void activateBuzzer() {
  tone(BUZZER_PIN, 1000); // Start the tone at 1000 Hz
}

void deactivateBuzzer() {
  noTone(BUZZER_PIN); // Stop the buzzer
}
