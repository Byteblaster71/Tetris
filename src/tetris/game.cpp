#include "../../include/tetris/Game.hpp"
#include <chrono>
#include <sstream>
#include <cmath>
#include <algorithm>

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
    // Try to load sound effects from assets (optional)
    m_clearSoundLoaded = m_clearBuffer.loadFromFile("assets/clear.wav");
    if (m_clearSoundLoaded) {
        m_clearSound = std::make_unique<sf::Sound>(m_clearBuffer);
        m_clearSound->setVolume(m_volume);
    }
    m_rotateSoundLoaded = m_rotateBuffer.loadFromFile("assets/rotate.wav");
    if (m_rotateSoundLoaded) {
        m_rotateSound = std::make_unique<sf::Sound>(m_rotateBuffer);
        m_rotateSound->setVolume(m_volume);
    }
    m_dropSoundLoaded = m_dropBuffer.loadFromFile("assets/drop.wav");
    if (m_dropSoundLoaded) {
        m_dropSound = std::make_unique<sf::Sound>(m_dropBuffer);
        m_dropSound->setVolume(m_volume);
    }
    m_hardDropSoundLoaded = m_hardDropBuffer.loadFromFile("assets/harddrop.wav");
    if (m_hardDropSoundLoaded) {
        m_hardDropSound = std::make_unique<sf::Sound>(m_hardDropBuffer);
        m_hardDropSound->setVolume(m_volume);
    }
    // UI and voice sounds
    m_uiClickLoaded = m_uiClickBuffer.loadFromFile("assets/ui_click.wav");
    if (m_uiClickLoaded) { m_uiClickSound = std::make_unique<sf::Sound>(m_uiClickBuffer); m_uiClickSound->setVolume(m_volume); }
    m_voiceLevelLoaded = m_voiceLevelBuffer.loadFromFile("assets/voice_levelup.wav");
    if (m_voiceLevelLoaded) { m_voiceLevelSound = std::make_unique<sf::Sound>(m_voiceLevelBuffer); m_voiceLevelSound->setVolume(m_volume); }
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

            // In-game controls
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
                    if (m_board.isValidPosition(blocks, newPos)) {
                        m_active.position = newPos;
                        if (m_dropSoundLoaded && m_dropSound) m_dropSound->play();
                    }
                } else if (key == sf::Keyboard::Key::Up) {
                    // rotate
                    int newRot = (m_active.rotation + 1) & 3;
                    auto blocks = Tetromino::getShape(m_active.type, newRot)[newRot];
                    if (m_board.isValidPosition(blocks, m_active.position)) {
                        m_active.rotation = newRot;
                        if (m_rotateSoundLoaded && m_rotateSound) m_rotateSound->play();
                    }
                } else if (key == sf::Keyboard::Key::Space) {
                    hardDrop();
                    if (m_hardDropSoundLoaded && m_hardDropSound) m_hardDropSound->play();
                }
            }

            // Restart after game over
            if (!m_running && key == sf::Keyboard::Key::R) {
                m_board = Board();
                m_running = true;
                m_paused = false;
                m_score = 0;
                spawnPiece();
            }

            // Global input for audio controls
            if (key == sf::Keyboard::Key::M) {
                m_muted = !m_muted;
                float v = m_muted ? 0.0f : m_volume;
                if (m_clearSound && m_clearSoundLoaded) m_clearSound->setVolume(v);
                if (m_rotateSound && m_rotateSoundLoaded) m_rotateSound->setVolume(v);
                if (m_dropSound && m_dropSoundLoaded) m_dropSound->setVolume(v);
                if (m_hardDropSound && m_hardDropSoundLoaded) m_hardDropSound->setVolume(v);
                if (m_uiClickSound && m_uiClickLoaded) m_uiClickSound->setVolume(v);
                if (m_voiceLevelSound && m_voiceLevelLoaded) m_voiceLevelSound->setVolume(v);
            }
            if (key == sf::Keyboard::Key::LBracket) { // decrease volume
                m_volume = std::max(0.0f, m_volume - 10.0f);
                if (!m_muted) {
                    if (m_clearSound && m_clearSoundLoaded) m_clearSound->setVolume(m_volume);
                    if (m_rotateSound && m_rotateSoundLoaded) m_rotateSound->setVolume(m_volume);
                    if (m_dropSound && m_dropSoundLoaded) m_dropSound->setVolume(m_volume);
                    if (m_hardDropSound && m_hardDropSoundLoaded) m_hardDropSound->setVolume(m_volume);
                    if (m_uiClickSound && m_uiClickLoaded) { m_uiClickSound->setVolume(m_volume); m_uiClickSound->play(); }
                    if (m_voiceLevelSound && m_voiceLevelLoaded) m_voiceLevelSound->setVolume(m_volume);
                }
            }
            if (key == sf::Keyboard::Key::RBracket) { // increase volume
                m_volume = std::min(100.0f, m_volume + 10.0f);
                if (!m_muted) {
                    if (m_clearSound && m_clearSoundLoaded) m_clearSound->setVolume(m_volume);
                    if (m_rotateSound && m_rotateSoundLoaded) m_rotateSound->setVolume(m_volume);
                    if (m_dropSound && m_dropSoundLoaded) m_dropSound->setVolume(m_volume);
                    if (m_hardDropSound && m_hardDropSoundLoaded) m_hardDropSound->setVolume(m_volume);
                    if (m_uiClickSound && m_uiClickLoaded) { m_uiClickSound->setVolume(m_volume); m_uiClickSound->play(); }
                    if (m_voiceLevelSound && m_voiceLevelLoaded) m_voiceLevelSound->setVolume(m_volume);
                }
            }
        }
        
        // Mouse handling for volume slider
        if (event->is<sf::Event::MouseButtonPressed>()) {
            auto m = event->getIf<sf::Event::MouseButtonPressed>();
            if (m->button == sf::Mouse::Button::Left) {
                    sf::Vector2i pos = m->position;
                    if (m_volumeSliderRect.contains(sf::Vector2f(static_cast<float>(pos.x), static_cast<float>(pos.y)))) {
                        m_draggingVolume = true;
                        float rel = (pos.x - m_volumeSliderRect.position.x) / m_volumeSliderRect.size.x;
                        m_volume = std::clamp(rel * 100.0f, 0.0f, 100.0f);
                        if (!m_muted && m_uiClickSound && m_uiClickLoaded) m_uiClickSound->play();
                        // update volumes
                        if (!m_muted) {
                            if (m_clearSound && m_clearSoundLoaded) m_clearSound->setVolume(m_volume);
                            if (m_rotateSound && m_rotateSoundLoaded) m_rotateSound->setVolume(m_volume);
                            if (m_dropSound && m_dropSoundLoaded) m_dropSound->setVolume(m_volume);
                            if (m_hardDropSound && m_hardDropSoundLoaded) m_hardDropSound->setVolume(m_volume);
                            if (m_voiceLevelSound && m_voiceLevelLoaded) m_voiceLevelSound->setVolume(m_volume);
                        }
                    }
                }
        }
        if (event->is<sf::Event::MouseButtonReleased>()) {
            auto m = event->getIf<sf::Event::MouseButtonReleased>();
            if (m->button == sf::Mouse::Button::Left) m_draggingVolume = false;
        }
        if (event->is<sf::Event::MouseMoved>()) {
                if (m_draggingVolume) {
                auto mm = event->getIf<sf::Event::MouseMoved>();
                float mx = static_cast<float>(mm->position.x);
                float rel = (mx - m_volumeSliderRect.position.x) / m_volumeSliderRect.size.x;
                m_volume = std::clamp(rel * 100.0f, 0.0f, 100.0f);
                if (!m_muted) {
                    if (m_clearSound && m_clearSoundLoaded) m_clearSound->setVolume(m_volume);
                    if (m_rotateSound && m_rotateSoundLoaded) m_rotateSound->setVolume(m_volume);
                    if (m_dropSound && m_dropSoundLoaded) m_dropSound->setVolume(m_volume);
                    if (m_hardDropSound && m_hardDropSoundLoaded) m_hardDropSound->setVolume(m_volume);
                    if (m_uiClickSound && m_uiClickLoaded) m_uiClickSound->setVolume(m_volume);
                    if (m_voiceLevelSound && m_voiceLevelLoaded) m_voiceLevelSound->setVolume(m_volume);
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (m_paused || !m_running) return;

    // Handle line clear animation
    if (m_animating) {
        m_lineClearTimer += dt;
        if (m_lineClearTimer >= m_lineClearDuration) {
            // finalize clear
            m_board.removeLines(m_linesToClear);
            int n = static_cast<int>(m_linesToClear.size());
            static const int baseScore[] = {0,100,300,500,800};
            int gained = (n >=1 && n <=4) ? baseScore[n] * (m_level + 1) : 0;
            m_score += gained;
            m_totalLines += n;
            int oldLevel = m_level;
            m_level = m_totalLines / 10;
            if (m_level > oldLevel && m_voiceLevelLoaded && m_voiceLevelSound && !m_muted) m_voiceLevelSound->play();
            m_dropInterval = std::max(0.05f, 0.8f - m_level * 0.05f);
            m_linesToClear.clear();
            m_animating = false;
            spawnPiece();
        }
        return;
    }

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
    // Find full lines and start clear animation if any
    m_linesToClear = m_board.getFullLines();
    if (!m_linesToClear.empty()) {
        m_animating = true;
        m_lineClearTimer = 0.0f;
        if (m_clearSoundLoaded && m_clearSound) m_clearSound->play();
        return; // wait until animation completes to remove lines and score
    }
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

    // If animating line clear, draw pulsing overlay on clearing rows
    if (m_animating) {
        float t = m_lineClearTimer / m_lineClearDuration; // 0..1
        float alpha = 160.0f * (1.0f - std::cos(t * 3.14159f)); // fade in/out
        for (int y : m_linesToClear) {
            for (int x = 0; x < BoardWidth; ++x) {
                sf::RectangleShape r(sf::Vector2f(CellSize - 2, CellSize - 2));
                unsigned char a = static_cast<unsigned char>(std::clamp(alpha, 0.f, 255.f));
                sf::Color c(255,255,255,a);
                r.setFillColor(c);
                r.setPosition(sf::Vector2f(static_cast<float>(x * CellSize + 1), static_cast<float>(y * CellSize + 1)));
                m_window.draw(r);
            }
        }
    }

    // draw simple UI: score and next piece (if font loaded)
    if (m_fontLoaded) {
        std::stringstream ss;
        ss << "Score: " << m_score << "\nLevel: " << m_level << "\nLines: " << m_totalLines;
        sf::Text text(m_font, ss.str(), 16);
        text.setFillColor(sf::Color::White);
        text.setPosition(sf::Vector2f(static_cast<float>(BoardWidth * CellSize + 10), 10.f));
        m_window.draw(text);
    }

    // draw volume slider UI on the right side
    {
        float sx = static_cast<float>(BoardWidth * CellSize + 10);
        float sy = static_cast<float>(static_cast<int>(m_window.getSize().y) - 40);
        float sw = 160.0f;
        float sh = 12.0f;
        m_volumeSliderRect = sf::FloatRect(sf::Vector2f(sx, sy), sf::Vector2f(sw, sh));

        sf::RectangleShape bg(sf::Vector2f(sw, sh));
        bg.setFillColor(sf::Color(80,80,80));
        bg.setPosition(sf::Vector2f(sx, sy));
        m_window.draw(bg);

        float fillW = (m_volume / 100.0f) * sw;
        sf::RectangleShape fill(sf::Vector2f(fillW, sh));
        fill.setFillColor(sf::Color(200,200,200));
        fill.setPosition(sf::Vector2f(sx, sy));
        m_window.draw(fill);

        // knob
        sf::CircleShape knob(7.f);
        knob.setFillColor(sf::Color::White);
        knob.setPosition(sf::Vector2f(sx + std::max(0.f, fillW - 7.f), sy - 3.f));
        m_window.draw(knob);

        // volume text
        if (m_fontLoaded) {
            std::stringstream vs;
            if (m_muted) vs << "Muted"; else vs << static_cast<int>(m_volume) << "%";
            sf::Text vtext(m_font, vs.str(), 12);
            vtext.setFillColor(sf::Color::White);
            vtext.setPosition(sf::Vector2f(sx, sy - 20.f));
            m_window.draw(vtext);
        }
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
