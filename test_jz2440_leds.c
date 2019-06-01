#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

void print_usage(char *file)
{
    printf("Usage: \n"
           "%s <dev> <on|off>\n"
           "eg:\n"
           "%s /dev/led0 on\n",
           file, file);
}

int main (int argc, char **argv)
{
        int fd;
        int val;
        char *file_name;
        char *command;

        if (argc != 3) {
                print_usage(argv[0]);
                return 0;
        }

        file_name = argv[1];

        fd = open(file_name, O_RDWR);
        if (fd < 0) {
                printf("Error: Can't open %s\n", file_name);
                return 0;
        }

        if (strcmp(argv[2], "on") == 0)
        {
        	val = 1;
        	printf("Test: turn on led\n");
        } else {
        	val = 0;
        	printf("Test: turn off led\n");
        }

        write(fd, &val, 4);
        close(fd);

    return 0;
}
