#include "../../include/tetris/Tetromino.hpp"

using namespace tetris;

// Define base shapes (rotation 0) as 4 block coordinates relative to an origin.
// We'll compute rotations (0..3) using simple rotation rules.

static const std::array<std::array<Point,4>,7> baseShapes = {
    std::array<Point,4>{ Point{0,1}, Point{1,1}, Point{2,1}, Point{3,1} }, // I
    std::array<Point,4>{ Point{1,0}, Point{2,0}, Point{1,1}, Point{2,1} }, // O
    std::array<Point,4>{ Point{1,0}, Point{0,1}, Point{1,1}, Point{2,1} }, // T
    std::array<Point,4>{ Point{0,0}, Point{0,1}, Point{1,1}, Point{2,1} }, // J
    std::array<Point,4>{ Point{2,0}, Point{0,1}, Point{1,1}, Point{2,1} }, // L
    std::array<Point,4>{ Point{1,0}, Point{2,0}, Point{0,1}, Point{1,1} }, // S
    std::array<Point,4>{ Point{0,0}, Point{1,0}, Point{1,1}, Point{2,1} }  // Z
};

static Point rotatePoint(const Point &p, int rotation) {
    // rotation: 0..3, rotate around (1,1) center for typical tetris shapes
    Point r = p - Point{1,1};
    for (int i = 0; i < (rotation & 3); ++i) {
        r = Point{-r.y, r.x};
    }
    return r + Point{1,1};
}

std::array<std::array<Point,4>,4> Tetromino::getShape(TetrominoType type, int rotation) {
    std::array<std::array<Point,4>,4> result;
    const auto &base = baseShapes[static_cast<int>(type)];
    for (int rot = 0; rot < 4; ++rot) {
        for (int i = 0; i < 4; ++i) {
            result[rot][i] = rotatePoint(base[i], rot);
        }
    }
    return result;
}
