
#ifndef TSWM_H
#define TSWM_H

#include <X11/Xlib.h>

typedef union {
    const char **cmd;  
    int i;             
} Arg;

typedef struct {
    unsigned int mod;
    KeySym keysym;
    void (*fn)(const Arg *);
    Arg arg;
} Key;

typedef struct Client {
    struct Client *next;
    Window win;
    int ws;                
    int fs;                 
    int unmaps;             
    int x, y;               
    unsigned int w, h;
} Client;

void spawn(const Arg *a);
void close_win(const Arg *a);
void fullscreen(const Arg *a);
void center(const Arg *a);
void cycle(const Arg *a);
void goto_ws(const Arg *a);
void send_ws(const Arg *a);
void quit(const Arg *a);

#endif
