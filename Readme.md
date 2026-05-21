# 6.5 Pertanyaan Praktikum
## 1. Jelaskan proses bagaimana tombol dapat mengubah kondisi LED menggunakan interrupt! 
Ketika tombol ditekan, sinyal pada pin 2 Arduino berubah dari HIGH menjadi LOW karena konfigurasi INPUT_PULLUP yang digunakan. Perubahan sinyal ini dideteksi oleh hardware interrupt controller pada Arduino Uno sebagai kondisi FALLING edge. Begitu kondisi tersebut terdeteksi, mikrokontroler secara otomatis menghentikan sementara eksekusi program utama di `loop()`, lalu melompat ke fungsi ISR bernama `tombolInterrupt()`. Di dalam ISR tersebut, nilai variabel `ledState` di-toggle (dibalik nilainya). Setelah ISR selesai dieksekusi, program utama dilanjutkan kembali, dan karena `loop()` terus membaca nilai `ledState` lalu menuliskannya ke pin 13 via `digitalWrite()`, maka kondisi LED pun ikut berubah sesuai nilai terbaru `ledState`.

---

## 2. Apa fungsi `attachInterrupt()` pada program tersebut? 
Fungsi `attachInterrupt()` berfungsi untuk mendaftarkan atau menghubungkan sebuah pin interrupt dengan fungsi ISR tertentu beserta kondisi pemicunya. Pada program ini, `attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, FALLING)` berarti: Arduino diperintahkan untuk memantau pin 2, dan setiap kali terdeteksi perubahan sinyal dari HIGH ke LOW (FALLING edge), maka fungsi `tombolInterrupt()` akan otomatis dipanggil oleh hardware. Tanpa `attachInterrupt()`, Arduino tidak akan mengetahui bahwa pin 2 harus dipantau dan fungsi ISR tidak akan pernah terpanggil meski tombol ditekan sekalipun.

---

## 3. Mengapa pada ISR tidak disarankan menggunakan `delay()` dan `Serial.print()`? 
ISR dirancang untuk dieksekusi secepat mungkin karena selama ISR berjalan, program utama sepenuhnya dihentikan dan interrupt lain berpotensi diabaikan. Fungsi `delay()` bekerja dengan cara memblokir eksekusi selama durasi tertentu menggunakan mekanisme timer internal, namun timer tersebut justru bisa terganggu ketika berada di dalam konteks interrupt, sehingga `delay()` tidak akan bekerja dengan benar dan malah menyebabkan sistem hang. Sementara `Serial.print()` merupakan operasi yang membutuhkan waktu relatif lama karena melibatkan komunikasi serial dengan buffer, dan fungsi ini juga bergantung pada interrupt internal UART yang tidak dapat dipanggil ulang saat berada di dalam ISR. Jika keduanya dipaksakan digunakan dalam ISR, sistem bisa menjadi tidak stabil, tidak responsif, atau bahkan crash.

---

## 4. Apa fungsi keyword `volatile` pada variabel `ledState`?
Keyword `volatile` memberitahu compiler bahwa nilai variabel tersebut dapat berubah kapan saja dari luar alur normal program, yaitu dari dalam ISR. Tanpa `volatile`, compiler optimasi (terutama pada level -O2 atau lebih) berpotensi melakukan optimasi agresif seperti menyimpan nilai variabel di register CPU dan tidak membacanya ulang dari memori setiap kali digunakan di `loop()`. Akibatnya, perubahan nilai `ledState` yang terjadi di dalam ISR bisa tidak terdeteksi oleh program utama. Dengan mendeklarasikan `volatile bool ledState`, compiler dipaksa selalu membaca nilai variabel langsung dari memori RAM setiap kali diakses, sehingga perubahan yang dilakukan ISR selalu terbaca dengan benar oleh `loop()`.

---

## 5. Pada percobaan digunakan mode interrupt FALLING. Modifikasikan program menggunakan mode interrupt lain (RISING, CHANGE, atau LOW) kemudian: 
- Jelaskan perbedaan cara kerja masing-masing mode interrupt tersebut
- Analisis perubahan perilaku LED yang terjadi pada setiap mode
- Sertakan source code dan penjelasan program dalam bentuk README.md

Mode FALLING memicu interrupt saat sinyal berubah dari HIGH ke LOW, yaitu tepat saat tombol ditekan (karena INPUT_PULLUP membuat kondisi normal adalah HIGH). Mode RISING adalah kebalikannya, interrupt dipicu saat sinyal berubah dari LOW ke HIGH, artinya LED baru akan toggle ketika tombol dilepaskan, bukan saat ditekan. Hal ini menyebabkan sedikit keterlambatan respons yang terasa tidak natural bagi pengguna. Mode CHANGE memicu interrupt pada kedua transisi, baik dari HIGH ke LOW maupun dari LOW ke HIGH, sehingga LED akan toggle dua kali dalam satu siklus tekan-lepas tombol, yang membuat LED tampak seperti tidak berubah kondisinya secara konsisten. Mode LOW memicu interrupt secara terus-menerus selama sinyal pada pin bernilai LOW (selama tombol ditekan), sehingga ISR akan dipanggil berkali-kali secara cepat dan LED akan berkedip sangat cepat atau berperilaku tidak terduga selama tombol ditahan.
Berikut source code modifikasi untuk masing-masing mode beserta README.md:

``` cpp
// mode_rising.ino
// Interrupt dipicu saat tombol DILEPAS (sinyal LOW -> HIGH)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle LED saat tombol dilepas
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, RISING);
}

void loop() {
  digitalWrite(13, ledState);
}
```
```cpp
// mode_change.ino
// Interrupt dipicu saat sinyal BERUBAH (tekan DAN lepas)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle setiap ada perubahan sinyal
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, CHANGE);
}

void loop() {
  digitalWrite(13, ledState);
}
```
```cpp
// mode_low.ino
// Interrupt dipicu TERUS-MENERUS selama sinyal LOW (tombol ditekan)
#include <Arduino.h>

volatile bool ledState = false;

void tombolInterrupt() {
  ledState = !ledState; // Toggle terus selama tombol ditahan
}

void setup() {
  pinMode(13, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), tombolInterrupt, LOW);
}

void loop() {
  digitalWrite(13, ledState);
}
```

---

# 6.6 Pertanyaan Praktikum
## 1. Jelaskan bagaimana fungsi `millis()` bekerja pada program tersebut! 
Fungsi `millis()` mengembalikan jumlah milidetik yang telah berlalu sejak Arduino pertama kali dinyalakan atau di-reset, dan nilai ini terus bertambah secara otomatis di background menggunakan Timer0 internal Arduino. Pada program tersebut, setiap kali `loop()` dieksekusi, nilai `millis()` saat itu disimpan ke variabel `currentMillis`. Kemudian dilakukan pengecekan: apakah selisih antara `currentMillis` dan `previousMillis` sudah mencapai atau melebihi nilai interval (1000 ms). Jika ya, berarti sudah 1 detik berlalu, maka `previousMillis` diperbarui dengan waktu sekarang, `ledState` di-toggle, dan perubahan ditulis ke pin LED. Jika belum 1 detik, program melewati blok if tersebut dan kembali ke awal `loop()` tanpa melakukan apa-apa, sehingga tidak ada waktu yang terbuang.

---

## 2. Apa perbedaan utama antara `delay()` dan `millis()`?
Perbedaan utamanya terletak pada cara keduanya menangani waktu tunggu. Fungsi `delay(1000)` bersifat blocking, artinya ia menghentikan total eksekusi seluruh program selama 1000 milidetik penuh. Selama periode itu, tidak ada kode lain yang bisa dijalankan, tidak ada input yang bisa dibaca, dan sistem benar-benar "beku" sementara. Sebaliknya, `millis()` hanya digunakan sebagai penanda waktu untuk dibandingkan, bukan untuk menghentikan program. Program tetap terus berjalan di setiap iterasi `loop()`, hanya saja aksi tertentu (seperti toggle LED) baru dilakukan ketika kondisi waktu terpenuhi. Ini membuat `millis()` jauh lebih fleksibel dan cocok untuk sistem yang perlu menangani banyak tugas sekaligus.

---

## 3. Mengapa metode `millis()` disebut non-blocking? 
Disebut non-blocking karena penggunaannya tidak pernah menghentikan atau memblokir alur eksekusi program. Ketika kondisi `if(currentMillis - previousMillis >= interval)` belum terpenuhi, program langsung melewatinya dan melanjutkan eksekusi ke baris berikutnya tanpa menunggu. Program utama terus berputar di `loop()` dengan kecepatan penuh, dan hanya melakukan aksi spesifik (toggle LED) ketika sudah waktunya. Ini berbeda dengan `delay()` yang secara aktif memblokir CPU. Dengan pendekatan non-blocking ini, satu program `loop()` bisa memantau banyak timer sekaligus hanya dengan menambahkan variabel `previousMillis` dan `interval` yang berbeda untuk masing-masing tugas.

---

## 4. Modifikasi program agar: 
- LED pertama berkedip setiap 1 detik
- LED kedua berkedip setiap 500 ms
- Tanpa menggunakan delay() 
## Berikan penjelasan setiap baris program dalam bentuk README.md.
```cpp
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
```

---

# 6.7 Pertanyaan Analisis
## 1. Sebutkan dan jelaskan keuntungan menggunakan interrupt dibanding polling! 
Polling mengharuskan mikrokontroler terus-menerus memeriksa kondisi suatu input dalam setiap iterasi loop, meskipun tidak ada kejadian yang terjadi. Ini sangat boros CPU karena sebagian besar waktu pemeriksaan dilakukan sia-sia. Interrupt memberikan beberapa keuntungan signifikan: pertama, efisiensi CPU jauh lebih tinggi karena mikrokontroler baru "terbangun" dan merespons hanya ketika benar-benar ada kejadian, sehingga waktu prosesor bisa digunakan untuk tugas lain. Kedua, respons jauh lebih cepat dan deterministik karena ISR langsung dipanggil begitu event terjadi, tanpa harus menunggu giliran pemeriksaan dalam loop. Ketiga, konsumsi daya bisa lebih rendah karena pada sistem dengan sleep mode, mikrokontroler bisa dibangunkan oleh interrupt sehingga tidak perlu terus aktif melakukan polling. Keempat, kode program menjadi lebih bersih dan modular karena penanganan event dipisah dari logika utama.

---

## 2. Mengapa timer penting dalam sistem embedded dan real-time? 
Timer adalah tulang punggung dari hampir semua sistem embedded dan real-time karena waktu adalah dimensi kritis dalam sistem-sistem tersebut. Dalam sistem real-time, tugas-tugas harus dieksekusi tepat pada waktunya, tidak boleh terlambat atau terlalu cepat, dan timer-lah yang memungkinkan penjadwalan presisi tersebut. Timer digunakan untuk menghasilkan sinyal PWM guna mengontrol kecepatan motor atau kecerahan LED, mengukur durasi antar event (seperti jarak dari sensor ultrasonik), membuat scheduler sederhana agar beberapa tugas dapat dijalankan secara bergantian sesuai slot waktu, serta menghasilkan timeout untuk mendeteksi kegagalan komunikasi. Tanpa timer, sistem embedded hanya bisa bekerja secara sekuensial berdasarkan urutan instruksi tanpa kemampuan mengukur atau menjadwalkan berdasarkan waktu nyata.

---

## 3. Jika interrupt dan timer digabung dalam satu sistem, bagaimana alur kerja sistem tersebut? 
Ketika interrupt dan timer digabungkan dalam satu sistem, keduanya bekerja secara paralel dan saling melengkapi. Alur kerjanya adalah sebagai berikut: program utama di `loop()` berjalan normal mengerjakan tugas-tugas reguler. Timer berjalan di background secara independen, memantau waktu yang berlalu menggunakan `millis()` atau melalui timer interrupt periodik, dan menjalankan aksi berbasis waktu seperti kedip LED atau pembacaan sensor berkala. Secara terpisah, external interrupt memantau pin input dan siap dipicu kapan saja oleh event eksternal seperti tombol ditekan. Ketika interrupt eksternal terjadi, program utama dihentikan sementara, ISR dijalankan dengan cepat, kemudian program utama dilanjutkan. Timer tetap berjalan akurat karena menggunakan hardware counter, sehingga penghitungan waktu tidak terganggu meski sempat ada interrupt. Hasil akhirnya adalah sistem yang mampu merespons event real-time secara instan sekaligus menjalankan tugas-tugas terjadwal secara akurat dan konsisten.

---

## 4. Apa yang terjadi jika ISR terlalu panjang atau kompleks?
Jika ISR dibuat terlalu panjang atau kompleks, berbagai masalah serius dapat terjadi. Pertama, program utama akan terhenti dalam waktu yang lama karena ISR memblokir eksekusi `loop()`, sehingga sistem terasa tidak responsif. Kedua, interrupt lain yang terjadi selama ISR sedang berjalan bisa terlewat atau tertunda, menyebabkan hilangnya event penting. Ketiga, jika ISR berisi operasi seperti `delay()` atau `Serial.print()` yang bergantung pada interrupt internal Arduino, sistem bisa mengalami deadlock karena interrupt yang dibutuhkan oleh fungsi-fungsi tersebut tidak bisa terpanggil sementara ISR sedang berjalan. Keempat, jika ada banyak interrupt yang saling berdekatan waktunya, tumpukan ISR yang belum selesai dapat menyebabkan stack overflow dan crash. Oleh karena itu, prinsip utama penulisan ISR yang baik adalah: masuk, lakukan sesedikit mungkin (biasanya hanya set flag atau update variabel), lalu keluar secepat mungkin. Pemrosesan lebih lanjut berdasarkan flag tersebut sebaiknya dilakukan di `loop()`.
