//
// Created by josh on 1/22/21.
//

#pragma once

#include "posh.h"

#if defined POSH_OS_WIN32
#include <conio.h>
#elif defined POSH_OS_LINUX

/**
 Linux (POSIX) implementation of _kbhit().
 Morgan McGuire, morgan@cs.brown.edu
 */
#include <cstdio>
#include <sys/select.h>
#include <termios.h>
#include <sys/ioctl.h>

int _kbhit() {
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

/**
 More code from the internet
 */
#include <termios.h>
#include <stdio.h>

static struct termios old, current;

/* Initialize new terminal i/o settings */
void initTermios(int echo) {
    tcgetattr(0, &old); /* grab old terminal i/o settings */
    current = old; /* make new settings same as old settings */
    current.c_lflag &= ~ICANON; /* disable buffered i/o */
    if (echo) {
        current.c_lflag |= ECHO; /* set echo mode */
    } else {
        current.c_lflag &= ~ECHO; /* set no echo mode */
    }
    tcsetattr(0, TCSANOW, &current); /* use these new terminal i/o settings now */
}

/* Restore old terminal i/o settings */
void resetTermios(void) {
    tcsetattr(0, TCSANOW, &old);
}

/* Read 1 character - echo defines echo mode */
char getch_(int echo) {
    char ch;
    initTermios(echo);
    ch = getchar();
    resetTermios();
    return ch;
}

/* Read 1 character without echo */
char _getch(void) {
    return getch_(0);
}

#endif

union wide_char {
    wchar_t wide_C;
    char C[2];
};

inline short key_wait() {
    wchar_t R;
    wide_char user_input;
    user_input.wide_C = 0;
    //Loop twice, or until code causes the loop to exit
    //Two times are neccessary for function keys unfortunately
    for (int i = 0; i < 2; ++i) {
        //While there isn't a key pressed, loop doing nothing
        while (!_kbhit()) {}
        //Grab the next key from the buffer
        //Since the loop is done, there must be at least one
        user_input.C[i] = _getch();
        switch (user_input.C[i]) {
#if defined POSH_OS_WIN32
            case 0:
            case -32:
#elif defined POSH_OS_LINUX
            case 27:
#endif
                //The key pressed is a Function key because it matches one of these two cases
                //This means getch() must be called twice
                //Break switch, run the for loop again ++i
                break;
            default:
                R = user_input.wide_C;
                return R;
                break;
        }
    }
    return -1;
}
