#pragma once

#include "Types.hpp"
#include <array>
#include <optional>
#include <vector>

namespace tetris {

struct Cell { int color = -1; };

class Board {
public:
    Board();
    bool isInside(const Point &p) const;
    bool isOccupied(const Point &p) const;
    bool isValidPosition(const std::array<Point,4> &blocks, const Point &pos) const;
    void place(const std::array<Point,4> &blocks, const Point &pos, int color);
    // Find indices of full lines (0..BoardHeight-1). Does not remove them.
    std::vector<int> getFullLines() const;
    // Remove the given lines and shift above rows down.
    void removeLines(const std::vector<int>& lines);
    const Cell& at(int x, int y) const;
private:
    std::array<std::array<Cell, BoardWidth>, BoardHeight> grid;
};

} // namespace tetris
