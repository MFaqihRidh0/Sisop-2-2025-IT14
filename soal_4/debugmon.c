#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define PROC_PATH "/proc"
#define LOGFILE "debugmon.log"

uid_t cari_uid(const char *nama_user) {
    struct passwd *pw = getpwnam(nama_user);
    if (!pw) {
        fprintf(stderr, "User '%s' tidak ditemukan.\n", nama_user);
        exit(EXIT_FAILURE);
    }
    return pw->pw_uid;
}

int hanya_angka(const char *str) {
    while (*str) {
        if (!isdigit(*str)) return 0;
        str++;
    }
    return 1;
}

void tulis_log(const char *proc_name, const char *status) {
    FILE *logfile = fopen(LOGFILE, "a");
    if (!logfile) return;

    time_t waktu = time(NULL);
    struct tm *waktu_local = localtime(&waktu);

    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "[%d:%m:%Y]-[%H:%M:%S]", waktu_local);

    fprintf(logfile, "%s_%s_%s\n", timestamp, proc_name, status);
    fclose(logfile);
}

void jadikan_daemon() {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    setsid();

    if (chdir("/home/mutiaradiva/soal_4/") < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_RDWR);
}

// Tambahkan parameter const char *mode_nama
void pantau_proses_user(const char *user, int mode_fail, const char *mode_nama) {
    uid_t uid_target = cari_uid(user);

    while (1) {
        DIR *proc_dir = opendir(PROC_PATH);
        if (!proc_dir) {
            sleep(5);
            continue;
        }

        struct dirent *entry;
        while ((entry = readdir(proc_dir)) != NULL) {
            if (!hanya_angka(entry->d_name)) continue;

            int pid = atoi(entry->d_name);
            char path[256], line[256], nama_proc[256] = "-";
            FILE *fp;
            uid_t uid_proses;

            snprintf(path, sizeof(path), PROC_PATH"/%d/status", pid);
            fp = fopen(path, "r");
            if (!fp) continue;

            while (fgets(line, sizeof(line), fp)) {
                if (sscanf(line, "Uid: %d", &uid_proses) == 1) break;
            }
            fclose(fp);

            if (uid_proses != uid_target) continue;

            snprintf(path, sizeof(path), PROC_PATH"/%d/comm", pid);
            fp = fopen(path, "r");
            if (fp) {
                fgets(nama_proc, sizeof(nama_proc), fp);
                nama_proc[strcspn(nama_proc, "\n")] = 0;
                fclose(fp);
            }

            if (strcmp(nama_proc, "debugmon") == 0) {
                tulis_log(mode_nama, "RUNNING");
            } else if (mode_fail) {
                if (kill(pid, SIGKILL) == 0) {
                    tulis_log(mode_nama, "FAILED");
                }
            }
        }
        closedir(proc_dir);
        sleep(5);
    }
}

// Tambahkan parameter const char *mode_nama
void hentikan_daemon(const char *user, const char *mode_nama) {
    DIR *proc_dir = opendir(PROC_PATH);
    if (!proc_dir) return;

    uid_t uid_target = cari_uid(user);
    struct dirent *entry;

    while ((entry = readdir(proc_dir)) != NULL) {
        if (!hanya_angka(entry->d_name)) continue;

        int pid = atoi(entry->d_name);
        char path[256], line[1024], exe[256];
        uid_t uid_proses;

        snprintf(path, sizeof(path), PROC_PATH"/%d/status", pid);
        FILE *fp = fopen(path, "r");
        if (!fp) continue;

        while (fgets(line, sizeof(line), fp)) {
            if (sscanf(line, "Uid: %d", &uid_proses) == 1) break;
        }
        fclose(fp);
        if (uid_proses != uid_target) continue;

        snprintf(path, sizeof(path), PROC_PATH"/%d/exe", pid);
        ssize_t len = readlink(path, exe, sizeof(exe) - 1);
        if (len == -1) continue;
        exe[len] = '\0';

        if (!strstr(exe, "debugmon")) continue;

        snprintf(path, sizeof(path), PROC_PATH"/%d/cmdline", pid);
        fp = fopen(path, "r");
        if (!fp) continue;
        size_t read_byte = fread(line, 1, sizeof(line) - 1, fp);
        fclose(fp);

        line[read_byte] = '\0';

        char *arg[10];
        int argc = 0;
        char *ptr = line;
        while (ptr < line + read_byte && argc < 10) {
            arg[argc++] = ptr;
            ptr += strlen(ptr) + 1;
        }

        if (argc >= 3 && strstr(arg[0], "debugmon") && strcmp(arg[2], user) == 0) {
            if (kill(pid, SIGTERM) == 0) {
                printf("Daemon debugmon (PID %d) untuk user '%s' dihentikan.\n", pid, user);
                tulis_log(mode_nama, "STOPPED");
            }
        }
    }
    closedir(proc_dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Pemakaian:\n");
        printf("  %s list <user>\n", argv[0]);
        printf("  %s daemon <user>\n", argv[0]);
        printf("  %s stop <user>\n", argv[0]);
        printf("  %s fail <user>\n", argv[0]);
        printf("  %s revert <user>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "list") == 0) {
        uid_t uid_target = cari_uid(argv[2]);
        DIR *proc_dir = opendir(PROC_PATH);
        if (!proc_dir) {
            perror("Gagal buka /proc");
            return EXIT_FAILURE;
        }

        struct dirent *entry;
        while ((entry = readdir(proc_dir)) != NULL) {
            if (!hanya_angka(entry->d_name)) continue;

            int pid = atoi(entry->d_name);
            char path[256], line[256], nama_proc[256] = "-";
            FILE *fp;
            uid_t uid_proses;
            double mem_kb = 0;

            snprintf(path, sizeof(path), PROC_PATH"/%d/status", pid);
            fp = fopen(path, "r");
            if (!fp) continue;

            while (fgets(line, sizeof(line), fp)) {
                if (sscanf(line, "Uid: %d", &uid_proses) == 1) break;
            }
            fclose(fp);
            if (uid_proses != uid_target) continue;

            snprintf(path, sizeof(path), PROC_PATH"/%d/comm", pid);
            fp = fopen(path, "r");
            if (fp) {
                fgets(nama_proc, sizeof(nama_proc), fp);
                nama_proc[strcspn(nama_proc, "\n")] = 0;
                fclose(fp);
            }

            snprintf(path, sizeof(path), PROC_PATH"/%d/statm", pid);
            fp = fopen(path, "r");
            if (fp) {
                long pages;
                if (fscanf(fp, "%ld", &pages) == 1) {
                    long page_kb = sysconf(_SC_PAGE_SIZE) / 1024;
                    mem_kb = pages * page_kb;
                }
                fclose(fp);
            }

            printf("PID: %-6d CMD: %-20s CPU: %.2f%% MEM: %.2f KB\n", pid, nama_proc, 0.0, mem_kb);
        }
        closedir(proc_dir);

        } else if (strcmp(argv[1], "daemon") == 0) {
            tulis_log("daemon-debugmon", "RUNNING");
            jadikan_daemon();
            pantau_proses_user(argv[2], 0, "daemon-debugmon");

        } else if (strcmp(argv[1], "fail") == 0) {
            tulis_log("fail-debugmon", "FAILED");
            jadikan_daemon();
            pantau_proses_user(argv[2], 1, "fail-debugmon");

        } else if (strcmp(argv[1], "stop") == 0) {
            tulis_log("stop-debugmon", "RUNNING");
            hentikan_daemon(argv[2], "stop-debugmon");

        } else if (strcmp(argv[1], "revert") == 0) {
            tulis_log("revert-debugmon", "RUNNING");
            hentikan_daemon(argv[2], "revert-debugmon");


    } else {
        printf("Perintah tidak dikenali.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
