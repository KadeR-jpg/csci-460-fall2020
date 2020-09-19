#include <stdio.h>  // (f)printf, stdout, stderr, etc.
#include <stdlib.h> // exit, etc.
#include <unistd.h> // fork, exec, sleep, etc.
#include <sys/types.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// Error Codes:
//  -1: exec failed!

int main(void)
{
    // Initializations.
    int rc;
    // char *photo_in = "photos/photoE.jpg";
    struct dirent *dir;
    char *photo;
    char *photo_out = "photo_out.jpg";
    // fprintf(stdout, "-> Converting: '%s'\n", photo_in);
    // Convert a photo.
    rc = fork();
    if (0 == rc)
    {
        // see man execlp.  program name, then list of arguments as char strings.
        // arg list must be terminated by a NULL
        // and the zeroth arg is (by convention) the name of the program
        // rc = execlp("convert", "convert", "-geometry", "50%", photo_in, photo_out, NULL);
        if (chdir("/photos") != 0)
        {
            chdir("photos");
            directory = opendir(".");

            // prinf("%s", getwd())s
            // printf("%s\n", getcwd(s, 100));
            while ((dir = readdir(directory)) != NULL)
            {
                photo = dir->d_name;
                printf("%s", photo);
                if (strstr(photo, ".jpg") != NULL)
                {
                    execlp("convert", "convert", "-resize", "50%", "-thumbnail" photo, photo_out, NULL);
                }
            }
            // closedir(directory);
        }

        return 0;
    }
}
