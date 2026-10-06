/* tswm - Texivis Suckless Window Manager. A tiny floating X11 window manager.
 * Part of the Texivi Software Suite (TSS). */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/* config: edit this block, then re-run ./install.sh */

/* workspaces and super key*/

#define MOD Mod4Mask            
#define NWS 6                   

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

static void spawn(const Arg *a);
static void close_win(const Arg *a);
static void fullscreen(const Arg *a);
static void center(const Arg *a);
static void cycle(const Arg *a);
static void goto_ws(const Arg *a);
static void send_ws(const Arg *a);
static void quit(const Arg *a);

static const char *term[] = {"xterm", NULL};

static const Key keys[] = {
/* Keybinds */

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

typedef struct Client {
    struct Client *next;
    Window win;
    int ws;                     
    int fs;                     
    int unmaps;                 
    int x, y;                   
    unsigned int w, h;
} Client;

static Display *dpy;
static Window root;
static int sw, sh;                     
static Client *clients, *cur;           
static int ws;                          
static int running = 1;
static unsigned int numlock;
static Atom wm_protocols, wm_delete;

/* mouse drag state (Super left to move, Super right to resize) */
static Window drag_win;
static unsigned int drag_button;
static int drag_x, drag_y, drag_gx, drag_gy;
static unsigned int drag_gw, drag_gh;


static unsigned int clean(unsigned int mask) {
    return mask & ~(numlock | LockMask) &
        (ShiftMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask | Mod5Mask);
}

static Client *find(Window w) {
    Client *c;

    for (c = clients; c; c = c->next)
        if (c->win == w) return c;
    return NULL;
}

static void focus(Client *c) {
    cur = c;
    if (c)
        XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
    else
        XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
}

static void focus_top(void) {
    Client *c, *top = NULL;

    for (c = clients; c; c = c->next)
        if (c->ws == ws) top = c;
    focus(top);
}

static void raise_focus(Client *c) {
    XRaiseWindow(dpy, c->win);
    focus(c);
}

static void drop(Client *c) {
    Client **pp;
    int had_focus = (cur == c);

    for (pp = &clients; *pp && *pp != c; pp = &(*pp)->next);
    if (*pp) *pp = c->next;
    if (drag_win == c->win) drag_win = 0;
    free(c);

    if (had_focus) {
        cur = NULL;
        focus_top();
    }
}

static void manage(Window w, int adopt) {
    XWindowAttributes wa;
    Client *c;

    if (find(w) || !XGetWindowAttributes(dpy, w, &wa)) return;
    if (!(c = calloc(1, sizeof(Client)))) exit(1);

    c->win = w;
    c->ws  = ws;

    if (clients) {
        Client *t = clients;
        while (t->next) t = t->next;
        t->next = c;
    } else {
        clients = c;
    }

    XSelectInput(dpy, w, StructureNotifyMask | EnterWindowMask);

    
    if (!adopt && wa.x == 0 && wa.y == 0) {
        int x = (sw - (wa.width  + 2 * wa.border_width)) / 2;
        int y = (sh - (wa.height + 2 * wa.border_width)) / 2;
        XMoveWindow(dpy, w, x < 0 ? 0 : x, y < 0 ? 0 : y);
    }
}


static void spawn(const Arg *a) {
    if (fork() == 0) {
        if (dpy) close(ConnectionNumber(dpy));
        setsid();
        execvp(a->cmd[0], (char **)a->cmd);
        _exit(1);
    }
}

static void close_win(const Arg *a) {
    Atom *protos;
    int i, n, has_delete = 0;

    if (!cur) return;

    if (XGetWMProtocols(dpy, cur->win, &protos, &n)) {
        for (i = 0; i < n; i++)
            if (protos[i] == wm_delete) has_delete = 1;
        XFree(protos);
    }

    if (has_delete) {
        XEvent ev = {0};

        ev.xclient.type         = ClientMessage;
        ev.xclient.window       = cur->win;
        ev.xclient.message_type = wm_protocols;
        ev.xclient.format       = 32;
        ev.xclient.data.l[0]    = (long)wm_delete;
        ev.xclient.data.l[1]    = CurrentTime;
        XSendEvent(dpy, cur->win, False, NoEventMask, &ev);
    } else {
        XKillClient(dpy, cur->win);    
    }
}

static void fullscreen(const Arg *a) {
    XWindowAttributes wa;

    if (!cur) return;

    if ((cur->fs = !cur->fs)) {
        if (XGetWindowAttributes(dpy, cur->win, &wa)) {
            cur->x = wa.x;
            cur->y = wa.y;
            cur->w = (unsigned int)wa.width;
            cur->h = (unsigned int)wa.height;
        }
        XMoveResizeWindow(dpy, cur->win, 0, 0, (unsigned int)sw, (unsigned int)sh);
        XRaiseWindow(dpy, cur->win);
    } else {
        XMoveResizeWindow(dpy, cur->win, cur->x, cur->y, cur->w, cur->h);
    }
}

static void center(const Arg *a) {
    XWindowAttributes wa;
    int x, y;

    if (!cur || cur->fs || !XGetWindowAttributes(dpy, cur->win, &wa)) return;

    x = (sw - (wa.width  + 2 * wa.border_width)) / 2;
    y = (sh - (wa.height + 2 * wa.border_width)) / 2;
    XMoveWindow(dpy, cur->win, x < 0 ? 0 : x, y < 0 ? 0 : y);
}

static void cycle(const Arg *a) {
    Client *c, *pick = NULL, *prev = NULL;

    if (!cur) return;

    if (a->i > 0) {
        for (c = cur->next; c && !pick; c = c->next)
            if (c->ws == ws) pick = c;
        for (c = clients; c && !pick; c = c->next)
            if (c->ws == ws) pick = c;
    } else {
        for (c = clients; c && c != cur; c = c->next)
            if (c->ws == ws) prev = c;
        pick = prev;
        if (!pick)
            for (c = cur->next; c; c = c->next)
                if (c->ws == ws) pick = c;     
    }

    if (pick && pick != cur) raise_focus(pick);
}

static void goto_ws(const Arg *a) {
    Client *c;

    if (a->i < 0 || a->i >= NWS || a->i == ws) return;

    for (c = clients; c; c = c->next)
        if (c->ws == ws) {
            c->unmaps++;
            XUnmapWindow(dpy, c->win);
        }

    ws = a->i;

    for (c = clients; c; c = c->next)
        if (c->ws == ws) XMapWindow(dpy, c->win);

    focus_top();
}

static void send_ws(const Arg *a) {
    if (!cur || a->i < 0 || a->i >= NWS || a->i == ws) return;

    cur->ws = a->i;
    cur->unmaps++;
    XUnmapWindow(dpy, cur->win);
    focus_top();
}

static void quit(const Arg *a) {
    running = 0;
}

/* events*/

static void button_press(XEvent *e) {
    XWindowAttributes wa;
    Client *c = find(e->xbutton.subwindow);

    if (!c || c->fs || !XGetWindowAttributes(dpy, c->win, &wa)) return;

    drag_win    = c->win;
    drag_button = e->xbutton.button;
    drag_x      = e->xbutton.x_root;
    drag_y      = e->xbutton.y_root;
    drag_gx     = wa.x;
    drag_gy     = wa.y;
    drag_gw     = (unsigned int)wa.width;
    drag_gh     = (unsigned int)wa.height;

    raise_focus(c);
}

static void button_release(XEvent *e) {
    drag_win = 0;
}

static void motion_notify(XEvent *e) {
    int dx, dy;
    long w, h;

    if (!drag_win) return;

    while (XCheckTypedEvent(dpy, MotionNotify, e));

    dx = e->xmotion.x_root - drag_x;
    dy = e->xmotion.y_root - drag_y;
    w  = (long)drag_gw + (drag_button == Button3 ? dx : 0);
    h  = (long)drag_gh + (drag_button == Button3 ? dy : 0);

    XMoveResizeWindow(dpy, drag_win,
        drag_gx + (drag_button == Button1 ? dx : 0),
        drag_gy + (drag_button == Button1 ? dy : 0),
        (unsigned int)(w < 1 ? 1 : w),
        (unsigned int)(h < 1 ? 1 : h));
}

static void key_press(XEvent *e) {
    KeySym sym = XkbKeycodeToKeysym(dpy, (KeyCode)e->xkey.keycode, 0, 0);
    unsigned int i;

    for (i = 0; i < sizeof(keys) / sizeof(*keys); i++)
        if (keys[i].keysym == sym && clean(keys[i].mod) == clean(e->xkey.state))
            keys[i].fn(&keys[i].arg);
}

static void map_request(XEvent *e) {
    Window w = e->xmaprequest.window;
    Client *c;

    manage(w, 0);
    if (!(c = find(w))) return;

    c->ws = ws;                
    XMapWindow(dpy, w);
    raise_focus(c);
}

static void configure_request(XEvent *e) {
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    Client *c = find(ev->window);
    XWindowChanges wc;

    if (c && c->fs) return;     

    wc.x          = ev->x;
    wc.y          = ev->y;
    wc.width      = ev->width;
    wc.height     = ev->height;
    wc.border_width = ev->border_width;
    wc.sibling    = ev->above;
    wc.stack_mode = ev->detail;
    XConfigureWindow(dpy, ev->window, (unsigned int)ev->value_mask, &wc);
}

static void destroy_notify(XEvent *e) {
    Client *c = find(e->xdestroywindow.window);

    if (c) drop(c);
}

static void unmap_notify(XEvent *e) {
    Client *c = find(e->xunmap.window);

    if (!c) return;
    if (c->unmaps > 0) {       
        c->unmaps--;
        return;
    }
    drop(c);                   
}

static void enter_notify(XEvent *e) {
    Client *c;

    while (XCheckTypedEvent(dpy, EnterNotify, e));

    if (e->xcrossing.mode != NotifyNormal && e->xcrossing.window != root) return;
    if ((c = find(e->xcrossing.window)) && c->ws == ws && c != cur) focus(c);
}

static void mapping_notify(XEvent *e);

static void grab_input(void) {
    unsigned int i, j, mods[4];
    XModifierKeymap *map = XGetModifierMapping(dpy);
    KeyCode nl = XKeysymToKeycode(dpy, XK_Num_Lock), code;

    numlock = 0;
    for (i = 0; i < 8; i++)
        for (j = 0; j < (unsigned int)map->max_keypermod; j++)
            if (nl && map->modifiermap[i * (unsigned int)map->max_keypermod + j] == nl)
                numlock = 1u << i;
    XFreeModifiermap(map);

    mods[0] = 0;
    mods[1] = LockMask;
    mods[2] = numlock;
    mods[3] = numlock | LockMask;

    XUngrabKey(dpy, AnyKey, AnyModifier, root);

    for (i = 0; i < sizeof(keys) / sizeof(*keys); i++)
        if ((code = XKeysymToKeycode(dpy, keys[i].keysym)))
            for (j = 0; j < 4; j++)
                XGrabKey(dpy, code, keys[i].mod | mods[j], root, True,
                         GrabModeAsync, GrabModeAsync);

    for (i = Button1; i <= Button3; i += 2)
        for (j = 0; j < 4; j++)
            XGrabButton(dpy, i, MOD | mods[j], root, True,
                        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                        GrabModeAsync, GrabModeAsync, None, None);
}

static void mapping_notify(XEvent *e) {
    XMappingEvent *ev = &e->xmapping;

    if (ev->request == MappingKeyboard || ev->request == MappingModifier) {
        XRefreshKeyboardMapping(ev);
        grab_input();
    }
}

/* startup*/

static int xerror(Display *d, XErrorEvent *e) {
    return 0;                  
}

static int xerror_start(Display *d, XErrorEvent *e) {
    fputs("tswm: another window manager is already running\n", stderr);
    exit(1);
}

static void adopt_windows(void) {
    Window r, p, *kids;
    unsigned int n, i;
    XWindowAttributes wa;

    if (!XQueryTree(dpy, root, &r, &p, &kids, &n)) return;

    for (i = 0; i < n; i++)
        if (XGetWindowAttributes(dpy, kids[i], &wa) &&
            !wa.override_redirect && wa.map_state == IsViewable)
            manage(kids[i], 1);

    if (kids) XFree(kids);
    focus_top();
}

static void (*handler[LASTEvent])(XEvent *) = {
    [ButtonPress]      = button_press,
    [ButtonRelease]    = button_release,
    [MotionNotify]     = motion_notify,
    [KeyPress]         = key_press,
    [MapRequest]       = map_request,
    [ConfigureRequest] = configure_request,
    [DestroyNotify]    = destroy_notify,
    [UnmapNotify]      = unmap_notify,
    [EnterNotify]      = enter_notify,
    [MappingNotify]    = mapping_notify,
};

int main(void) {
    XEvent ev;
    int s;

    if (!(dpy = XOpenDisplay(NULL))) {
        fputs("tswm: cannot open display\n", stderr);
        return 1;
    }

    signal(SIGCHLD, SIG_IGN);

    s    = DefaultScreen(dpy);
    root = RootWindow(dpy, s);
    sw   = DisplayWidth(dpy, s);
    sh   = DisplayHeight(dpy, s);

    wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
    wm_delete    = XInternAtom(dpy, "WM_DELETE_WINDOW", False);

    XSetErrorHandler(xerror_start);
    XSelectInput(dpy, root, SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(dpy, False);
    XSetErrorHandler(xerror);

    XDefineCursor(dpy, root, XCreateFontCursor(dpy, XC_left_ptr));
    grab_input();
    adopt_windows();

    while (running) {
        XNextEvent(dpy, &ev);
        if (handler[ev.type]) handler[ev.type](&ev);
    }

    XCloseDisplay(dpy);
    return 0;
}
