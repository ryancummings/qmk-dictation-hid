#!/usr/bin/env sh
# Build or flash the example keymap: ./build.sh [compile|flash] [keyboard]
# Applies patches/ to the QMK checkout that `qmk env` reports, unless already applied.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
qmk_home=$(qmk env | sed -n 's/^QMK_HOME="\(.*\)"$/\1/p')
for patch in "$here"/patches/*.patch; do
    if git -C "$qmk_home" apply --reverse --check "$patch" 2>/dev/null; then
        echo "Already applied: $(basename "$patch")"
    else
        git -C "$qmk_home" apply "$patch"
        echo "Applied: $(basename "$patch")"
    fi
done
cd "$here"
exec qmk "${1:-compile}" -kb "${2:-planck/rev6_drop}" -km dictation_hid
