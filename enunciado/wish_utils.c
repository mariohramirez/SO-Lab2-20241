#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wish_utils.h"

extern char *mypath[];

void execute_exit(char *args)
{
	exit(0);
}

void execute_cd(char *newpath)
{
	char *path = strtok_r(newpath, " ", &newpath);
	if (path == NULL)
	{
		printf("Error: el comando cd requiere un argumento\n");
	}
	else
	{
		if (strtok_r(NULL, " ", &newpath) != NULL)
		{
			printf("Error: el comando cd solo acepta un argumento\n");
		}
		else
		{
			if (access(path, F_OK) == 0)
			{
				chdir(path);
				printf("La nueva ruta es %s\n", path);
			}
			else
			{
				printf("Directory does not exist: %s\n", path);
			}
		}
	}
}

void execute_path(char *newpath, char ***mypath)
{
	char *copypath = strdup(newpath);
	int path_count = 0;
	int i = 0;
	char *path = strtok_r(copypath, " ", &copypath);

	while (path != NULL)
	{
		path_count++;
		path = strtok_r(copypath, " ", &copypath);
	}

	if (path_count == 0)
	{
		printf("Variable mypath sin modificaciones\n");
	}
	else
	{
		*mypath = realloc(*mypath, (path_count + 1) * sizeof(char *));
		path = strtok_r(newpath, " ", &newpath);
		i = 0;
		while (path != NULL)
		{
			(*mypath)[i] = strdup(path);
			path = strtok_r(newpath, " ", &newpath);
			i++;
		}
		(*mypath)[i] = "";
	}
}
