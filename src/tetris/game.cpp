#include "../../include/tetris/Game.hpp"
#include <chrono>
#include <sstream>

using namespace tetris;

const std::array<sf::Color, static_cast<int>(TetrominoType::Count)> Game::colors = {
    sf::Color::Cyan, sf::Color::Yellow, sf::Color::Magenta,
    sf::Color::Blue, sf::Color(255,165,0), sf::Color::Green, sf::Color::Red
};

static TetrominoType randomType(std::mt19937 &rng) {
    std::uniform_int_distribution<int> dist(0, static_cast<int>(TetrominoType::Count) - 1);
    return static_cast<TetrominoType>(dist(rng));
}

Game::Game(sf::RenderWindow &window): m_window(window) {
    std::random_device rd;
    m_rng.seed(rd());
    m_next = randomType(m_rng);
    spawnPiece();
    // Load a system font if available
    m_fontLoaded = m_font.openFromFile("C:/Windows/Fonts/arial.ttf");
}

void Game::spawnPiece() {
    m_active.type = m_next;
    m_active.rotation = 0;
    m_active.position = Point{BoardWidth/2 - 2, -1};
    m_next = randomType(m_rng);
    // Check immediate collision -> game over
    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[0];
    if (!m_board.isValidPosition(blocks, m_active.position)) {
        m_running = false;
    }
}

void Game::processInput() {
    while (const std::optional<sf::Event> event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) m_window.close();
        if (event->is<sf::Event::KeyPressed>()) {
            auto key = event->getIf<sf::Event::KeyPressed>()->code;
            if (key == sf::Keyboard::Key::Escape) m_window.close();
            if (key == sf::Keyboard::Key::P) m_paused = !m_paused;
            if (!m_paused && m_running) {
                if (key == sf::Keyboard::Key::Left) {
                    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
                    Point newPos = m_active.position + Point{-1,0};
                    if (m_board.isValidPosition(blocks, newPos)) m_active.position = newPos;
                } else if (key == sf::Keyboard::Key::Right) {
                    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
                    Point newPos = m_active.position + Point{1,0};
                    if (m_board.isValidPosition(blocks, newPos)) m_active.position = newPos;
                } else if (key == sf::Keyboard::Key::Down) {
                    // soft drop
                    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
                    Point newPos = m_active.position + Point{0,1};
                    if (m_board.isValidPosition(blocks, newPos)) m_active.position = newPos;
                } else if (key == sf::Keyboard::Key::Up) {
                    // rotate
                    int newRot = (m_active.rotation + 1) & 3;
                    auto blocks = Tetromino::getShape(m_active.type, newRot)[newRot];
                    if (m_board.isValidPosition(blocks, m_active.position)) m_active.rotation = newRot;
                } else if (key == sf::Keyboard::Key::Space) {
                    hardDrop();
                }
            } else if (!m_running && key == sf::Keyboard::Key::R) {
                // restart
                m_board = Board();
                m_running = true;
                m_paused = false;
                m_score = 0;
                spawnPiece();
            }
        }
    }
}

void Game::update(float dt) {
    if (m_paused || !m_running) return;
    m_dropTimer += dt;
    if (m_dropTimer >= m_dropInterval) {
        m_dropTimer = 0.0f;
        auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
        Point newPos = m_active.position + Point{0,1};
        if (m_board.isValidPosition(blocks, newPos)) {
            m_active.position = newPos;
        } else {
            // lock piece
            lockPiece();
        }
    }
}

void Game::lockPiece() {
    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
    m_board.place(blocks, m_active.position, static_cast<int>(m_active.type));
    int cleared = m_board.clearFullLines();
    m_score += cleared * 100;
    spawnPiece();
}

void Game::hardDrop() {
    auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
    Point pos = m_active.position;
    while (m_board.isValidPosition(blocks, pos + Point{0,1})) pos.y += 1;
    m_active.position = pos;
    lockPiece();
}

void Game::render() {
    m_window.clear(sf::Color::Black);

    // draw board
    for (int y = 0; y < BoardHeight; ++y) {
        for (int x = 0; x < BoardWidth; ++x) {
            int c = m_board.at(x,y).color;
            if (c != -1) {
                sf::RectangleShape r(sf::Vector2f(CellSize - 2, CellSize - 2));
                r.setFillColor(colors[c]);
                r.setPosition(sf::Vector2f(static_cast<float>(x * CellSize + 1), static_cast<float>(y * CellSize + 1)));
                m_window.draw(r);
            }
        }
    }

    // draw active piece
    if (m_running) {
        auto blocks = Tetromino::getShape(m_active.type, m_active.rotation)[m_active.rotation];
        for (const auto &b : blocks) {
            Point p = m_active.position + b;
            if (p.y >= 0) {
                sf::RectangleShape r(sf::Vector2f(CellSize - 2, CellSize - 2));
                r.setFillColor(colors[static_cast<int>(m_active.type)]);
                r.setPosition(sf::Vector2f(static_cast<float>(p.x * CellSize + 1), static_cast<float>(p.y * CellSize + 1)));
                m_window.draw(r);
            }
        }
    }

    // draw simple UI: score and next piece (if font loaded)
    if (m_fontLoaded) {
        std::stringstream ss;
        ss << "Score: " << m_score;
        sf::Text text(m_font, ss.str(), 16);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(static_cast<float>(BoardWidth * CellSize + 10), 10.f));
        m_window.draw(text);
    }

    if (!m_running) {
        sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(BoardWidth * CellSize), 60.f));
        overlay.setFillColor(sf::Color(0,0,0,160));
        overlay.setPosition(sf::Vector2f(0.f, static_cast<float>(BoardHeight * CellSize/2 - 30)));
        m_window.draw(overlay);
        if (m_fontLoaded) {
            sf::Text text(m_font, "Game Over - Press R to restart", 20);
            text.setFillColor(sf::Color::White);
            text.setPosition(sf::Vector2f(10.f, static_cast<float>(BoardHeight * CellSize / 2.f - 20.f)));
            m_window.draw(text);
        }
    }

    m_window.display();
}

void Game::run() {
    sf::Clock clock;
    while (m_window.isOpen()) {
        float dt = clock.restart().asSeconds();
        processInput();
        update(dt);
        render();
    }
}
