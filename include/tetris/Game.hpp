#pragma once

#include "Types.hpp"
#include "Tetromino.hpp"
#include "Board.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
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
    int m_level = 0;
    int m_totalLines = 0;

    // Line clear animation state
    std::vector<int> m_linesToClear;
    bool m_animating = false;
    float m_lineClearTimer = 0.0f;
    float m_lineClearDuration = 0.6f;

    // Audio
    sf::SoundBuffer m_clearBuffer;
    std::unique_ptr<sf::Sound> m_clearSound;
    bool m_clearSoundLoaded = false;
    // extra sound effects
    sf::SoundBuffer m_rotateBuffer;
    std::unique_ptr<sf::Sound> m_rotateSound;
    bool m_rotateSoundLoaded = false;

    sf::SoundBuffer m_dropBuffer;
    std::unique_ptr<sf::Sound> m_dropSound;
    bool m_dropSoundLoaded = false;

    sf::SoundBuffer m_hardDropBuffer;
    std::unique_ptr<sf::Sound> m_hardDropSound;
    bool m_hardDropSoundLoaded = false;

    float m_volume = 100.0f; // 0..100
    bool m_muted = false;
    // UI sounds and voice
    sf::SoundBuffer m_uiClickBuffer;
    std::unique_ptr<sf::Sound> m_uiClickSound;
    bool m_uiClickLoaded = false;

    sf::SoundBuffer m_voiceLevelBuffer;
    std::unique_ptr<sf::Sound> m_voiceLevelSound;
    bool m_voiceLevelLoaded = false;

    // Volume slider UI
    bool m_showVolumeSlider = true;
    sf::FloatRect m_volumeSliderRect; // in pixels, updated at render
    bool m_draggingVolume = false;
    sf::Font m_font;
    bool m_fontLoaded = false;

    static const std::array<sf::Color, static_cast<int>(TetrominoType::Count)> colors;
};

} // namespace tetris
