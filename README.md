# In-Memory Logistics Simulator (C++ CLI)

Simulator logistik berbasis CLI ini dibuat untuk keperluan laboratorium struktur data akademik. Program ini mensimulasikan manajemen gudang sortir (hub kota) dan perutean paket otomatis menggunakan kombinasi tiga struktur data dinamis murni berbasis pointer dinamis (tanpa library STL/`std::vector`/`std::queue`).

## Fitur Utama
1. **Queue (FIFO - Linked List Based)**: Merepresentasikan antrean gudang sortir di tiap hub kota.
2. **Directed Graph (Adjacency List)**: Merepresentasikan rute logistik antar-kota dengan bobot jarak (km).
3. **Linked List (Singly Linked List - Traceability)**: Riwayat lengkap perjalanan transit paket yang mencatat histori pergerakan secara dinamis.
4. **Algoritma Dijkstra Murni**: Menghitung rute terpendek antar kota secara dinamis menggunakan implementasi algoritma Dijkstra yang ditulis dari awal tanpa bantuan STL map/queue.
5. **Aman dari Kebocoran Memori (No Memory Leaks)**: Seluruh peminjaman alokasi memori dinamis dibebaskan secara rekursif/iteratif saat aplikasi ditutup.

---

## Struktur Direktori Proyek
```text
C:\Users\ACER\.gemini\antigravity\scratch\logistic_simulator\
├── main.cpp            # Kode program C++ utama
├── README.md           # Petunjuk instalasi dan eksekusi
└── laporan_analisis.md # Draf laporan analisis teoritis akademis lengkap
```

---

## Cara Kompilasi dan Eksekusi di Windows

Untuk menjalankan simulator ini, pastikan Anda memiliki compiler C++ (seperti `g++` dari MinGW/GCC) yang sudah dikonfigurasi dalam PATH Windows.

### 1. Membuka Terminal
Buka PowerShell atau Command Prompt (cmd) dan arahkan ke folder proyek ini:
```powershell
cd "C:\Users\ACER\.gemini\antigravity\scratch\logistic_simulator"
```

### 2. Kompilasi Kode Program
Jalankan perintah berikut untuk mengompilasi file `main.cpp` menjadi aplikasi eksekutabel:
```powershell
g++ -std=c++11 main.cpp -o simulator.exe
```

### 3. Menjalankan Aplikasi
Jalankan program hasil kompilasi:
```powershell
.\simulator.exe
```

---

## Panduan Penggunaan Menu Simulator

Saat program dijalankan, program akan terisi otomatis dengan data demonstrasi bawaan (`RESI-101`, `RESI-102`, `RESI-103`) agar Anda dapat langsung melakukan uji coba.

1. **Menu 1 (Tampilkan Peta Hub)**: Menampilkan kota-kota aktif beserta daftar rute logistik yang tersedia dan bobotnya (km).
2. **Menu 2 (Status Antrean Hub)**: Menampilkan antrean paket yang sedang menunggu di gudang sortir masing-masing kota secara detail.
3. **Menu 3 (Registrasi Paket Baru)**: Mendaftarkan paket dengan memasukkan nomor resi unik, kota asal, kota tujuan, dan waktu pendaftaran. Paket otomatis masuk ke antrean kota asal.
4. **Menu 4 (Proses Dispatch Hub)**: Memproses pengiriman paket dari suatu kota. Sistem akan mengeluarkan semua paket dari antrean hub kota tersebut, menghitung rute terpendek ke tujuan masing-masing menggunakan algoritma Dijkstra, kemudian mengirimkannya ke kota tujuan/transit berikutnya.
5. **Menu 5 (Lacak Paket / Traceback)**: Memasukkan nomor resi paket untuk menampilkan tabel histori transit perjalanan yang direkam oleh Linked List secara berurutan.
