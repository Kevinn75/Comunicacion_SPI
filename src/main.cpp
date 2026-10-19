#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>

// Configuración de Pines SPI para el BMP280
#define BMP_SCK  18
#define BMP_MISO 19
#define BMP_MOSI 23
#define BMP_CS   5

// Dirección I2C del LCD (0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_BMP280 bmp(BMP_CS);

// Ajuste de calibración para Manizales
const float OFFSET_TEMP = 6.25; 
unsigned long previoMillis = 0;
const long intervalo = 1000;

void setup() {
  Serial.begin(115200);
  delay(500);

  // 1. Inicialización de I2C estabilizado
  Wire.begin(21, 22);
  Wire.setClock(50000); // 50 kHz para evitar ruido en pantalla

  // 2. Configuración inicial del LCD
  lcd.init();
  delay(50);
  lcd.backlight();
  lcd.clear();

  // Imprimir etiquetas estáticas una sola vez
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(0, 1);
  lcd.print("Pres:");

  // 3. Inicialización de SPI Hardware para BMP280
  SPI.begin(BMP_SCK, BMP_MISO, BMP_MOSI, BMP_CS);

  if (!bmp.begin()) {
    Serial.println("Error: No se encontro el sensor BMP280 mediante SPI.");
    lcd.setCursor(6, 0);
    lcd.print("Error SPI!");
    while (1);
  }

  // Configuración de muestreo adecuada para altitud
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  Serial.println("--- ESTACIÓN METEOROLÓGICA MANIZALES INICIADA ---");
}

void loop() {
  unsigned long actualMillis = millis();

  if (actualMillis - previoMillis >= intervalo) {
    previoMillis = actualMillis;

    // Lectura y calibración
    float tempCalibrada = bmp.readTemperature() - OFFSET_TEMP;
    float presAbsolutaHpa = bmp.readPressure() / 100.0F;

    // Actualizar únicamente los valores en el LCD (evita parpadeo)
    lcd.setCursor(6, 0);
    lcd.print(tempCalibrada, 1);
    lcd.print(" C   ");

    lcd.setCursor(6, 1);
    lcd.print(presAbsolutaHpa, 1);
    lcd.print(" hPa ");

    // Salida Serial
    Serial.print("Temp: ");
    Serial.print(tempCalibrada, 1);
    Serial.print(" °C | Pres: ");
    Serial.print(presAbsolutaHpa, 1);
    Serial.println(" hPa");
  }
}