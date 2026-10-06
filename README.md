# TSWM (Texivi's Suckless Window Manager)

TSWM is a tiny floating X11 window manager written in C.

Part of The [Texivi software suite](tss.md).

## Features

- Floating windows, focus follows the mouse
- Move and resize
- Fullscreen and center
- 6 workspaces

## Requirements

- A C compiler
- Xlib headers
- `xterm` (the default terminal, easy to change, see below)

## Install

```
git clone https://github.com/texivi/TSWM
cd TSWM
sh install.sh
```
And it will be automatically added to your login manager.

if you don't have one the installer will edit `~/.xinitrc` (or `~/.xsession` for xdm) start it with:

```
startx
```

Use `sh install.sh -y` to answer yes to every question.

## Uninstall

```
sh install.sh uninstall
```

This removes the binary and the login entry. the lines in `~/.xinitrc` or `~/.xsession` aren't deleted 

## Keybinds

`Super` is the Windows key.

| Keys                          | Action                    |
|-------------------------------|---------------------------|
| `Super + Enter`               | Open a terminal           |
| `Super + W`                   | Close the window          |
| `Super + F`                   | Toggle fullscreen         |
| `Super + C`                   | Center the window         |
| `Alt + Tab`                   | Next window               |
| `Alt + Shift + Tab`           | Previous window           |
| `Super + 1` to `6`            | Go to workspace           |
| `Super + Shift + 1` to `6`    | Send window to workspace  |
| `Super + Shift + Q`           | Quit TSWM                 |
| `Super + left mouse drag`     | Move a window             |
| `Super + right mouse drag`    | Resize a window           |

## Changing the keybinds

Open `tswm.c` and edit the section between `keybinds start` and `keybinds end`, then run `sh install.sh` again. Log out and back in (or restart TSWM) to apply it.

Each bind is a modifier, key, action and argument.

```c
{MOD, XK_Return, spawn, {.cmd = term}},
```

To change the terminal, edit this line:

```c
static const char *term[] = {"xterm", NULL};
```

To bind another program, add a command next to `term` and a line for it:

```c
static const char *browser[] = {"firefox", NULL};

{MOD, XK_b, spawn, {.cmd = browser}},
```

`MOD` is `Mod4Mask` (Super). Use `Mod1Mask` for Alt. `NWS` sets the number of workspaces.

---

*Part of the Texivi project.*
