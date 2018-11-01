#define _XOPEN_SOURCE 600
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#include <stdio.h>
#include <string.h>

int create_pty()
{
    int master = posix_openpt(O_RDWR);
    grantpt(master);
    unlockpt(master);
    return master;
}

int main()
{
    int master = create_pty();
    char * slavename = ptsname(master);
    if (slavename == NULL)
        _exit(EXIT_FAILURE);
    printf("%s\n", slavename);

    if (!(fork()))
    {
        close(master);

        int slave = open(slavename, O_RDWR);
        dup2(slave, 0);
        dup2(slave, 1);
        dup2(slave, 2);
        close(slave);

        sleep(10);

        printf("\
When I find my code in tons of trouble,\r\n\
Friends and colleagues come to me,\r\n\
Speaking words of wisdom:\r\n\
\"Write in C.\"\r\n\
\r\n\
As the deadline fast approaches,\r\n\
And bugs are all that I can see,\r\n\
Somewhere, someone whispers:\r\n\
\"Write in C.\"\r\n\
\r\n\
Write in C, Write in C,\r\n\
Write in C, oh, Write in C.\r\n\
LOGO's dead and buried,\r\n\
Write in C.\r\n\
\r\n\
I used to write a lot of FORTRAN,\r\n\
For science it worked flawlessly.\r\n\
Try using it for graphics!\r\n\
Write in C.\r\n\
\r\n\
\r\n\
If you've just spent nearly 30 hours\r\n\
Debugging some assembly,\r\n\
Soon you will be glad to\r\n\
Write in C.\r\n\
\r\n\
Write in C, Write in C,\r\n\
Write in C, yeah, Write in C.\r\n\
Only wimps use BASIC.\r\n\
Write in C.\r\n\
\r\n\
Write in C, Write in C\r\n\
Write in C, oh, Write in C.\r\n\
Pascal won't quite cut it.\r\n\
Write in C.\r\n\
\r\n\
Write in C, Write in C,\r\n\
Write in C, yeah, Write in C.\r\n\
Don't even mention COBOL.\r\n\
Write in C.\r\n\
                ");

        _exit(EXIT_SUCCESS);
    }

    while (1)
    {
        char ch;
        int r = read(master, &ch, 1);
        if (r <= 0)
            _exit(EXIT_SUCCESS);
        if ('A' <= ch && ch < 'Z')
            ch++;
        write(master, &ch, 1);
    }
}
