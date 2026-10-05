# TSWM (Texivi's Simple Window Manager)

> A tiny, minimalist floating X11 window manager written in C.

Part of the **[Texivi Software Suite (TSS)](tss.md)**.

---

## Installation

### 1. Clone the Repository

```bash
git clone [https://github.com/texivi/tswm.git](https://github.com/texivi/tswm.git)
cd tswm
```

### run the installer

```bash
./install.sh
```

*For non-interactive / automated setups, pass the `-y` flag:*

```bash
./install.sh -y
```

### 3. Manual Build (Alternative)

If you prefer building directly with `make`:

```bash
make
sudo make install
```

Without a display manager, add the following to your `~/.xinitrc` and run `startx`:

```bash
exec tswm
```

---

## Configuration

1. Edit `config.def.h` (or `config.h` after your first build).
2. Modify whatever the fuck you want
3. Recompile and install:
   ```bash
   make && sudo make install
   ```

---

## Keybindings (Super = MOD) 

| Keys | Action |
| :--- | :--- |
| `Super` + `Enter` | Open terminal (`xterm`) |
| `Super` + `W` | Close window |
| `Super` + `F` | Toggle fullscreen |
| `Super` + `C` | Center window |
| `Alt` + `Tab` / `Alt` + `Shift` + `Tab` | Cycle windows |
| `Super` + `1..6` | Go to workspace 1–6 |
| `Super` + `Shift` + `1..6` | Send window to workspace 1–6 |
| `Super` + `Shift` + `Q` | Quit TSWM |
| `Super` + left drag | Move window |
| `Super` + right drag | Resize window |
