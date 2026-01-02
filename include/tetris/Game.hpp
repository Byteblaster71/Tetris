#pragma once

#include "Types.hpp"
#include "Tetromino.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>
#include <random>

namespace tetris {

class Game {
public:
    Game(sf::RenderWindow &window);
    void run();
private:
    void processInput();
    void update(float dt);
    void render();
    void spawnPiece();
    void lockPiece();
    void hardDrop();

    sf::RenderWindow &m_window;
    Board m_board;
    Tetromino m_active;
    TetrominoType m_next;
    std::mt19937 m_rng;
    float m_dropTimer = 0.0f;
    float m_dropInterval = 0.6f; // seconds
    bool m_running = true;
    bool m_paused = false;
    int m_score = 0;
    sf::Font m_font;
    bool m_fontLoaded = false;

    static const std::array<sf::Color, static_cast<int>(TetrominoType::Count)> colors;
};

} // namespace tetris
