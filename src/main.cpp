#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_BMP280.h>

// Pin Chip Select para el BMP280 por SPI
#define BMP_CS 5

// Pantalla LCD 16x2 por I2C (0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Objeto BMP280 configurado para SPI por Hardware
Adafruit_BMP280 bmp(BMP_CS);

// --- CALIBRACIÓN PARA HABITACIÓN (MANIZALES) ---
const float OFFSET_TEMP = 6.25; // Resta 6.25 °C a la lectura bruta para ajustar a ~20 °C

unsigned long previoMillis = 0;
const long intervalo = 1000; // Actualiza cada 1 segundo

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Inicializar bus I2C para la Pantalla LCD
  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Inicializar BMP280 vía SPI por Hardware
  if (!bmp.begin()) {
    Serial.println("Error: No se encontro el sensor BMP280 mediante SPI.");
    lcd.setCursor(0, 0);
    lcd.print("Error BMP (SPI)!");
    while (1); // Detener si falla la comunicación SPI
  }

  // Configuración del sensor para mediciones estables
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);

  Serial.println("--- BMP280 POR SPI (MANIZALES) INICIADO ---");
}

void loop() {
  unsigned long actualMillis = millis();

  if (actualMillis - previoMillis >= intervalo) {
    previoMillis = actualMillis;

    // 1. Obtención de lecturas a través de bus SPI
    float tempBruta = bmp.readTemperature();
    float presAbsolutaHpa = bmp.readPressure() / 100.0F; // Convierte Pascales a hPa

    // 2. Aplicar calibración de temperatura
    float tempCalibrada = tempBruta - OFFSET_TEMP;

    // --- MONITOR SERIAL ---
    Serial.print("Temp (SPI): ");
    Serial.print(tempCalibrada, 1);
    Serial.print(" *C | Presion Manizales: ");
    Serial.print(presAbsolutaHpa, 1);
    Serial.println(" hPa");

    // --- PANTALLA LCD 16x2 ---
    lcd.setCursor(0, 0);
    lcd.print("Temp: ");
    lcd.print(tempCalibrada, 1);
    lcd.print(" C    ");

    lcd.setCursor(0, 1);
    lcd.print("Pres: ");
    lcd.print(presAbsolutaHpa, 1);
    lcd.print(" hPa  ");
  }
}