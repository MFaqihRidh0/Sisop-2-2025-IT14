#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#define QUARANTINE_DIR "./quarantine"
#define STARTERKIT_DIR "./starter_kit"
#define LOG_FILE "activity.log"
#define ZIP_FILE "starter_kit.zip"
#define FILE_ID "1_5GxIGfQr3mNKuavJbte_AoRkEQLXSKS"

void log_activity(const char *type, const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d-%02d-%04d][%02d:%02d:%02d] - ",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec);

    if (strcmp(type, "decrypt") == 0)
        fprintf(log, "Successfully started decryption process with PID %s.\n", message);
    else if (strcmp(type, "shutdown") == 0)
        fprintf(log, "Successfully shut off decryption process with PID %s.\n", message);
    else if (strcmp(type, "quarantine") == 0)
        fprintf(log, "%s - Successfully moved to quarantine directory.\n", message);
    else if (strcmp(type, "return") == 0)
        fprintf(log, "%s - Successfully returned to starter kit directory.\n", message);
    else if (strcmp(type, "eradicate") == 0)
        fprintf(log, "%s - Successfully deleted.\n", message);
    else if (strcmp(type, "decode") == 0)
        fprintf(log, "Decoded: %s\n", message);
    else
        fprintf(log, "%s\n", message);

    fclose(log);
}

char *base64_decode(const char *input) {
    BIO *bio, *b64;
    int input_len = strlen(input);

    // Alokasi buffer decode (Base64 decode bisa lebih pendek dari input)
    int max_decoded_len = (input_len * 3) / 4;
    char *temp = malloc(max_decoded_len + 1);
    memset(temp, 0, max_decoded_len + 1);

    bio = BIO_new_mem_buf(input, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    int decoded_len = BIO_read(bio, temp, max_decoded_len);
    temp[decoded_len] = '\0';

    BIO_free_all(bio);

    // Buat buffer akhir tanpa \n atau \r
    char *cleaned = malloc(decoded_len + 1);
    int j = 0;
    for (int i = 0; i < decoded_len; i++) {
        if (temp[i] != '\n' && temp[i] != '\r') {
            cleaned[j++] = temp[i];
        }
    }
    cleaned[j] = '\0';

    free(temp);
    return cleaned;
}


void decrypt_filenames() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) return;

    struct dirent *entry;
    char oldpath[512], newpath[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;

        snprintf(oldpath, sizeof(oldpath), "%s/%s", QUARANTINE_DIR, entry->d_name);
        char *decoded = base64_decode(entry->d_name);

        if (decoded && strlen(decoded) > 0) {
            snprintf(newpath, sizeof(newpath), "%s/%s", QUARANTINE_DIR, decoded);
            if (rename(oldpath, newpath) == 0) log_activity("decode", decoded);
            free(decoded);
        }
    }

    closedir(dir);
}

void download_and_extract() {
    pid_t pid;

    if ((pid = fork()) == 0) {
        char *argv[] = {"wget", "--no-check-certificate",
                        "https://docs.google.com/uc?export=download&id=" FILE_ID,
                        "-O", ZIP_FILE, NULL};
        execvp("wget", argv);
        exit(1);
    }
    waitpid(pid, NULL, 0);

    if ((pid = fork()) == 0) {
        char *argv[] = {"unzip", "-o", ZIP_FILE, "-d", STARTERKIT_DIR, NULL};
        execvp("unzip", argv);
        exit(1);
    }
    waitpid(pid, NULL, 0);

    if ((pid = fork()) == 0) {
        char *argv[] = {"rm", "-f", ZIP_FILE, NULL};
        execvp("rm", argv);
        exit(1);
    }
    waitpid(pid, NULL, 0);

    log_activity("info", "Downloaded and extracted starterkit.");
}

int move_file(const char *from, const char *to) {
    return rename(from, to);
}

void quarantine_files() {
    DIR *dir = opendir(STARTERKIT_DIR);
    if (!dir) return;

    struct dirent *entry;
    char from[512], to[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(from, sizeof(from), "%s/%s", STARTERKIT_DIR, entry->d_name);
        snprintf(to, sizeof(to), "%s/%s", QUARANTINE_DIR, entry->d_name);

        if (move_file(from, to) == 0)
            log_activity("quarantine", entry->d_name);
    }

    closedir(dir);
}

void clean_starterkit_dir() {
    DIR *dir = opendir(STARTERKIT_DIR);
    if (!dir) return;

    struct dirent *entry;
    char path[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(path, sizeof(path), "%s/%s", STARTERKIT_DIR, entry->d_name);
        remove(path);
    }

    closedir(dir);
}

void return_files() {
    clean_starterkit_dir(); // Bersihkan starter_kit terlebih dahulu

    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) return;

    struct dirent *entry;
    char from[512], to[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(from, sizeof(from), "%s/%s", QUARANTINE_DIR, entry->d_name);
        snprintf(to, sizeof(to), "%s/%s", STARTERKIT_DIR, entry->d_name);

        if (move_file(from, to) == 0)
            log_activity("return", entry->d_name);
    }

    closedir(dir);
}

void eradicate_quarantine() {
    DIR *dir = opendir(QUARANTINE_DIR);
    if (!dir) return;

    struct dirent *entry;
    char path[512];

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') continue;
        snprintf(path, sizeof(path), "%s/%s", QUARANTINE_DIR, entry->d_name);

        if (remove(path) == 0)
            log_activity("eradicate", entry->d_name);
    }

    closedir(dir);
}

void start_decryption() {
    pid_t pid = fork();
    if (pid < 0) exit(1);

    if (pid > 0) {
        printf("Daemon started with PID %d\n", pid);
        exit(0);
    }

    setsid();
    char pid_str[16];
    snprintf(pid_str, sizeof(pid_str), "%d", getpid());
    log_activity("decrypt", pid_str);

    while (1) {
        decrypt_filenames();
        sleep(10);
    }
}

void shutdown_daemon() {
    FILE *fp = popen("ps aux | grep './starterkit --decrypt' | grep -v grep", "r");
    if (!fp) return;

    char buf[512];
    int pid, count = 0;

    while (fgets(buf, sizeof(buf), fp)) {
        if (sscanf(buf, "%*s %d", &pid) == 1) {
            if (kill(pid, SIGTERM) == 0) {
                char pid_str[16];
                snprintf(pid_str, sizeof(pid_str), "%d", pid);
                log_activity("shutdown", pid_str);
                count++;
            }
        }
    }

    pclose(fp);
    if (count == 0)
        printf("No active decryption processes found.\n");
    else
        printf("Shutdown %d daemon process(es).\n", count);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Invalid usage.\n");
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s --decrypt\n", argv[0]);
        fprintf(stderr, "  %s --shutdown\n", argv[0]);
        fprintf(stderr, "  %s --quarantine\n", argv[0]);
        fprintf(stderr, "  %s --return\n", argv[0]);
        fprintf(stderr, "  %s --eradicate\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "--decrypt") == 0 ||
        strcmp(argv[1], "--quarantine") == 0 ||
        strcmp(argv[1], "--eradicate") == 0 ||
        strcmp(argv[1], "--shutdown") == 0 ||
        strcmp(argv[1], "--return") == 0) {
        download_and_extract();
    }

    if (strcmp(argv[1], "--decrypt") == 0)
        start_decryption();
    else if (strcmp(argv[1], "--shutdown") == 0)
        shutdown_daemon();
    else if (strcmp(argv[1], "--quarantine") == 0)
        quarantine_files();
    else if (strcmp(argv[1], "--return") == 0)
        return_files();
    else if (strcmp(argv[1], "--eradicate") == 0)
        eradicate_quarantine();
    else {
        fprintf(stderr, "[ERROR] Unknown option: %s\n", argv[1]);
        return 1;
    }

    return 0;
}
