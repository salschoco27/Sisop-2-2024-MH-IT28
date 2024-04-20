#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>

#define LOG_FILE_EXTENSION ".log"
#define LOG_ENTRY_MAX_LEN 100

void log_process(char *username, pid_t pid, char *process_name, int status) {
    time_t current_time;
    struct tm *tm_info;
    char log_filename[] = "aca.log";
    char log_entry[LOG_ENTRY_MAX_LEN];
    FILE *log_file;

    time(&current_time);
    tm_info = localtime(&current_time);

    sprintf(log_filename, "%s%s", username, LOG_FILE_EXTENSION); // Perbaikan di sini
    log_file = fopen(log_filename, "a");
    if (log_file == NULL) {
 perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    strftime(log_entry, LOG_ENTRY_MAX_LEN, "[%d:%m:%Y]-[%H:%M:%S]-", tm_info);
    sprintf(log_entry + strlen(log_entry), "pid_%d-%s_%s\n", pid, process_name, (status == 0) ? "JALAN" : "GAGAL");
    fprintf(log_file, "%s", log_entry);

    fclose(log_file);
}

void monitor_process(char *username) {
    char command[256];
    sprintf(command, "ps -u %s", username);
    FILE *fp = popen(command, "r");
    if (fp == NULL) {
        perror("Error opening pipe");
        exit(EXIT_FAILURE);
    }

    
    char buffer[1024];
    printf("Memantau proses untuk pengguna %s:\n", username);
 while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    printf("%s", buffer); // Cetak output ke terminal

        // Memperoleh PID dan nama proses dari buffer
        pid_t pid;
        char process_name[256];
        int status;

        // Contoh cara mendapatkan PID dan nama proses
        sscanf(buffer, "%d %s", &pid, process_name);

        // Contoh cara menentukan status berdasarkan kondisi tertentu
        // Misalnya, jika nama proses adalah "my_process", statusnya adalah JALAN
        // Jika tidak, statusnya adalah GAGAL
        if (strcmp(process_name, "my_process") == 0) {
            status = 0; // JALAN
        } else {
            status = 1; // GAGAL
        }

        // Memanggil fungsi log_process
        printf("Mencatat proses ke file log...\n");
        log_process(username, pid, process_name, status);
        printf("Proses berhasil dicatat ke file log.\n");
    }
 pclose(fp);
}

void block_processes(char *username) {
    char command[256];
    sprintf(command, "pkill -u %s", username);
    system(command);
}

void unblock_processes(char *username) {
    char command[256];
    sprintf(command, "pkill -u %s -CONT", username);
    system(command);
}

void signal_handler(int signal) {
    // Do nothing
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Penggunaan: %s <opsi> <username>\n", argv[0]);
        printf("Opsi:\n");
        printf("-m : Memulai pemantauan proses pengguna\n");
        printf("-s : Menghentikan pemantauan proses pengguna\n");
        printf("-c : Memblokir proses pengguna\n");
 printf("-a : Membuka blokir proses pengguna\n");
        exit(EXIT_FAILURE);
    }

    char *option = argv[1];
    char *username = argv[2];

    if (strcmp(option, "-m") == 0) {
        // Memulai pemantauan
        signal(SIGCHLD, signal_handler);
        daemon(0, 0);
        while (1) {
            monitor_process(username);
            sleep(5); // Sesuaikan interval sesuai kebutuhan
        }
    } else if (strcmp(option, "-s") == 0) {
        // Menghentikan pemantauan
        // Tidak perlu tindakan, proses akan dihentikan secara eksternal
    } else if (strcmp(option, "-c") == 0) {
        // Memblokir proses
        block_processes(username);
    } else if (strcmp(option, "-a") == 0) {
        // Membuka blokir proses
        unblock_processes(username);
    } else {
        printf("Opsi tidak valid\n");
 exit(EXIT_FAILURE);
    }

    return 0;
}
