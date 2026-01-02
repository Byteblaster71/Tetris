# test8 — Tetris (SFML)

Dieses Projekt ist ein kleines Tetris-Demo mit SFML.

Voraussetzungen
- CMake (>= 3.16)
- Ein C++ Compiler (MSVC / clang / gcc)
- SFML development files in D:/Programme/SFML-3.0.2 (Std. SFML-3 install)

Schnellstart
1. Configure: `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DSFML_DIR=D:/Programme/SFML-3.0.2/lib/cmake/SFML`
2. Build: `cmake --build build --config Debug`
3. Run: `build/bin/app[.exe]` (Windows) oder `./build/bin/app`

Wenn CMake das SFML-Modul nicht findet, prüfen Sie den Pfad `D:/Programme/SFML-3.0.2/lib/cmake/SFML` und passen `SFML_DIR` entsprechend an.

Sound & Assets
- Optional: place `clear.wav` in the `assets/` folder to enable a line-clear sound effect. If not present, the game runs silently.
 
Audio files (optional)
- `clear.wav` : play on line clear
- `rotate.wav`: play on piece rotation
- `drop.wav`  : play on soft drop
- `harddrop.wav`: play on hard drop
 
UI & Voice assets (optional)
- `ui_click.wav`: UI click when adjusting volume or interacting with UI
- `voice_levelup.wav`: optional voice line when you level up

Controls for audio
- `M` : toggle mute
- `[` : decrease volume by 10%
- `]` : increase volume by 10%

Scoring & Levels
- Lines cleared give points: 1→100, 2→300, 3→500, 4→800, scaled by (level+1); level increases every 10 cleared lines and drop speed increases with level.
