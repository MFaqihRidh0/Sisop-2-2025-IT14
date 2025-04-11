# LAPRES Praktikum Sistem Operasi Modul 2 - IT14

## Anggota
1. Muhammad Fatihul Qolbi Ash Shiddiqi (5027241023)
2. Mutiara Diva Jaladitha (5027241083)
3. M. Faqih Ridho (5027241123)

## DAFTAR ISI
- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# soal-1

**Dikerjakan oleh M. Faqih Ridho (5027241123)**

## Desktipsi Soal 

disuruh untuk mencari password dalam suatu file bernama Clue.zip untuk bisa masuk ke website. untuk menjalankannya kita disuruh untuk membuat file action.c yang berisi pemrograman untuk menjalankan perintah soal. soal 1a adalah unzip file Clue.zip dan hapus file Clue.zip . soal 1 B membuat filtering the files untuk memindahkan file-file yang hanya dinamakan dengan 1 huruf dan 1 angka tanpa special character kedalam folder bernama Filtered. 1.C isi folder Filtered yang berupa file .txt digabung ke dalam file COmbine.txt.  1. D. karena isi Combine.txt adalah string berpola kriptografi rot13 maka harus di decode. 1.E harus berhasil masukkan password ke website dari file Decode.txt. Soal 1 A-D proggramnya harus dimasukkan ke dalam file action.c dan dapat dipanggil masing-masing serta tambahan eror handling jika salah argument.

#### A. Downloading the clues
# Action Program
Program **action.c** merupakan solusi yang menggabungkan beberapa mode operasi:
1. **Mode 1 (Tanpa Argumen):** Mengekstrak file *Clues.zip* jika folder *Clues* belum ada.
2. **Mode 2 (Filter):** Memindahkan file .txt yang memiliki nama satu digit atau satu huruf ke folder *Filtered* dan menghapus file .txt lain di folder *Clues*.
3. **Mode 3 (Combine):** Menggabungkan isi file dari folder *Filtered* ke dalam file *Combined.txt* secara bergantian (interleaving) antara file digit dan huruf, lalu menghapus file-file aslinya.
4. **Mode 4 (Decode):** Melakukan ROT13 pada file *Combined.txt* untuk menghasilkan file *Decoded.txt* tanpa menghapus *Combined.txt*.

---

## Mode 1: Ekstraksi Clues.zip (Tanpa Argumen)

### Tujuan Mode 1
- **Cek Folder Clues:** Program mengecek apakah folder *Clues* sudah ada di direktori.
- **Ekstraksi:** Jika folder *Clues* tidak ada, file *Clues.zip* akan diekstrak sehingga menghasilkan folder *Clues* dan isinya.
- **Penghapusan File ZIP:** Setelah ekstraksi berhasil, file *Clues.zip* dihapus agar tidak memenuhi direktori.

### Alur Eksekusi Mode 1
- **Cek Keberadaan Folder:**  
  Menggunakan fungsi `directory_exists("Clues")` untuk mengetahui apakah folder *Clues* sudah ada.
- **Ekstraksi dan Penghapusan:**  
  Jika folder *Clues* belum ada, maka:
  - Menjalankan perintah `unzip Clues.zip` untuk mengekstrak file.
  - Menjalankan perintah `rm Clues.zip` untuk menghapus file ZIP setelah ekstraksi.

### Fungsi Pendukung yang Terkait

- **`directory_exists(const char *path)`**
  - **Kegunaan:**  
    - Mengecek apakah suatu *path* ada.
    - Memastikan bahwa *path* tersebut merupakan sebuah folder.
  - **Implementasi Inti:**
    - Memanfaatkan fungsi `stat()` untuk mengambil informasi status dari *path*.
    - Menggunakan makro `S_ISDIR` untuk memastikan *path* adalah direktori.

- **`system(const char *command)`**
  - **Kegunaan:**  
    - Menjalankan perintah shell langsung dari dalam program C.
  - **Penggunaan di Mode 1:**
    - Menjalankan `unzip Clues.zip` untuk mengekstrak file.
    - Menjalankan `rm Clues.zip` untuk menghapus file ZIP setelah ekstraksi.

### Flow Diagram Sederhana Mode 1
- **Mulai:** Program dijalankan tanpa argumen.
  - **Cek folder *Clues*:**  
    - **Jika ada:**  
      - Cetak pesan “Folder Clues sudah ada. Tidak mengekstrak apa pun.”
    - **Jika tidak ada:**  
      - Ekstrak *Clues.zip*.
      - Hapus *Clues.zip*.
      - Cetak pesan “Berhasil mengekstrak dan menghapus Clues.zip.”

---

## Contoh Kode (Mode 1)

Berikut cuplikan kode dari Mode 1 di file `action.c`:

```c
if (argc == 1) {
    if (directory_exists("Clues")) {
        printf("Folder Clues sudah ada. Tidak mengekstrak apa pun.\n");
    } else {
        printf("Folder Clues belum ada. Mengekstrak Clues.zip...\n");
        if (system("unzip Clues.zip") != 0) {
            fprintf(stderr, "Gagal mengekstrak Clues.zip.\n");
            return 1;
        }
        if (system("rm Clues.zip") != 0) {
            fprintf(stderr, "Gagal menghapus Clues.zip.\n");
            return 1;
        }
        printf("Berhasil mengekstrak dan menghapus Clues.zip.\n");
    }
}

#### Output

#### B. Filtering the files

#### Output

#### C. Combine the files content

#### Output

#### Decode the file

#### Output

#### E. Filtering the files
masukkan password ke website dengan password yaitu BewareOfAmpy

#### Output

# soal-2

# soal-3

# soal-4
