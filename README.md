# Nomad Command — Чистый C++17 + OpenGL ES 3.0 / SDL2 (Без Godot)

Нативный 3D RTS-движок без сторонних игровых движков:
* **Чистый C++17 + OpenGL ES 3.0 шейдеры** (освещение, глубина, 60 FPS).
* **Прямой загрузчик `.obj` из Meshy AI** (`src/ObjLoader.cpp` с авто-нормализацией масштаба из `models/tesla.obj` и `models/trooper.obj`).
* **Управление с экрана (SDL2 Touch + Mouse)**, камера RTS, бои, ИИ-волны и база Ала-Тоо.

### Сборка в Termux:
```bash
bash scripts/build_termux.sh
```
