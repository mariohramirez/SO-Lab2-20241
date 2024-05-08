/*
    * wish_utils.c
    *
    * Se encarga de la ejecución de los comandos internos de wish 
    * y de la redirección de la salida estándar y de errores.
    * 
    * Autores: Mario Ramirez y Andres Ramirez Aristizabal
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "wish_utils.h"

char error_message2[30] = "An error has occurred\n";

/*
    * Función que se encarga de ejecutar el comando exit.
    * 
    * Recibe un puntero a un array de caracteres que contiene los argumentos del comando.
    * 
    * Si el comando exit recibe argumentos, imprime un mensaje de error.
    * Si el comando exit no recibe argumentos, termina la ejecución del programa.
    * 
    * No retorna nada.
*/
void execute_exit(char *args) {
    char *path = strtok_r(args, " ", &args);
    if (path != NULL) {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
    } else {
        exit(0);
    }
}

/*
    * Función que se encarga de ejecutar el comando cd.
    * 
    * Recibe un puntero a un array de caracteres que contiene el nuevo directorio al que se desea cambiar.
    * 
    * Si el comando cd recibe más de un argumento, imprime un mensaje de error.
    * Si el comando cd recibe un argumento, cambia el directorio de trabajo al directorio especificado.
    * Si el directorio especificado no existe, imprime un mensaje de error.
    * 
    * No retorna nada.
*/
void execute_cd(char *newpath) {
    char *path = strtok_r(newpath, " ", &newpath);
<<<<<<< HEAD
    if (path == NULL) {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }
    if (strtok_r(NULL, " ", &newpath) != NULL) {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }
    if (access(path, F_OK) == 0) {
        chdir(path);
    } else {
=======
    if (path == NULL)
    {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }

    if (strtok_r(NULL, " ", &newpath) != NULL)
    {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
        return;
    }

    if (access(path, F_OK) == 0)
    {
        chdir(path);
    }
    else
    {
>>>>>>> 9683ebd4f0af740acc7fae267221f23e3f45f957
        write(STDERR_FILENO, error_message2, strlen(error_message2));
    }
}

/*
    * Función que se encarga de procesar una adena de rutas y actualizar las rutas actuales.
    * 
    * Recibe un puntero a un array de caracteres que contiene las rutas que se van a procesar
    * Recibe un puntero a un puntero a un array de caracteres que contiene las rutas actuales
    * 
    * Si el comando path recibe un argumento vacío, se eliminan todos los directorios de búsqueda actuales.
    * Si el comando path recibe un argumento, se actualizan los directorios de búsqueda actuales.
    * 
    * No retorna nada.
*/
void execute_path(char *newpath, char ***mypath) {
    char *copypath = strdup(newpath);
    int path_count = 0;
    int i = 0;
    char *path = strtok_r(copypath, " ", &copypath);
    while (path != NULL) {
        path_count++;
        path = strtok_r(copypath, " ", &copypath);
    }
<<<<<<< HEAD
    if (path_count == 0) {
        i = 0;
        while (strcmp((*mypath)[i], "") != 0) {
            (*mypath)[i] = strdup("");
            i++;
        }
    } else {
=======

    if (path_count == 0)
    {
        i = 0;
        while (strcmp((*mypath)[i], "") != 0)
        {
            (*mypath)[i] = strdup("");
            i++;
        }    
    }
    else
    {
>>>>>>> 9683ebd4f0af740acc7fae267221f23e3f45f957
        *mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));
        path = strtok_r(newpath, " ", &newpath);
        i = 0;
<<<<<<< HEAD
        while (path != NULL) {
            if (strcmp(path, "bin") == 0 || strcmp(path, "/bin") == 0 || strcmp(path, "/bin/") == 0 || strcmp(path, "./bin") == 0 || strcmp(path, "./bin/") == 0) {
                (*mypath)[i] = strdup("/bin/");
            } else {
                if (strstr(path, "./") != path) {
=======
        while (path != NULL)
        {
            if (strcmp(path, "bin") == 0 || strcmp(path, "/bin") == 0 || strcmp(path, "/bin/") == 0 || strcmp(path, "./bin") == 0 || strcmp(path, "./bin/") == 0)
            {
                (*mypath)[i] = strdup("/bin/");
            }
            else
            {
                if (strstr(path, "./") != path)
                {
>>>>>>> 9683ebd4f0af740acc7fae267221f23e3f45f957
                    char *new_path = malloc(strlen(path) + 3);
                    strcpy(new_path, "./");
                    strcat(new_path, path);
                    (*mypath)[i] = new_path;
<<<<<<< HEAD
                } else {
                    (*mypath)[i] = strdup(path);
                }
                if (path[strlen(path) - 1] != '/') {
=======
                }
                else
                {
                    (*mypath)[i] = strdup(path);
                }
                if (path[strlen(path) - 1] != '/')
                {
>>>>>>> 9683ebd4f0af740acc7fae267221f23e3f45f957
                    strcat((*mypath)[i], "/");
                }
            }
            path = strtok_r(newpath, " ", &newpath);
            i++;
        }
        (*mypath)[i] = "";
    }
}

/*
    * Función que se encarga de ejecutar un comando con redirección de la salida estándar y de errores.
    * 
    * Recibe un array que contiene los argumentos del comando.
    * Recibe una cadena  de caracteres que representa el nombre del archivo al que se redirigira.
    * 
    * Si el comando no se puede ejecutar, imprime un mensaje de error.
    * Si el comando se ejecuta correctamente, redirige la salida estándar y de errores al archivo especificado.
    * 
    * Retorna 1 si el comando se ejecutó correctamente.
*/
int wish_launch_redirect(char **args, char *file) {
    pid_t pid, wpid;
    int status;
    pid = fork();
    if (pid == 0) {
        int fd = open(file, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
<<<<<<< HEAD
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
=======

        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);

>>>>>>> 9683ebd4f0af740acc7fae267221f23e3f45f957
        close(fd);
        execv(args[0], args);
    } else if (pid < 0) {
        write(STDERR_FILENO, error_message2, strlen(error_message2));
    } else {
        wait(NULL);
    }
    return 1;
}
