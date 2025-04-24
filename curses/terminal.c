#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <termios.h>

struct termios orig_term;
struct termios new_term;

void reset_terminal_mode()
{
    tcsetattr(0, TCSANOW, &orig_term);
}

void set_terminal_mode()
{

    tcgetattr(0, &orig_term);
    memcpy(&new_term, &orig_term, sizeof(new_term));

    //register the exit in the ncurses function
    cfmakeraw(&new_term);
    tcsetattr(0, TCSANOW, &new_term);
}


int kbhit()
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL,NULL);
}

int kbhit_nb()
{
    struct timeval tv = { 0L, 0L };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv);
}

