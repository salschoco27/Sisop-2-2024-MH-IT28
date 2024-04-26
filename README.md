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

### hehe.c
