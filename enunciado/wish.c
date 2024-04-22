//Inclusion de librerias

//FUnciones de entrada y salida
#include <stdio.h>
//Funciones de manejo de memoria
#include <stdlib.h>
//Funciones de manejo de strings
#include <string.h>
//Funciones de manejo de procesos
#include <unistd.h>
//Funciones de manejo de señales
#include <sys/wait.h>
//Funciones de control de descriptores de archivos
#include <fcntl.h>
//Funciones de manejo de archivos
#include <sys/stat.h>
//Funciones de manejo de directorios
#include <libgen.h>
//Funciones para leer lineas de entrada de forma interactiva
#include <readline/readline.h>
//Funciones para leer lineas de entrada de forma interactiva con historial
#include <readline/history.h>

// Definicion de constantes
//Longitud maxima de una linea de entrada de conmando
#define MAX_LINE_LENGTH 1024
//Numero maximo de argumentos que se pueden pasar en un comando
#define MAX_ARGS 64
//Modos de ejecucion, modo batch o en lote se establece en 0, los comando son leidos de un archivo
//Modo interactivo se establece en 1, los comandos son leidos de la entrada estandar
#define MODE_BATCH 0
#define MODE_INTERACTIVE 1
//Guardar el modo de ejecucion
int MODE;

//Se definen funciones
void execute_command(char *args[], int background);
void execute_exit(char *args);
void execute_cd(char *newpath);
void interactive_mode();
void batch_mode(char *batch_file);


//Se crea un proceso para la ejecucion del comandoingresado por el usuario
//Recibe un arreglo de strings con los argumentos del comando
//Recibe un entero que indica si el comando se debe ejecutar en segundo plano
void execute_command(char *args[], int background) {
    //Se hace uso de la funcion fork para crear un proceso hijo, se guarda el ID del proceso hijo
    pid_t pid = fork();

    //Verificar si el fork fue exitoso
    if (pid < 0) {
        //Imprimir un mensaje de error y salir
        fprintf(stderr, "Fork failed\n");
        exit(1);
    }
    //si el ID del proceso hijo es 0, se ejecuta el comando en el proceso hijo 
    else if (pid == 0) {
        // Se usa la funcion execvp para ejecutar el comando en args[0]
        // y se pasan los argumentos en args
        // Si la funcion retorna un valor menor a 0, se imprime un mensaje de error
        if (execvp(args[0], args) < 0) {
            fprintf(stderr, "An error has occurred\n");
            exit(1);
        }
    } else {
        // Si el valor es distinto de 0, se 
        if (!background) {
            waitpid(pid, NULL, 0); // Wait for child process to finish
        }
    }
}

// Implementación del comando exit para salir del programa
void execute_exit(char *args)
{
    //Divide la cadena de argumentos en tokens separados por espacio
    //Usa la funcion strtok_r para evitar problemas de concurrencia
	char *path = strtok_r(args, " ", &args);
    //Verifica si encontro un token en la cadena de argumentos
    //Si no encontro un token, imprime un mensaje de error
	if (path != NULL)
	{
		fprintf(stderr, "An error has occurred\n");
	}
    //Si no se encontro un token, se sale del programa con una salida exitosa
	else
	{
		exit(0);
	}
}

// Implementación del comando cd para cambiar la ruta del directorio actual,sólo debe recibir 1 argumento que será el cambio a la nueva ruta
void execute_cd(char *newpath)
{
    //Usa la funcion srtok_r para obtener el primer token de la cadena newpath
	char *path = strtok_r(newpath, " ", &newpath);
    //Verifica si encontro un token en la cadena de argumentos, si es null imprime un mensaje de error
	if (path == NULL)
	{
		fprintf(stderr, "An error has occurred\n");
	}
	else
	{
		if (strtok_r(NULL, " ", &newpath) != NULL)
		{
            fprintf(stderr, "An error has occurred\n");
		}
		else
		{
			if (access(path, F_OK) == 0) // Si la ruta existe
			{
				chdir(path);
			}
			else
			{
            fprintf(stderr, "An error has occurred\n");
			}
		}
	}
}

//Funcion que se encarga de leer los comandos entrados desde consola
void interactive_mode() {
    //Arreglo con la longitud definida por MAX_LINE_LENGTH, almacena la entrada del usuario
    char input[MAX_LINE_LENGTH];
    //Arreglo de punteros de longitud definida por MAX_ARGS, almacena los argumentos individuales
    char *args[MAX_ARGS];

    //Bucle infinito para leer los comandos ingresados por el usuario
    while (1) {
        // Se imprime un prompt para el usuario
        printf("wish> ");
        // Se usa la funcion fflush para que el prompt se imprima en la consola y no almacene en buffer
        fflush(stdout);

        // Se usa la funcion fgets para leer la entrada del usuario, si la funcion retorna NULL
        // se imprime un mensaje de error y se sale del programa
        if (fgets(input, MAX_LINE_LENGTH, stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            exit(1);
        }

        // Se remueve el caracter de nueva linea
        input[strcspn(input, "\n")] = '\0';

        // Se llama a la funcion execute_command para ejecutar el comando
        execute_command(args, 0);
    }
}

// Funcion que se encarga de leer los comandos desde un archivo de lote
// Recibe un string con el nombre del archivo de lote
void batch_mode(char *batch_file) {
    // Se abre el archivo de lote en modo lectura y se almacena en file
    FILE *file = fopen(batch_file, "r");
    // Si fallo fopen imprime un mensahe de error
    if (file == NULL) {
        fprintf(stderr, "Error opening batch file\n");
        exit(1);
    }

    // Arreglo con la longitud definida por MAX_LINE_LENGTH, almacena la entrada del usuario
    char input[MAX_LINE_LENGTH];
    // Arreglo de punteros de longitud definida por MAX_ARGS, almacena los argumentos individuales
    char *args[MAX_ARGS];

    // Bucle para leer los comandos del archivo de lote, se ejecuta mientras fgets no retorne NULL
    while (fgets(input, MAX_LINE_LENGTH, file) != NULL) {
        // Se remueve el caracter de nueva linea
        input[strcspn(input, "\n")] = '\0';

        // Se llama a la funcion execute_command para ejecutar el comando en segundo plano
        execute_command(args, 1);
    }

    // Se cierra el archivo de lote
    fclose(file);
}


//Funcion principal
int main(int argc, char *argv[]) {
    // Verificar si se proporciono un archivo de lote o no
    if (argc == 1) {
        MODE = MODE_INTERACTIVE;
        interactive_mode(); // No se proporcionó un archivo de lote, ejecutar en modo interactivo
    } else if (argc == 2) {
        MODE = MODE_BATCH;
        batch_mode(argv[1]); // Se proporcionó un archivo de lote, ejecutar en modo batch
    } else {
        // Se imprime un mensaje de error si se proporcionaron demasiados argumentos
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(1);
    }

    return 0;
}




