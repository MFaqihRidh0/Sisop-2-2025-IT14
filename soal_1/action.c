#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

// =====================
// Fungsi Pendukung Umum
// =====================

// Cek apakah sebuah direktori ada
int directory_exists(const char *path) {
    struct stat st;
    return (stat(path, &st) == 0 && S_ISDIR(st.st_mode));
}

// =====================
// Fungsi ROT13 untuk mode Decode
// =====================

char rot13_char(char c) {
    if (c >= 'a' && c <= 'z')
        return 'a' + ((c - 'a' + 13) % 26);
    else if (c >= 'A' && c <= 'Z')
        return 'A' + ((c - 'A' + 13) % 26);
    else
        return c;
}

void process_rot13(FILE *in, FILE *out) {
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        fputc(rot13_char((char)ch), out);
    }
}

// =====================
// Fungsi Comparator untuk mode Combine
// =====================

// Bandingkan file berdasarkan nilai digit (untuk file yang namanya 1 digit)
int compare_digit(const void *a, const void *b) {
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;
    int da = fa[0] - '0';
    int db = fb[0] - '0';
    return (da - db);
}

// Bandingkan file berdasarkan nilai ASCII (untuk file yang namanya 1 huruf)
int compare_letter(const void *a, const void *b) {
    const char *fa = *(const char **)a;
    const char *fb = *(const char **)b;
    return (fa[0] - fb[0]);
}

// =====================
// Fungsi untuk menampilkan penggunaan yang benar
// =====================
void print_usage(const char *prog_name) {
    printf("Penggunaan yang benar:\n");
    printf("  1) Tanpa argumen  : %s\n", prog_name);
    printf("  2) Filter         : %s -m Filter\n", prog_name);
    printf("  3) Combine        : %s -m Combine\n", prog_name);
    printf("  4) Decode         : %s -m Decode\n", prog_name);
}

// =====================
// Main Program
// =====================
int main(int argc, char *argv[]) {
    // -------------------------------------------------------------------
    // Mode 1: Tanpa argumen — Ekstrak Clues.zip jika folder Clues belum ada.
    // -------------------------------------------------------------------
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
    // -------------------------------------------------------------------
    // Mode 2: Filter — Pindahkan file .txt dengan nama 1 digit atau 1 huruf ke folder Filtered.
    // -------------------------------------------------------------------
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
        // Hapus file .txt lain yang tidak memenuhi kriteria
        printf("Menghapus semua file .txt lain yang tidak memenuhi kriteria...\n");
        if (system("find Clues -type f -name '*.txt' -exec rm {} \\;") != 0) {
            fprintf(stderr, "Gagal menghapus sisa file .txt.\n");
        }
        printf("Proses filter selesai.\n");
    }
    // -------------------------------------------------------------------
    // Mode 3: Combine — Gabungkan isi file dari folder Filtered ke Combined.txt secara bergantian.
    // -------------------------------------------------------------------
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
            // Abaikan entri "." dan ".."
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            size_t len = strlen(entry->d_name);
            // Format yang valid: misalnya "x.txt" (panjang 5 karakter)
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
        // Urutkan file-file tersebut
        qsort(digitFiles, digitCount, sizeof(char *), compare_digit);
        qsort(letterFiles, letterCount, sizeof(char *), compare_letter);
        // Gabungkan isi file ke dalam Combined.txt
        FILE *combined = fopen("Combined.txt", "w");
        if (!combined) {
            perror("Gagal membuat/menulis Combined.txt");
            return 1;
        }
        // Lakukan interleaving: file digit dulu, kemudian huruf, bergantian
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
                // Hapus file setelah isinya digabung
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
        // Bebaskan memori
        for (int idx = 0; idx < digitCount; idx++) {
            free(digitFiles[idx]);
        }
        for (int idx = 0; idx < letterCount; idx++) {
            free(letterFiles[idx]);
        }
        printf("Proses combine selesai. Hasil ada di Combined.txt\n");
    }
    // -------------------------------------------------------------------
    // Mode 4: Decode — Lakukan ROT13 pada Combined.txt lalu simpan ke Decoded.txt.
    // (File Combined.txt tidak dihapus.)
    // -------------------------------------------------------------------
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
    // -------------------------------------------------------------------
    // Error Handling: Argumen tidak valid
    // -------------------------------------------------------------------
    else {
        fprintf(stderr, "Error: Argumen yang diberikan tidak valid.\n");
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
