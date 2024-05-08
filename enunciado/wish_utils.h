/*
    * wish_utils.h
    *
    *  Archivo de encabezado que contiene las declaraciones de funciones y variables utilizadas en el programa principal
*/

#ifndef __WISH_U_H__
#define __WISH_U_H__

void execute_exit(char *args);
void execute_cd(char *newpath);
void execute_path(char *newpaths, char ***mypath);
int wish_launch_redirect(char **args, char *file);

#endif