#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 100

void openApps(int argc, char *argv[]);
void readConfigFile(char *filename);
void killApps(char *filename);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2>... or %s -f <file.conf> or %s -k [file.conf]\n", argv[0], argv[0], argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        openApps(argc - 2, argv + 2);
    } else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 3) {
            printf("Usage: %s -f <file.conf>\n", argv[0]);
            return 1;
        }
        readConfigFile(argv[2]);
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 2) {
            killApps(NULL);
        } else if (argc == 3) {
            killApps(argv[2]);
        } else {
            printf("Usage: %s -k [file.conf]\n", argv[0]);
            return 1;
        }
    } else {
        printf("Invalid argument: %s\n", argv[1]);
        return 1;
    }

    return 0;
}

void openApps(int argc, char *argv[]) {
    if (argc < 2 || argc % 2 != 0) {
        printf("Usage: %s -o <app1> <num1> <app2> <num2>...\n", argv[0]);
        return;
    }

    for (int i = 0; i < argc; i += 2) {
        char *app = argv[i];
        int num = atoi(argv[i + 1]);

        for (int j = 0; j < num; j++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app, app, NULL);
                printf("Failed to execute %s\n", app);
                exit(1);
            }
        }
    }
}

void readConfigFile(char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file %s\n", filename);
        return;
    }

    char line[MAX_ARGS];
    while (fgets(line, MAX_ARGS, file) != NULL) {
        char *args[MAX_ARGS];
        int argc = 0;
        char *token = strtok(line, " \n");
        while (token != NULL) {
            args[argc++] = token;
            token = strtok(NULL, " \n");
        }

        if (argc % 2 != 0) {
            printf("Invalid format in config file %s\n", filename);
            fclose(file);
            return;
        }

        openApps(argc, args);
    }

    fclose(file);
}

void killApps(char *filename) {
    if (filename == NULL) {
        system("killall setup");
    } else {
        char command[MAX_ARGS];
        snprintf(command, MAX_ARGS, "killall $(grep -o '^[^[:space:]]*' %s)", filename);
        system(command);
    }
}
