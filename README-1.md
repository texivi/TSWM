# TSWM (Texivi Window Manager)

A tiny floating X11 window manager in C. No bar, no borders, no extras.
Part of the Texivi Software Suite (TSS).

## Build and install

    make
    sudo make install        # also adds a TSWM entry for display managers

Without a display manager, put `exec tswm` in `~/.xinitrc` and run `startx`.

Edit `config.def.h` (or `config.h` after the first build) to change keys
or the terminal, then run `make` again.

## Keys (Super = MOD)

| Keys                       | Action                         |
|----------------------------|--------------------------------|
| Super + Enter              | open terminal (`xterm`)        |
| Super + W                  | close window                   |
| Super + F                  | toggle fullscreen              |
| Super + C                  | center window                  |
| Alt + Tab / Alt+Shift+Tab  | cycle windows                  |
| Super + 1..6               | go to workspace                |
| Super + Shift + 1..6       | send window to workspace       |
| Super + Shift + Q          | quit TSWM                      |
| Super + left drag          | move window                    |
| Super + right drag         | resize window                  |

Focus follows the mouse.

Part of the Texivi project.
