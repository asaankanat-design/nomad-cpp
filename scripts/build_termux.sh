#!/data/data/com.termux/files/usr/bin/bash
set -e
pkg update -y && pkg install -y clang cmake sdl2
mkdir -p build && cd build
cmake .. && make -j4
echo "Сборка готова! Запуск (в Termux-X11): ./nomad_gl"
