#define _POSIX_SOURCE
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h> // exit, etc.
#include <unistd.h> // fork, exec, sleep, etc.
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include "readline.h"
// #include "html_writer.h"

void display(char *);

int main(int argc, char *argv[])
{
    int forkOne;
    DIR *dir;
    struct dirent *ent;
    char *photos;
    char *photoDirectory;
    FILE *fp;

    if (argc >= 2)
    {
        photoDirectory = argv[1];
    }
    else
    {
        perror("No directory given");
    }

    if ((dir = opendir(photoDirectory)) != NULL)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            photos = ent->d_name;
            char directory[100];
            char thumbnail[100];
            strcpy(directory, "photos/");
            strcpy(thumbnail, "thumbnails/");
            strcat(directory, photos);
            strcat(thumbnail, photos);

            //I'm going to be honest i found this if statement online
            //and dont really understand what is happening even after
            //i read the documentation on strstr()
            if (strstr(photos, ".jpg") != NULL)
            {
                forkOne = fork();
                if (forkOne == 0)
                {
                    display(directory);
                    exit(0);
                }
                else
                {
                    //Janky way to store captions
                    char rotate[32];
                    char entry[8];
                    char caption[64];
                    fprintf(stdout, "Input rotation amount: ");
                    scanf("%s", rotate);
                    fprintf(stdout, "Would you like to add a caption [y or n]: ");
                    scanf("%s", entry);
                    if (strcmp(entry, "y") == 0)
                    {
                        fp = fopen("captions.txt", "a");
                        if (fp == NULL)
                        {
                            printf("Error opening Caption file");
                        }
                        else
                        {
                            fprintf(stdout, "What would you like your caption to be?: ");
                            scanf("%s", caption);
                            fprintf(fp, "%s \n", caption);
                            fclose(fp);
                        }
                    }
                    else
                    {
                        fp = fopen("captions.txt", "a");
                        if (fp == NULL)
                        {
                            printf("Error opening Caption file");
                        }
                        else
                        {
                            fprintf(fp, "\n");
                            fclose(fp);
                        }
                    }

                    if (fork() == 0)
                    {
                        execlp("convert", "convert", directory, "-rotate", rotate, directory, NULL);
                        exit(0);
                    }
                    else
                    {
                        wait(NULL);
                        kill(forkOne, SIGTERM);
                        // continue;
                    }
                }
                int seventyFork;
                seventyFork = fork();
                if (seventyFork == -1)
                {
                    printf("FORKING error");
                    EXIT_FAILURE;
                }

                if (seventyFork == 0)
                {
                    char seventy[32];
                    strcpy(seventy, "seventyPer/");
                    strcat(seventy, photos);
                    execlp("convert", "convert", directory, "-resize", "75%", seventy, NULL);
                }

                int thumbFork;
                thumbFork = fork();

                if (thumbFork == -1)
                {
                    printf("FORKING error");
                    EXIT_FAILURE;
                }

                if (thumbFork == 0)
                {
                    char thumbs[32];
                    strcpy(thumbs, "thumbnails/");
                    strcat(thumbs, photos);
                    execlp("convert", "convert", directory, "-resize", "25%", thumbs, NULL);
                }
            }
        }
        closedir(dir);
    }
    else
    {
        printf("Problem opening directory");
        EXIT_FAILURE;
    }
    return 0;
}

void display(char *photos)
{
    execlp("display", "display", photos, NULL);
}
