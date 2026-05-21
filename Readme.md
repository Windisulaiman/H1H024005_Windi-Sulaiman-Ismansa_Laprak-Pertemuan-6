# ⚡ Modul 6 : Interrupt dan Timer
---

# 📚 SOAL DAN JAWABAN MODUL 6 — Interrupt dan Timer

Praktikum Sistem Mikrokontroler  
Arduino Uno — External Interrupt & Timer (`millis()`)

---

# 🔔 Percobaan 6A : External Interrupt

## 📌 Pertanyaan Praktikum 6A

---

###  Soal 1  
Bagaimana tombol dapat mengubah kondisi LED menggunakan interrupt?

### ✅ Jawaban

Ketika tombol ditekan, sinyal pada Pin 2 berubah dari HIGH menjadi LOW karena menggunakan konfigurasi `INPUT_PULLUP`. Perubahan ini memicu external interrupt (`INT0`) pada Arduino Uno.

Saat interrupt terjadi:

1. Program utama dihentikan sementara
2. Fungsi ISR (`tombolInterrupt()`) dijalankan
3. Variabel `ledState` di-toggle
4. Program utama dilanjutkan kembali
5. Nilai `ledState` ditulis ke LED menggunakan `digitalWrite()`

Dengan mekanisme ini, LED dapat berubah kondisi tanpa polling tombol secara terus-menerus.

---

###  Soal 2  
Apa fungsi `attachInterrupt()` pada program?

### ✅ Jawaban

Fungsi `attachInterrupt()` digunakan untuk menghubungkan pin interrupt dengan fungsi ISR (Interrupt Service Routine).

Contoh:

```cpp
attachInterrupt(
  digitalPinToInterrupt(2),
  tombolInterrupt,
  FALLING
);
```

Penjelasan parameter:

- `digitalPinToInterrupt(2)` → mengubah Pin 2 menjadi interrupt
- `tombolInterrupt` → fungsi ISR yang dijalankan
- `FALLING` → interrupt aktif saat sinyal HIGH → LOW

Tanpa fungsi ini, Arduino tidak mengetahui ISR mana yang harus dipanggil ketika interrupt terjadi.

---

###  Soal 3  
Mengapa `delay()` dan `Serial.print()` tidak disarankan dalam ISR?

### ✅ Jawaban

ISR harus berjalan sangat cepat dan singkat.

#### 🔹 `delay()`
Fungsi `delay()` menggunakan Timer0 interrupt. Saat ISR berjalan, interrupt lain dinonaktifkan sehingga `delay()` dapat menyebabkan sistem hang.

#### 🔹 `Serial.print()`
`Serial.print()` menggunakan interrupt UART. Jika dipanggil di ISR dapat menyebabkan deadlock atau corrupt data.

Karena itu ISR hanya disarankan untuk operasi ringan seperti:
- mengubah variabel
- toggle status
- set flag

---

###  Soal 4  
Apa fungsi keyword `volatile` pada variabel `ledState`?

### ✅ Jawaban

Keyword `volatile` memberi tahu compiler bahwa nilai variabel dapat berubah sewaktu-waktu di luar alur normal program, yaitu di dalam ISR.

Contoh:

```cpp
volatile bool ledState = false;
```

Tanpa `volatile`, compiler bisa menyimpan nilai variabel di register CPU sehingga perubahan dari ISR tidak terbaca.

Dengan `volatile`, compiler selalu membaca nilai terbaru langsung dari RAM.

---

###  Soal 5  
Pada percobaan digunakan mode interrupt `FALLING`. Modifikasikan menggunakan mode interrupt lain (`RISING`, `CHANGE`, atau `LOW`) kemudian jelaskan perbedaannya!

---

# 🔹 Mode FALLING

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, FALLING);
```

### Cara Kerja
Interrupt aktif saat sinyal berubah HIGH → LOW.

### Perilaku LED
LED toggle saat tombol ditekan.

### Analisis
Paling cocok digunakan untuk push button.

---

# 🔹 Mode RISING

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);
```

### Cara Kerja
Interrupt aktif saat sinyal LOW → HIGH.

### Perilaku LED
LED berubah saat tombol dilepas.

### Analisis
Respon sedikit terlambat dibanding FALLING.

---

# 🔹 Mode CHANGE

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);
```

### Cara Kerja
Interrupt aktif setiap perubahan sinyal.

### Perilaku LED
LED toggle dua kali:
- saat tombol ditekan
- saat tombol dilepas

### Analisis
LED bisa terlihat tidak berubah karena toggle terjadi dua kali sangat cepat.

---

# 🔹 Mode LOW

```cpp
attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, LOW);
```

### Cara Kerja
Interrupt aktif terus selama pin LOW.

### Perilaku LED
LED berkedip sangat cepat selama tombol ditekan.

### Analisis
Kurang cocok untuk toggle sederhana.

---

# ⏱️ Percobaan 6B : Timer Menggunakan millis()

---

##  Pertanyaan Praktikum 6B

---

###  Soal 1  
Bagaimana fungsi `millis()` bekerja pada program?

### ✅ Jawaban

Fungsi `millis()` mengembalikan waktu sejak Arduino menyala dalam satuan milidetik.

Program bekerja dengan:

1. Membaca waktu saat ini menggunakan `millis()`
2. Menghitung selisih waktu dengan `previousMillis`
3. Jika selisih ≥ interval:
   - LED di-toggle
   - `previousMillis` diperbarui

Contoh:

```cpp
if(currentMillis - previousMillis >= interval)
```

Metode ini memungkinkan LED berkedip tanpa menghentikan program utama.

---

###  Soal 2  
Apa perbedaan utama antara `delay()` dan `millis()`?

### ✅ Jawaban

| delay() | millis() |
|---|---|
| Blocking | Non-blocking |
| Program berhenti sementara | Program tetap berjalan |
| Tidak responsif | Tetap responsif |
| Tidak cocok multitasking | Cocok multitasking sederhana |

Contoh:

```cpp
delay(1000);
```

CPU berhenti selama 1 detik.

Sedangkan:

```cpp
if(millis() - previousMillis >= interval)
```

Program tetap berjalan sambil mengecek waktu.

---

###  Soal 3  
Mengapa metode `millis()` disebut non-blocking?

### ✅ Jawaban

Karena `millis()` tidak menghentikan eksekusi program.

Program tetap menjalankan seluruh kode dalam `loop()` sambil terus mengecek kondisi waktu.

Keuntungannya:
- sistem tetap responsif
- dapat membaca sensor
- dapat menerima input
- multitasking sederhana

---

###  Soal 4  
Modifikasi program agar:
- LED pertama berkedip setiap 1 detik
- LED kedua berkedip setiap 500 ms
- Tanpa menggunakan `delay()`

### ✅ Jawaban

```cpp
#include <Arduino.h>

// LED 1
unsigned long previousMillis1 = 0;
const long interval1 = 1000;
bool ledState1 = false;

// LED 2
unsigned long previousMillis2 = 0;
const long interval2 = 500;
bool ledState2 = false;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
}

void loop() {

  unsigned long currentMillis = millis();

  // LED 1
  if(currentMillis - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis;
    ledState1 = !ledState1;
    digitalWrite(13, ledState1);
  }

  // LED 2
  if(currentMillis - previousMillis2 >= interval2) {
    previousMillis2 = currentMillis;
    ledState2 = !ledState2;
    digitalWrite(12, ledState2);
  }
}
```

### Analisis Program

- LED pertama berkedip setiap 1000 ms
- LED kedua berkedip setiap 500 ms
- Kedua LED dapat berjalan bersamaan tanpa `delay()`
- Implementasi multitasking sederhana menggunakan `millis()`

---

# 🧩 Pertanyaan Praktikum Umum

---

###  Soal 1  
Sebutkan dan jelaskan keuntungan menggunakan interrupt dibanding polling!

### ✅ Jawaban

### 🔹 Efisiensi CPU
CPU hanya bekerja saat event terjadi.

### 🔹 Respons Lebih Cepat
Interrupt diproses hampir seketika.

### 🔹 Hemat Daya
CPU dapat sleep dan aktif saat interrupt terjadi.

### 🔹 Cocok untuk Sistem Real-Time
Event penting tidak mudah terlewat.

---

###  Soal 2  
Mengapa timer penting dalam sistem embedded dan real-time?

### ✅ Jawaban

Timer digunakan untuk:
- PWM
- pengukuran waktu
- scheduler
- interrupt periodik
- delay non-blocking

Dalam sistem real-time, ketepatan waktu sangat penting sehingga timer menjadi komponen utama.

---

###  Soal 3  
Jika interrupt dan timer digabung dalam satu sistem, bagaimana alur kerja sistem tersebut?

### ✅ Jawaban

1. Program utama berjalan di `loop()`
2. Timer (`millis()`) menjalankan tugas periodik
3. Ketika event eksternal terjadi:
   - interrupt aktif
   - ISR dijalankan
4. Setelah ISR selesai:
   - program utama dilanjutkan kembali

Keduanya bekerja saling melengkapi:
- timer → tugas periodik
- interrupt → event asinkron

---

###  Soal 4  
Apa yang terjadi jika ISR terlalu panjang atau kompleks?

### ✅ Jawaban

ISR yang terlalu panjang dapat menyebabkan:

- program utama tertunda
- sistem menjadi lambat
- interrupt lain terlewat
- missed interrupt
- stack overflow

Karena itu ISR harus:
- singkat
- ringan
- efisien

---

#  Kesimpulan

Praktikum Modul 6 berhasil menunjukkan implementasi:

- External Interrupt menggunakan push button
- Timer non-blocking menggunakan `millis()`

Interrupt memungkinkan sistem merespons event secara cepat tanpa polling, sedangkan `millis()` memungkinkan multitasking sederhana tanpa menghentikan program utama.

Kombinasi keduanya menjadi fondasi penting dalam pengembangan embedded system yang:
- responsif
- efisien
- real-time
- multitasking sederhana

---
