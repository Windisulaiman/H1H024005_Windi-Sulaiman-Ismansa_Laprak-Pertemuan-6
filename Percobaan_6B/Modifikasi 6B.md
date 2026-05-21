````cpp
#include <Arduino.h>

// ============================================================
// PIN ASSIGNMENT
// ============================================================
const int PIN_LED1 = 13;   // LED pertama
const int PIN_LED2 = 12;   // LED kedua

// ============================================================
// TIMER LED 1 – interval 1000 ms (1 detik)
// ============================================================
unsigned long prevMillis1 = 0;
const long interval1      = 1000;
bool ledState1            = false;

// ============================================================
// TIMER LED 2 – interval 500 ms (0.5 detik)
// ============================================================
unsigned long prevMillis2 = 0;
const long interval2      = 500;
bool ledState2            = false;

void setup() {
  pinMode(PIN_LED1, OUTPUT);  // Pin 13 sebagai output untuk LED 1
  pinMode(PIN_LED2, OUTPUT);  // Pin 12 sebagai output untuk LED 2
}

void loop() {
  // Ambil waktu saat ini sekali, dipakai bersama oleh kedua timer
  // agar tidak terjadi perbedaan pembacaan antar timer
  unsigned long now = millis();

  // ----------------------------------------------------------
  // TIMER LED 1 – cek setiap iterasi loop, tanpa memblokir
  // ----------------------------------------------------------
  if (now - prevMillis1 >= interval1) {
    prevMillis1 = now;           // catat waktu terakhir toggle
    ledState1   = !ledState1;    // balik kondisi LED 1
    digitalWrite(PIN_LED1, ledState1);
  }

  // ----------------------------------------------------------
  // TIMER LED 2 – berjalan independen dari LED 1
  // Kedua timer bekerja serentak karena tidak ada delay()
  // ----------------------------------------------------------
  if (now - prevMillis2 >= interval2) {
    prevMillis2 = now;           // catat waktu terakhir toggle
    ledState2   = !ledState2;    // balik kondisi LED 2
    digitalWrite(PIN_LED2, ledState2);
  }

  // Di sini masih bisa ditambah task lain (baca sensor, dsb.)
  // karena loop() tidak pernah terblokir
}
````
