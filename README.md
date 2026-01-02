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
