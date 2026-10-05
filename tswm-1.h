/* tswm.h - types and action prototypes for tswm */

#ifndef TSWM_H
#define TSWM_H

#include <X11/Xlib.h>

typedef union {
    const char **cmd;   /* NULL terminated argv for spawn */
    int i;              /* workspace index, cycle direction */
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
    int ws;                 /* workspace this window lives on */
    int fs;                 /* fullscreen flag */
    int unmaps;             /* unmaps we caused ourselves (workspace hiding) */
    int x, y;               /* saved geometry for leaving fullscreen */
    unsigned int w, h;
} Client;

/* actions, usable from config.h */
void spawn(const Arg *a);
void close_win(const Arg *a);
void fullscreen(const Arg *a);
void center(const Arg *a);
void cycle(const Arg *a);
void goto_ws(const Arg *a);
void send_ws(const Arg *a);
void quit(const Arg *a);

#endif
