#include <SFML/Graphics.hpp>
#include "tetris/Game.hpp"

int main() {
    // Window sized to fit the board plus UI area
    int width = tetris::BoardWidth * tetris::CellSize + 200;
    int height = tetris::BoardHeight * tetris::CellSize;
    sf::VideoMode vm(sf::Vector2u((unsigned)width, (unsigned)height));
    sf::RenderWindow window(vm, "Tetris - test8");
    window.setFramerateLimit(60);

    tetris::Game game(window);
    game.run();

    return 0;
}
