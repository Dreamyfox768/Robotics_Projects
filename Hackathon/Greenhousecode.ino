#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>   // OLED Display Library
#include <dht.h>                // DHT11 Temperature and Humidity Sensor Library

// ------------------------ OLED DISPLAY SETUP ------------------------
#define SCREEN_WIDTH 128        // OLED display width in pixels
#define SCREEN_HEIGHT 64        // OLED display height in pixels
#define OLED_RESET -1           // No reset pin used with I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ------------------------ DHT SENSOR SETUP --------------------------
#define DHT_PIN 2               // DHT11 sensor connected to digital pin 2
dht DHT;

// -------------------- SOIL MOISTURE SENSOR SETUP --------------------
const int SoilSensorPin = 2;   // Soil moisture sensor connected to pin 2
const int AirValue = 850;      // Dry soil value
const int WaterValue = 600;    // Wet soil value

// -------------------- WATER LEVEL SENSOR SETUP ----------------------
int waterSensorPin = A1;       // Water level sensor connected to analog pin A1

// ---------------------- HEARTBEAT SENSOR SETUP ----------------------
const int PULSE_SENSOR_PIN = A3;   // Pulse sensor connected to analog pin A3
int Signal;                        // Raw analog signal from pulse sensor
const int Threshold = 550;         // Threshold to detect beats

// -------------------------- SETUP FUNCTION --------------------------
void setup() {
  Serial.begin(9600);

  // Initialize OLED Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Stop if OLED is not detected
  }

  // Initial splash screen
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Green ");
  display.setCursor(0, 20);
  display.println("Heart");
  display.setCursor(0, 40);
  display.println("Monitoring");
  display.display();
  delay(5000);
  display.clearDisplay();

  // Setup for built-in LED (used with heartbeat sensor)
  pinMode(LED_BUILTIN, OUTPUT);
}

// --------------------------- MAIN LOOP ------------------------------
void loop() {
  // ------------------- Read DHT Sensor -------------------
  int readData = DHT.read11(DHT_PIN);
  float t = DHT.temperature;
  float h = DHT.humidity;

  // Log temperature and humidity to Serial Monitor
  Serial.print("Temperature = ");
  Serial.print(t);
  Serial.print("°C | ");
  Serial.print((t * 9.0) / 5.0 + 32.0);
  Serial.println("°F");
  Serial.print("Humidity = ");
  Serial.print(h);
  Serial.println("%\n");

  // ---------------- Read Water Level Sensor ----------------
  int waterLevel = analogRead(waterSensorPin);
  Serial.print("Water Level Raw Value: ");
  Serial.println(waterLevel);

  // ---------------- Read Brightness -----------------------
  int brightness = analogRead(A0);
  Serial.print("Brightness Value: ");
  Serial.println(brightness);

  // ---------------- Read Heartbeat Sensor ------------------
  Signal = analogRead(PULSE_SENSOR_PIN);
  Serial.print("Pulse Signal: ");
  Serial.println(Signal);

  if (Signal > Threshold) {
    digitalWrite(LED_BUILTIN, HIGH); // Beat detected
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // ------------------ Display on OLED ----------------------
  display.clearDisplay();

  // Labels (Left Side)
  display.setTextSize(1);
  display.setCursor(0, 0);    display.println("Temperature:");
  display.setCursor(0, 10);   display.println("Humidity:");
  display.setCursor(0, 20);   display.println("Water Level:");
  display.setCursor(0, 30);   display.println("Brightness:");
  display.setCursor(0, 40);   display.println("Heartbeat:");
  display.setCursor(0, 50);   display.println("GPS:");

  // Data (Right Side)
  display.setCursor(85, 0);   display.print(t);    display.println("C");
  display.setCursor(85, 10);  display.print(h);    display.println("%");

  display.setCursor(85, 20);
  if (waterLevel <= 200)
    display.println("LOW");
  else
    display.println("HIGH");

  display.setCursor(85, 30);  display.println(brightness);
  display.setCursor(85, 40);  display.println(Signal);   // Heartbeat raw value
  display.setCursor(50, 50);  display.println("XX YY ZZ"); // Dummy GPS coords

  display.display();

  delay(1000); // Refresh delay
}
