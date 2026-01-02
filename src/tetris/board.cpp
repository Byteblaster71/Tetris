#include "../../include/tetris/Board.hpp"
#include <algorithm>

using namespace tetris;

Board::Board() {
    for (int y = 0; y < BoardHeight; ++y)
        for (int x = 0; x < BoardWidth; ++x)
            grid[y][x].color = -1;
}

bool Board::isInside(const Point &p) const {
    return p.x >= 0 && p.x < BoardWidth && p.y >= 0 && p.y < BoardHeight;
}

bool Board::isOccupied(const Point &p) const{
    if (!isInside(p)) return true; // outside counts as occupied for collision
    return grid[p.y][p.x].color != -1;
}

bool Board::isValidPosition(const std::array<Point,4> &blocks, const Point &pos) const {
    for (const auto &b : blocks) {
        Point p = pos + b;
        if (p.x < 0 || p.x >= BoardWidth || p.y >= BoardHeight) return false;
        if (p.y >= 0 && grid[p.y][p.x].color != -1) return false;
    }
    return true;
}

void Board::place(const std::array<Point,4> &blocks, const Point &pos, int color){
    for (const auto &b : blocks) {
        Point p = pos + b;
        if (p.y >= 0 && p.y < BoardHeight && p.x >= 0 && p.x < BoardWidth)
            grid[p.y][p.x].color = color;
    }
}

int Board::clearFullLines(){
    int lines = 0;
    int write = BoardHeight - 1;
    for (int y = BoardHeight - 1; y >= 0; --y) {
        bool full = true;
        for (int x = 0; x < BoardWidth; ++x) {
            if (grid[y][x].color == -1) { full = false; break; }
        }
        if (!full) {
            if (write != y) grid[write] = grid[y];
            --write;
        } else {
            ++lines;
        }
    }
    // Clear remaining rows
    for (int y = write; y >= 0; --y)
        for (int x = 0; x < BoardWidth; ++x) grid[y][x].color = -1;
    return lines;
}

const Cell& Board::at(int x, int y) const { return grid[y][x]; }
