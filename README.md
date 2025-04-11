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

# Action Program
Program **action.c** merupakan solusi yang menggabungkan beberapa mode operasi:
1. **Mode 1 (Tanpa Argumen):** Mengekstrak file *Clues.zip* jika folder *Clues* belum ada.
2. **Mode 2 (Filter):** Memindahkan file .txt yang memiliki nama satu digit atau satu huruf ke folder *Filtered* dan menghapus file .txt lain di folder *Clues*.
3. **Mode 3 (Combine):** Menggabungkan isi file dari folder *Filtered* ke dalam file *Combined.txt* secara bergantian (interleaving) antara file digit dan huruf, lalu menghapus file-file aslinya.
4. **Mode 4 (Decode):** Melakukan ROT13 pada file *Combined.txt* untuk menghasilkan file *Decoded.txt* tanpa menghapus *Combined.txt*.

---

#### A. Downloading the clues

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

```
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
```
#### Output

#### B. Filtering the files
## Mode 2: Filter File dari Folder Clues ke Folder Filtered

### Alur Eksekusi Mode 2
- **Pengecekan Folder Clues:**  
  Pastikan folder *Clues* sudah ada, karena file yang akan difilter berada di sana.
  
- **Pembuatan Folder Filtered:**  
  - Jika folder **Filtered** belum ada, program membuatnya menggunakan perintah `mkdir` dengan izin `0755`.
  
- **Memindahkan File yang Sesuai Kriteria:**  
  - Program memindahkan file-file yang memiliki nama **satu digit** dari folder Clues ke folder Filtered dengan menjalankan perintah:
    ```
    find Clues -type f \( -regex '.*/[0-9]\.txt' \) -exec mv {} Filtered/ \;
    ```
  - Demikian pula, file yang memiliki nama **satu huruf** dipindahkan menggunakan:
    ```
    find Clues -type f \( -regex '.*/[a-zA-Z]\.txt' \) -exec mv {} Filtered/ \;
    ```
  - Perintah `find` di sini:
    - **Mencari** file dengan tipe regular (`-type f`) yang cocok dengan pola *regex* yang ditentukan.
    - **Menjalankan perintah `mv`** untuk memindahkan file yang ditemukan ke folder **Filtered**.

- **Menghapus File yang Tidak Sesuai:**  
  - Semua file .txt di folder *Clues* yang tidak dipindahkan (yakni file yang tidak sesuai dengan kriteria satu digit/huruf) dihapus dengan perintah:
    ```
    find Clues -type f -name '*.txt' -exec rm {} \;
    ```
  - Tujuan penghapusan ini adalah untuk membersihkan file yang dianggap "tidak berguna" dari folder **Clues**.

### Fungsi/Perintah Pendukung dan Kegunaannya
- **`directory_exists(const char *path)`**  
  - **Kegunaan:** Memeriksa apakah folder *Clues* (dan nantinya *Filtered*) sudah ada sebelum melakukan operasi.
  - **Detail:**  
    - Menggunakan fungsi `stat()` dan makro `S_ISDIR` untuk memastikan path merupakan direktori.

- **`mkdir("Filtered", 0755)`**  
  - **Kegunaan:** Membuat folder *Filtered* jika folder tersebut belum ada.
  - **Detail:**  
    - Izin 0755 memastikan pemilik dapat membaca, menulis, dan menjalankan, sementara grup dan lainnya hanya dapat membaca dan menjalankan.

- **Perintah Shell `find` dengan `mv` dan `rm`:**  
  - **`find Clues -type f \( -regex '.*/[0-9]\.txt' \) -exec mv {} Filtered/ \;`**  
    - **Fungsi:** Mencari file dengan nama satu digit dan memindahkannya ke folder *Filtered*.
  - **`find Clues -type f \( -regex '.*/[a-zA-Z]\.txt' \) -exec mv {} Filtered/ \;`**  
    - **Fungsi:** Mencari file dengan nama satu huruf dan memindahkannya ke folder *Filtered*.
  - **`find Clues -type f -name '*.txt' -exec rm {} \;`**  
    - **Fungsi:** Menghapus file .txt lain yang tidak memenuhi kriteria agar folder *Clues* hanya berisi file-file yang tidak diperlukan.

### Flow Diagram Sederhana Mode 2
- **Mulai:** Jalankan program dengan argumen `-m Filter`
  - **Cek Folder Clues:** Pastikan folder *Clues* ada.
  - **Buat Folder Filtered:** Jika belum ada, buat folder *Filtered*.
  - **Pindahkan File Sesuai Kriteria:**  
    - Pindahkan file yang namanya satu digit ke folder *Filtered*.
    - Pindahkan file yang namanya satu huruf ke folder *Filtered*.
  - **Hapus File Lain:** Hapus semua file .txt lain di folder *Clues*.
  - **Selesai:** Cetak pesan "Proses filter selesai."

### Cuplikan Kode Mode 2 dari `action.c`

```
else if (argc == 3 && strcmp(argv[1], "-m") == 0 && strcmp(argv[2], "Filter") == 0) {
    if (!directory_exists("Clues")) {
        printf("Folder Clues tidak ditemukan. Tidak ada yang difilter.\n");
        return 0;
    }
    // Buat folder Filtered jika belum ada
    if (!directory_exists("Filtered")) {
        printf("Membuat folder Filtered...\n");
        if (mkdir("Filtered", 0755) != 0) {
            perror("Gagal membuat folder Filtered");
            return 1;
        }
    }
    // Pindahkan file .txt dengan nama 1 digit ke folder Filtered
    printf("Memindahkan file .txt yang namanya 1 digit ke folder Filtered...\n");
    if (system("find Clues -type f \\( -regex '.*/[0-9]\\.txt' \\) -exec mv {} Filtered/ \\;") != 0) {
        fprintf(stderr, "Gagal memindahkan file digit.\n");
    }
    // Pindahkan file .txt dengan nama 1 huruf ke folder Filtered
    printf("Memindahkan file .txt yang namanya 1 huruf ke folder Filtered...\n");
    if (system("find Clues -type f \\( -regex '.*/[a-zA-Z]\\.txt' \\) -exec mv {} Filtered/ \\;") != 0) {
        fprintf(stderr, "Gagal memindahkan file huruf.\n");
    }
    // Hapus file .txt lainnya yang tidak memenuhi kriteria
    printf("Menghapus semua file .txt lain yang tidak memenuhi kriteria...\n");
    if (system("find Clues -type f -name '*.txt' -exec rm {} \\;") != 0) {
        fprintf(stderr, "Gagal menghapus sisa file .txt.\n");
    }
    printf("Proses filter selesai.\n");
}
```
#### Output

#### C. Combine the files content
## Mode 3: Combine File dari Folder Filtered ke dalam Combined.txt

### Alur Eksekusi Mode 3
- **Pengecekan Folder Filtered:**  
  Program memastikan folder **Filtered** ada sebelum memulai proses penggabungan.

- **Pengambilan Daftar File:**  
  Program membuka folder **Filtered** dan membaca setiap entri.  
  - File dengan format nama 1 karakter (misalnya "3.txt", "b.txt") diidentifikasi dan dipisahkan dalam dua array:
    - Satu array untuk file yang namanya **digit**.
    - Satu array untuk file yang namanya **huruf**.

- **Sorting File:**  
  - **File Digit:** Diurutkan berdasarkan nilai angka (menggunakan fungsi **`compare_digit`**).
  - **File Huruf:** Diurutkan berdasarkan urutan ASCII (menggunakan fungsi **`compare_letter`**).

- **Penggabungan Isi File:**  
  Isi file-file yang telah diurutkan kemudian digabungkan ke **Combined.txt** secara interleaved (bergantian):
  - Pertama, file digit pertama dibaca dan isinya ditulis ke Combined.txt.
  - Kemudian, file huruf pertama dibaca dan ditulis.
  - Proses ini terus berlanjut hingga semua file terproses.

- **Penghapusan File Asli:**  
  Setelah isinya dipindahkan ke **Combined.txt**, file-file dari folder **Filtered** dihapus menggunakan fungsi `remove()`.

---

### Fungsi dan Perintah Pendukung yang Digunakan

- **`directory_exists(const char *path)`**
  - **Kegunaan:** Memastikan folder **Filtered** ada.
  - **Detail:** Mengecek dengan fungsi `stat` dan makro `S_ISDIR`.

- **Fungsi Comparator:**
  - **`compare_digit(const void *a, const void *b)`**
    - **Kegunaan:** Mengurutkan file-file yang namanya 1 digit dengan membandingkan karakter pertama dari nama file.
  - **`compare_letter(const void *a, const void *b)`**
    - **Kegunaan:** Mengurutkan file-file yang namanya 1 huruf secara ASCII.

- **Fungsi `qsort()`**
  - **Kegunaan:** Mengurutkan array nama file (baik untuk digit maupun huruf) sesuai dengan comparator yang telah didefinisikan.

- **Fungsi Standard I/O (`fopen`, `fgets`, `fputs`, `remove`)**
  - **Kegunaan:**  
    - **`fopen()`** digunakan untuk membuka file di folder **Filtered** dan file **Combined.txt**.
    - **`fgets()`** dan **`fputs()`** digunakan untuk membaca dan menulis isi file.
    - **`remove()`** digunakan untuk menghapus file asli setelah proses penggabungan.



### Flow Diagram Sederhana Mode 3
1. **Mulai:** Jalankan program dengan argumen `-m Combine`.
2. **Baca Folder Filtered:**
   - Ambil semua file yang sesuai (format `x.txt` dengan panjang 5 karakter).
   - Pisahkan menjadi dua kategori: file digit dan file huruf.
3. **Sorting:**
   - Urutkan file-digit menggunakan `compare_digit`.
   - Urutkan file-huruf menggunakan `compare_letter`.
4. **Interleaving:**
   - Baca file dari array digit dan huruf secara bergantian.
   - Tulis isinya ke **Combined.txt**.
5. **Hapus File Asli:**
   - Setelah ditulis, hapus file dari **Filtered**.
6. **Selesai:** Tampilkan pesan bahwa proses combine selesai.

---

### Cuplikan Kode Mode 3 dari `action.c`

```c
else if (argc == 3 && strcmp(argv[1], "-m") == 0 && strcmp(argv[2], "Combine") == 0) {
    if (!directory_exists("Filtered")) {
        printf("Folder Filtered tidak ditemukan. Tidak bisa melakukan Combine.\n");
        return 0;
    }
    DIR *dir;
    struct dirent *entry;
    dir = opendir("Filtered");
    if (!dir) {
        perror("Gagal membuka folder Filtered");
        return 1;
    }
    // Siapkan array untuk nama file
    char *digitFiles[100];
    char *letterFiles[100];
    int digitCount = 0, letterCount = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        size_t len = strlen(entry->d_name);
        if (len == 5 && strcmp(entry->d_name + len - 4, ".txt") == 0) {
            char first = entry->d_name[0];
            if (isdigit((unsigned char)first)) {
                digitFiles[digitCount] = (char *)malloc(len + 1);
                strcpy(digitFiles[digitCount], entry->d_name);
                digitCount++;
            } else if (isalpha((unsigned char)first)) {
                letterFiles[letterCount] = (char *)malloc(len + 1);
                strcpy(letterFiles[letterCount], entry->d_name);
                letterCount++;
            }
        }
    }
    closedir(dir);
    qsort(digitFiles, digitCount, sizeof(char *), compare_digit);
    qsort(letterFiles, letterCount, sizeof(char *), compare_letter);
    FILE *combined = fopen("Combined.txt", "w");
    if (!combined) {
        perror("Gagal membuat/menulis Combined.txt");
        return 1;
    }
    int i = 0, j = 0;
    while (i < digitCount || j < letterCount) {
        if (i < digitCount) {
            char path[256];
            snprintf(path, sizeof(path), "Filtered/%s", digitFiles[i]);
            FILE *f = fopen(path, "r");
            if (f) {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), f))
                    fputs(buffer, combined);
                fclose(f);
            }
            remove(path);
            i++;
        }
        if (j < letterCount) {
            char path[256];
            snprintf(path, sizeof(path), "Filtered/%s", letterFiles[j]);
            FILE *f = fopen(path, "r");
            if (f) {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), f))
                    fputs(buffer, combined);
                fclose(f);
            }
            remove(path);
            j++;
        }
    }
    fclose(combined);
    for (int idx = 0; idx < digitCount; idx++) {
        free(digitFiles[idx]);
    }
    for (int idx = 0; idx < letterCount; idx++) {
        free(letterFiles[idx]);
    }
    printf("Proses combine selesai. Hasil ada di Combined.txt\n");
}
```
#### Output

#### D. Decode the file

## Mode 4: Decode (ROT13 pada Combined.txt)

### Alur Eksekusi Mode 4
- Program membuka file **Combined.txt** dalam mode baca.
- Jika file **Combined.txt** tidak ditemukan, pesan akan ditampilkan dan proses berhenti.
- Program membuka file **Decoded.txt** dalam mode tulis.
- Isi **Combined.txt** diproses karakter demi karakter menggunakan algoritma ROT13:
  - Untuk setiap karakter, fungsi `rot13_char()` mengubah huruf (baik kecil maupun besar) dengan menambahkan 13 posisi dalam alfabet.
  - Karakter yang bukan huruf dikopi tanpa perubahan.
- Hasil proses ROT13 dituliskan ke **Decoded.txt**.
- Setelah seluruh isi file telah diproses, kedua file ditutup.
- File **Combined.txt** dipertahankan (tidak dihapus) untuk referensi.

### Fungsi dan Perintah Pendukung

- **`process_rot13(FILE *in, FILE *out)`**
  - **Kegunaan:**  
    - Membaca file input karakter demi karakter dan menuliskan hasil transformasi ROT13 ke file output.
  - **Detail Implementasi:**  
    - Menggunakan `fgetc()` untuk mendapatkan karakter dari file input.
    - Setiap karakter diproses melalui fungsi `rot13_char()`.
    - Hasilnya ditulis ke file output menggunakan `fputc()`.
  
- **`rot13_char(char c)`**
  - **Kegunaan:**  
    - Menerapkan transformasi ROT13 pada karakter input.
  - **Detail Implementasi:**  
    - Jika karakter adalah huruf kecil (`a-z`), dikonversi dengan rumus:  
      `('a' + ((c - 'a' + 13) % 26))`
    - Jika karakter adalah huruf besar (`A-Z`), dikonversi dengan rumus:  
      `('A' + ((c - 'A' + 13) % 26))`
    - Karakter non-huruf dikembalikan tanpa perubahan.

### Cuplikan Kode Mode 4 dari `action.c`

```
else if (argc == 3 && strcmp(argv[1], "-m") == 0 && strcmp(argv[2], "Decode") == 0) {
    FILE *combined = fopen("Combined.txt", "r");
    if (!combined) {
        printf("File Combined.txt tidak ditemukan. Tidak ada yang didecode.\n");
        return 0;
    }
    FILE *decoded = fopen("Decoded.txt", "w");
    if (!decoded) {
        perror("Gagal membuat/menulis Decoded.txt");
        fclose(combined);
        return 1;
    }
    printf("Melakukan decode ROT13 dari Combined.txt...\n");
    process_rot13(combined, decoded);
    fclose(combined);
    fclose(decoded);
    printf("Decoded.txt berhasil dibuat. File Combined.txt tetap dipertahankan.\n");
}
```

#### Output

#### E. Filtering the files
masukkan password ke website dengan password yaitu BewareOfAmpy

#### Output

# soal-2

# soal-3

# soal-4
