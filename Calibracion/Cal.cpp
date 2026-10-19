
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Configuracion de la pantalla LCD I2C 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Pin del potenciometro
const int potPin = 34;

// Hora inicial del reloj
int hora = 0;
int minuto = 0;
int segundo = 0;

// Control del tiempo
unsigned long previo = 0;
const unsigned long intervalo = 1000;

void setup() {
  Serial.begin(115200);

  // Configuracion I2C del ESP32
  Wire.begin(21, 22);

  // Inicializacion de la LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Resolucion ADC del ESP32: 12 bits (0 a 4095)
  analogReadResolution(12);

  Serial.println("Reloj digital y lectura del potenciometro");
}

void loop() {
  unsigned long actual = millis();

  // Actualizar el reloj cada segundo
  if (actual - previo >= intervalo) {
    previo += intervalo;
    segundo++;

    if (segundo >= 60) {
      segundo = 0;
      minuto++;

      if (minuto >= 60) {
        minuto = 0;
        hora++;

        if (hora >= 24) {
          hora = 0;
        }
      }
    }
  }

  // Leer el potenciometro
  int valor = analogRead(potPin);

  // Fila superior: reloj HH:MM:SS
  lcd.setCursor(4, 0);

  if (hora < 10) lcd.print('0');
  lcd.print(hora);
  lcd.print(':');

  if (minuto < 10) lcd.print('0');
  lcd.print(minuto);
  lcd.print(':');

  if (segundo < 10) lcd.print('0');
  lcd.print(segundo);

  // Fila inferior: potenciometro
  lcd.setCursor(0, 1);
  lcd.print("POT: ");
  lcd.print(valor);
  lcd.print("       "); // Borrar posibles caracteres sobrantes

  // Monitor serial
  Serial.printf("Hora: %02d:%02d:%02d | Potenciometro: %d\n",
                hora, minuto, segundo, valor);

  delay(100);
}
