#pragma once

#include "Types.hpp"
#include <array>
#include <vector>

namespace tetris {

struct Tetromino {
    TetrominoType type;
    int rotation = 0; // 0..3
    Point position; // board coordinates of origin

    static std::array<std::array<Point,4>,4> getShape(TetrominoType type, int rotation);
};

} // namespace tetris
