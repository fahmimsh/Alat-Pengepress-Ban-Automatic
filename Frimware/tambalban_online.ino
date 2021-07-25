//program kang mustoffa
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Thermocouple.h>             //Library termokopel
#include <MAX6675_Thermocouple.h>     //Library MAX6675 

#define PB            2               //PUSH BUTTON
#define relay         8
#define buzz          3
#define pin_pwm       11              //pwm motor
#define pin_motor_a   12
#define pin_motor_b   13
#define SCK_PIN       6              //MAX6675
#define CS_PIN        5             //MAX6675
#define SO_PIN        4              //MAX6675

Thermocouple* thermocouple;           //Thermocouple
SSD1306AsciiAvrI2c display;           //lcd
//timer
unsigned long prevstart = 0;
unsigned long prevstop = 0;
unsigned long prevpanas = 0;
//sistem
int jalan = 0, stop_sys = 0;
int panas = 0, bus = 0;
//double celsius = 0;
int c = 0, d =0;

void Running_sys(){
  jalan = 1; bus = 1;
  prevstart = millis();
  display.clear();
  display.setCursor(0, 0);
  display.println("PRESS MOTOR");
}
void motor(bool arah, uint8_t pwm){
  analogWrite(pin_pwm, pwm);
  switch(arah){
    case 0:
      digitalWrite(pin_motor_b, HIGH);
      digitalWrite(pin_motor_a, LOW);
      break;
    case 1:
      digitalWrite(pin_motor_b, LOW);
      digitalWrite(pin_motor_a, HIGH);
      break;
  }
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(pin_motor_a, OUTPUT);
  pinMode(pin_motor_b, OUTPUT);
  pinMode(PB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PB), Running_sys, RISING);
  thermocouple = new MAX6675_Thermocouple(SCK_PIN, CS_PIN, SO_PIN);
  display.begin(&Adafruit128x64, 0x3C);
  display.setFont(System5x7);
  display.clear();
  display.set2X();
  display.setFont(Callibri11_bold);
    display.setCursor(0, 0);
    display.println("TAMBAL BAN");
    display.setCursor(10, 30);
    display.println(" ONLINE ");
    Serial.println("Bismillah berjalan");
    Serial.println("TEKAN TOMBOL PUSH BUTTON UNTUK MEMULAI");
    delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (jalan == 1){
    const double celsius = thermocouple->readCelsius();
    if ( d == 0){
      motor(0, 20);
      Serial.println();
    }
    digitalWrite(relay, HIGH);
    display.setCursor(0, 30);
    display.print("S: ");
    display.print(celsius,2);
    display.println(" C");
    Serial.print("SUHU TAMBAL : "); Serial.print(celsius); Serial.print(" C || ");
    unsigned long newtime = millis();
    if (panas == 0){
     if (newtime - prevstart >= 10000){
      Serial.println("Motor Stop");
      motor(0, 0);
      d = 1;
      panas = 1;
      display.setCursor(0, 0);
      display.println("MOTOR STOP");
     }
    }
    if (bus == 1){
      digitalWrite(buzz, HIGH);
      delay(500);
      digitalWrite(buzz, LOW); 
      bus = 0;
    }
    if (panas == 1 && celsius >= 60){
     if (c == 0){
      prevpanas = millis();
      prevstart = millis();
      c = 1;
     }
     display.clear();
     display.setCursor(0, 0);
     display.println("Mulai Proses");
     Serial.println("Mulai Proses Panas");
     display.println("Mulai Proses Panas");
     if (millis() - prevpanas >= 30000){ //3menit
        digitalWrite(relay, LOW);
        stop_sys = 1;
        motor(0, 20);
        delay(100);
        bus = 1;
        prevstop = millis();
        display.clear();
        display.setCursor(0, 0);
        display.println("Proses Selesai");
        Serial.println("Proses Selesai");
      }
      if (stop_sys == 1){
        if (millis() - prevstart >= 10000){
           motor(0, 0);
           display.setCursor(0, 0);
            display.println("TAMBAL BAN");
            display.setCursor(10, 30);
            display.println(" ONLINE ");
            Serial.println("Bismillah berjalan");
            Serial.println("TEKAN TOMBOL PUSH BUTTON UNTUK MEMULAI");
            jalan = 0; stop_sys = 0; panas = 0; d = 0; c = 0;
            delay(100);
      }
    }
   }
   delay(10);
 }
}
