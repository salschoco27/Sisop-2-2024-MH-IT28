# SOAL SHIFT SISTEM OPERASI MODUL 2
Kelompok IT 28:
- Salsabila Rahmah (5027271005)
- Fadlillah Cantika Sari H (5027271042)
- I Dewa Made Satya Raditya (5027271051)
## Soal 1

### virus.c
Define librarynya masing masing
```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define INTERVAL_SECONDS 15
#define LOG_FILE "virus.log"
```

Fungsi untuk replace string bernama str
```c
// untuk replace string
char *replace_substring(char *str, const char *sub, const char *replace) {
    char *pos = strstr(str, sub);
    if (pos == NULL) return str; // Substring not found

    size_t sub_len = strlen(sub);
    size_t replace_len = strlen(replace);
    size_t str_len = strlen(str);
    
    if (sub_len != replace_len) {
        memmove(pos + replace_len, pos + sub_len, str_len - (pos - str + sub_len) + 1);
    }
    memcpy(pos, replace, replace_len);
    
    return str;
}
```

Fungsi untuk replace suspicious string yang akan di-declare dibawah.
```c
//replace string yang sudah ditentukan
void process_file(const char *path) {
    const char *suspicious_strings[] = {"m4LwAr3", "5pYw4R3", "R4nS0mWaR3"};
    const char *replacements[] = {"[MALWARE]", "[SPYWARE]", "[RANSOMWARE]"};
    const int num_strings = sizeof(suspicious_strings) / sizeof(suspicious_strings[0]);

    char buffer[1024]; // buffer
    FILE *file = fopen(path, "r+");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    int replaced = 0;
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        for (int i = 0; i < num_strings; ++i) {
            char *new_str = replace_substring(buffer, suspicious_strings[i], replacements[i]);
            if (new_str != buffer) {
                fseek(file, -strlen(buffer), SEEK_CUR); 
                fputs(new_str, file); 
                replaced = 1;
             
                time_t now;
                struct tm *local_time = localtime(&now);
                char timestamp[20];
		time(&now);
                strftime(timestamp, sizeof(timestamp), "[%d-%m-%Y][%H:%M:%S]", local_time);

		FILE *log_file = fopen(LOG_FILE, "a");
		if (log_file == NULL){
		perror("Can't open log file."); return;}

                    fprintf(log_file, "%s Suspicious string at %s successfully replaced!\n", timestamp, path);
                    fclose(log_file);
                break;
            }
        }
    }

    fclose(file);
}
```
Untuk menjalankan program pada semua file yang ada dalam satu folder sesuai path yang diinputkan.
```c
//menjalankan program di semua file dalam suatu folder
void process_directory(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char file_path[PATH_MAX];
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);
            process_file(file_path);
        }
    }

    closedir(dir);
}
```
Fungsi Main
```c
int main(int argc, char *argv[]) {
//membuat virus.log
	FILE  *log_file = fopen(LOG_FILE, "a");
	if (log_file == NULL){
	perror("Error");
	return EXIT_FAILURE;
	}
	fclose(log_file);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]);
        return 1;
    }

    //Implementasi Daemon
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        perror("Failed to fork");
        return 1;
    }
    if (pid > 0) {
        return 0; // Parent process exits
    }

    umask(0);
    if (setsid() < 0) {
        perror("Process failed.");
        return 1;
    }

    if (chdir("/") < 0) {
        perror("Process failed.");
        return 1;
    }


    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    open("/dev/null", O_RDONLY);
    open("/dev/null", O_WRONLY);
    open("/dev/null", O_WRONLY);

    while (1) {
        process_directory(argv[1]);
        sleep(INTERVAL_SECONDS);
    }

    return 0;
}
```
**Command untuk menjalankan program**
1. Membuat folder baru (sisop2), lalu membuat file txt yang berisi text yang sudah diberikan.
2. Berpindah ke folder yang berisi program virus.c
3. Menjalankan ```gcc -o virus virus.c```
4. Menjalankan ```./virus.c```
5. Menjalankan ```./virus.c /home/salsa/sisop2```
6. Cek apakah file virus.log sudah muncul dengan menjakankan ```ls```
7. Jika virus.log sudah muncul maka jalankan ```cat virus.log``` untuk menampilkan isi dari file tersebut (harusnya berisi status dari string yang berhasil diubah)
8. Berpindah ke folder sisop2 lalu jalankan ```cat virus.txt``` (nama file saya).

**Dokumentasi**
![image](https://github.com/salschoco27/Sisop-2-2024-MH-IT28/assets/151063684/057ac65b-3bd3-478e-b556-b99b14ea9f9c)
Error:
- _PuPuPu_ terulang terus
- Ketika saya coba 2 code dalam satu file.txt, berhasil merubah MALWARE dan RANSOMWARE
- Dalam code ini, hanya RANSOMWARE yang terubah.
- Status yang ada dalam virus.log tidak muncul, status tidak masuk kedalam virus.log

## Soal 3

### admin.c

1. Buat file admin.c yang berisi Program yang akan menampilkan seluruh proses yang dilakukan oleh seorang user. Program akan berjalan secara daemon. 
```
nano admin.c
```
2. Isi dari file admin.c memuat program C 
```
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

    return 0;
}
```
3. Kompilasi file admin.c dan pastikan ada yang error atau tidak
   ```
   gcc admin.c -o admin
   ```
4. Cek apakar file admin.c sudah ada atau belum
   ```
   ls admin.c
   ```
5. Buat file baru bernama aca.log
   ```
   touch aca.log
   ```
6. Cek apakah file aca.log sudah ada atau belum
   ```
   ls aca.log
   ```
7. Jalankan Program
   ```
   ./admin -m aca
   ```
   mematikan fitur
   ```
   ./admin -s aca
   ```
8. 

