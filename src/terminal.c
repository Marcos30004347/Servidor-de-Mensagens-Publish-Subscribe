#include "terminal.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>

int keyboard_input()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

int get_charactere()
{
    unsigned char character;
    if ((read(0, &character, sizeof(character))) < 0)
        return '\0';
    else
        return character;
}

int read_inputed_line(char* message, int max)
{
    int length = 0;
    char chacatere = get_charactere();

    while (chacatere != '\n')
    {
        if(length >= max)
            return -1;

        message[length++] = chacatere;
        chacatere = get_charactere();
    }

    message[length++] = '\n';
    return length;
}


