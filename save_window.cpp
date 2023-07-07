#pragma once
#include <SFML/Graphics.hpp>
#include "save_window.hpp"

namespace cr_lvl {

    save_file::save_file(std::string font_name, sf::RenderWindow& window) {
        _window = &window;
        _font.loadFromFile(font_name);
        _text.setFont(_font);
        _text.setStyle(sf::Text::Regular);
        _text.setColor(sf::Color(210, 210, 210));
        _text.setCharacterSize(25);
    };
    void save_file::draw(std::string text, sf::Vector2i pos) {
        _text.setString(text);
        _text.setPosition(pos.x, pos.y);
        _window->draw(_text);
    };
    void save_file::set_size(int size) {
        _text.setCharacterSize(size);
    };
    save_file::~save_file() {}


}