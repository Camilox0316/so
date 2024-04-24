#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 4096

void mostrar_arbol(char *directorio, int nivel);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <directorio>\n", argv[0]);
        return 1;
    }

    char ruta[MAX_PATH_LENGTH];
    if (realpath(argv[1], ruta) == NULL) {
        perror("realpath");
        return 1;
    }

    mostrar_arbol(ruta, 0);
    return 0;
}

void mostrar_arbol(char *directorio, int nivel) {
    DIR *dir;
    struct dirent *entrada;
    char ruta[MAX_PATH_LENGTH];
    struct stat stat_info;

    dir = opendir(directorio);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entrada = readdir(dir)) != NULL) {
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        snprintf(ruta, MAX_PATH_LENGTH, "%s/%s", directorio, entrada->d_name);

        if (lstat(ruta, &stat_info) == -1) {
            perror("lstat");
            continue;
        }

        for (int i = 0; i < nivel; i++) {
            printf("│   ");
        }

        if (S_ISDIR(stat_info.st_mode)) {
            printf("├── %s/\n", entrada->d_name);
            mostrar_arbol(ruta, nivel + 1);
        } else {
            printf("├── %s (%.1f KB, %s)\n", entrada->d_name, (double)stat_info.st_size / 1024, ctime(&stat_info.st_mtime));
        }
    }

    closedir(dir);
}
