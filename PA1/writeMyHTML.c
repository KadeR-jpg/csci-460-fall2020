#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

void write_html(char *);

void write_html(char *thumbnail)
{
    char *outfilename = "index.html";
    DIR *dir;
    struct dirent *ent;
    FILE *fp = NULL;
    char *photos;

    if ((fp = fopen(outfilename, "w")) == NULL)
    {
        fprintf(stderr, "**error: cannot open file '%s'.**\n", outfilename);
        exit(1);
    }

    // Write the BEGINNING parts of the basic structure of an HTML file.
    char *title = "Image Gallery Yeah";
    char *subtitle = "Browse to your hearts content";
    fprintf(fp, "<!DOCTYPE html>\n");
    fprintf(fp, "<html>\n");
    fprintf(fp, "<head>\n");
    fprintf(fp, "<title>%s</title>\n", title);
    fprintf(fp, "<link rel=\"stylesheet\" href=\"main.css\">");
    fprintf(fp, "</head>\n");
    fprintf(fp, "<body>\n");

    // Write some content for the page
    fprintf(fp, "<h1>%s</h1>\n", title);
    fprintf(fp, "<h2>%s</h2>\n", subtitle);

    // fp = fopen("captions.txt", "r");
    // if (fp == NULL)
    // {
    //     printf("Error opening captions");
    //     exit(EXIT_FAILURE);
    // }

    // while ((read = getline(&line, &len, fp)) != -1)
    // {
    //     printf("%s", line);
    // }

    // fclose(fp);

    // if (line)
    // {
    //     free(line);
    // }

    // Create an unordered list with items from the command line
    fprintf(fp, "<ul>\n");
    if ((dir = opendir(thumbnail)) != NULL)
    {

        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
        {
            photos = ent->d_name;
            if (strstr(photos, ".jpg") != NULL)
            {
                char thumb[100];
                strcpy(thumb, "thumbs/");
                strcat(thumb, photos);
                //fprintf(fp, "<li>\n");
                //fprintf(fp, "<h3>This is an image</h3>\n");
                //fprintf(fp, "<a href=%s><img src=%s></a>\n", thumb, thumb);
                //fprintf(fp, "</li>\n");

                fprintf(fp, "<div class=\"gallery\">\n");
                fprintf(fp, "<a target=\"_blank\" href=%s>\n", thumb);
                fprintf(fp, "<img src=%s alt=\"Cinque Terre\"\n", thumb);
                fprintf(fp, "</a>\n");
                fprintf(fp, "<div class=\"desc\">Add a description of the image here</div>\n");
                fprintf(fp, "</div>\n");
            }
        }
    }
    fprintf(fp, "</ul>\n");

    // Write the END parts of the basic structure of an HTML file.
    fprintf(fp, "</body>\n");
    fprintf(fp, "</html>\n");

    // Wrap up...
    fclose(fp);
}