#pragma once

#include <array>
#include <SFML/System/Vector2.hpp>

namespace tetris {

using Point = sf::Vector2i;

constexpr int BoardWidth = 10;
constexpr int BoardHeight = 20;
constexpr int CellSize = 30; // pixels

enum class TetrominoType { I, O, T, J, L, S, Z, Count };

} // namespace tetris
