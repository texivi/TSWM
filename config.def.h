/*default tswm config. Copied to config.h on first build. */

#ifndef CONFIG_H
#define CONFIG_H

#define MOD Mod4Mask            /* Super key */
#define NWS 6                   /* number of workspaces */

static const char *term[] = {"xterm", NULL};

static const Key keys[] = {
    /* modifier           key        action      argument */
    {MOD,                 XK_Return, spawn,      {.cmd = term}},
    {MOD,                 XK_w,      close_win,  {0}},
    {MOD,                 XK_f,      fullscreen, {0}},
    {MOD,                 XK_c,      center,     {0}},
    {MOD|ShiftMask,       XK_q,      quit,       {0}},

    {Mod1Mask,            XK_Tab,    cycle,      {.i =  1}},
    {Mod1Mask|ShiftMask,  XK_Tab,    cycle,      {.i = -1}},

    {MOD,                 XK_1,      goto_ws,    {.i = 0}},
    {MOD|ShiftMask,       XK_1,      send_ws,    {.i = 0}},
    {MOD,                 XK_2,      goto_ws,    {.i = 1}},
    {MOD|ShiftMask,       XK_2,      send_ws,    {.i = 1}},
    {MOD,                 XK_3,      goto_ws,    {.i = 2}},
    {MOD|ShiftMask,       XK_3,      send_ws,    {.i = 2}},
    {MOD,                 XK_4,      goto_ws,    {.i = 3}},
    {MOD|ShiftMask,       XK_4,      send_ws,    {.i = 3}},
    {MOD,                 XK_5,      goto_ws,    {.i = 4}},
    {MOD|ShiftMask,       XK_5,      send_ws,    {.i = 4}},
    {MOD,                 XK_6,      goto_ws,    {.i = 5}},
    {MOD|ShiftMask,       XK_6,      send_ws,    {.i = 5}},
};

#endif
