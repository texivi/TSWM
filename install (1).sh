#!/bin/sh
# install.sh - build and install TSWM, then offer to add it to your login screen.
# Needs only tswm.c next to it, a C compiler and the X11 headers.
#
#   ./install.sh              build, install, and ask about the login entry
#   ./install.sh -y           same, answer yes to every question
#   ./install.sh uninstall    remove the binary and the login entry
#
# Part of the Texivi Software Suite (TSS).

set -e
cd "$(dirname "$0")"

PREFIX=${PREFIX:-/usr/local}
BINDIR=$PREFIX/bin
XSESSIONS=${XSESSIONS:-/usr/share/xsessions}
ENTRY=$XSESSIONS/tswm.desktop

# privileged commands: plain when root, sudo otherwise (SUDO= overrides)
if [ -z "${SUDO+x}" ]; then
    if [ "$(id -u)" -eq 0 ]; then SUDO=""; else SUDO="sudo"; fi
fi

YES=0
ACTION=install
for arg in "$@"; do
    case $arg in
        -y|--yes)  YES=1 ;;
        uninstall) ACTION=uninstall ;;
        *) echo "usage: $0 [-y] [uninstall]" >&2; exit 1 ;;
    esac
done

ask() {  # ask "question" -> 0 for yes (default yes)
    [ "$YES" -eq 1 ] && return 0
    printf '%s [Y/n] ' "$1"
    read -r ans || ans=n
    case $ans in [nN]*) return 1 ;; *) return 0 ;; esac
}

detect_dm() {
    dm=""
    if [ -r /etc/X11/default-display-manager ]; then
        dm=$(basename "$(cat /etc/X11/default-display-manager)")
    elif [ -L /etc/systemd/system/display-manager.service ]; then
        dm=$(basename "$(readlink -f /etc/systemd/system/display-manager.service)" .service)
    fi
    if [ -z "$dm" ]; then
        for p in gdm3 gdm sddm lightdm lxdm ly slim xdm; do
            if pgrep -x "$p" >/dev/null 2>&1; then dm=$p; break; fi
        done
    fi
    case $dm in
        gdm*|GDM*) echo gdm ;;
        sddm*)     echo sddm ;;
        lightdm*)  echo lightdm ;;
        lxdm*)     echo lxdm ;;
        ly*)       echo ly ;;
        slim*)     echo slim ;;
        xdm*)      echo xdm ;;
        *)         echo "" ;;
    esac
}

# for managers without a session menu (and for plain startx)
xinit_fallback() {
    file=$1
    if [ -f "$file" ] && grep -q 'exec tswm' "$file"; then
        echo "$file already starts tswm."
    elif [ -f "$file" ]; then
        echo "$file already exists. Add this as its last line:  exec tswm"
    elif ask "Create $file so it starts TSWM?"; then
        printf 'exec tswm\n' > "$file"
        echo "Created $file."
    fi
}

if [ "$ACTION" = uninstall ]; then
    $SUDO rm -f "$BINDIR/tswm" "$ENTRY"
    echo "TSWM removed. (Lines in ~/.xinitrc or ~/.xsession are left alone.)"
    exit 0
fi

echo "Building TSWM..."
bin=$(mktemp)
cc -O2 -std=c99 -Wall -Wextra -pedantic -Wno-unused-parameter \
    -o "$bin" tswm.c -lX11 || {
    rm -f "$bin"
    echo "Build failed. You need a C compiler and the X11 headers"
    echo "(libx11-dev on Debian/Ubuntu, libX11-devel on Fedora, libx11 on Arch)."
    exit 1
}

echo "Installing to $BINDIR..."
$SUDO install -Dm755 "$bin" "$BINDIR/tswm"
rm -f "$bin"

DM=${TSWM_DM:-$(detect_dm)}

case $DM in
    gdm|sddm|lightdm|lxdm|ly)
        if ask "Detected $DM. Add TSWM to the login screen?"; then
            tmp=$(mktemp)
            cat > "$tmp" <<DESK
[Desktop Entry]
Name=TSWM
Comment=Texivi Window Manager
Exec=$BINDIR/tswm
Type=Application
DESK
            $SUDO install -Dm644 "$tmp" "$ENTRY"
            rm -f "$tmp"
            echo "Added. Log out and pick TSWM from the session menu."
            [ "$DM" = gdm ] && echo "(GDM: click the gear icon on the password screen.)"
        fi
        ;;
    xdm)
        echo "Detected xdm (no session menu)."
        xinit_fallback "$HOME/.xsession"
        ;;
    *)
        [ "$DM" = slim ] && echo "Detected slim (no session menu)." \
                         || echo "No login manager detected."
        xinit_fallback "$HOME/.xinitrc"
        echo "Start it with: startx"
        ;;
esac

echo "Done."
