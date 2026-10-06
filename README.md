# TSWM (Texivi's Suckless Window Manager)

TSWM is a tiny floating X11 window manager written in C.

Part of [Texivi's software suite](tss.md).

## Features

- Floating windows, focus follows the mouse
- Move and resize
- Fullscreen and center
- 6 workspaces
- One source file, about 500 lines

## Requirements

- A C compiler (`cc`)
- Xlib headers
  - Debian/Ubuntu: `sudo apt install build-essential libx11-dev`
  - Fedora: `sudo dnf install gcc libX11-devel`
  - Arch: `sudo pacman -S base-devel libx11`
- `xterm` (the default terminal, easy to change, see below)

## Install

```
git clone https://github.com/texivi/TSWM
cd TSWM
sh install.sh
```

The installer builds TSWM with `cc` and installs it to `/usr/local/bin`. It then detects your login manager (GDM, SDDM, LightDM, LXDM or ly) and asks if you want TSWM added to the login screen. Log out and pick **TSWM** from the session menu.

On GDM, click the gear icon on the password screen to choose a session.

No login manager, or xdm/slim? The installer offers to create `~/.xinitrc` (or `~/.xsession` for xdm) containing `exec tswm`. Then start it with:

```
startx
```

Use `sh install.sh -y` to answer yes to every question.

## Uninstall

```
sh install.sh uninstall
```

This removes the binary and the login entry. A line in `~/.xinitrc` or `~/.xsession` is left alone.

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

There is no config file. Open `tswm.c` and edit the section between `keybinds start` and `keybinds end`, then run `sh install.sh` again. Log out and back in (or restart TSWM) to apply it.

Each bind is one line: modifier, key, action, argument.

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

Available actions: `spawn`, `close_win`, `fullscreen`, `center`, `cycle`, `goto_ws`, `send_ws`, `quit`.

`MOD` is `Mod4Mask` (Super). Use `Mod1Mask` for Alt. `NWS` sets the number of workspaces.

---

*Part of the Texivi project.*
