#pragma once
#include <SFML/Graphics.hpp>
#include "palit_title.hpp"

namespace cr_lvl {

    palit_title::palit_title(sf::Vector2i size, sf::Vector2i pos, sf::RectangleShape* block, std::string text_name, sf::RenderWindow& window) {
        _text_name = text_name.substr(0, text_name.find_last_of('.'));
        _block = block;
        _window = &window;
        _pos = pos;
        _base.setFillColor(sf::Color(125, 131, 255));
        
    }
    void palit_title::draw(int pos_y) {
        _pos.y = pos_y;
        _base.setPosition(sf::Vector2f(_pos.x, _pos.y));
        _block->setPosition(sf::Vector2f(_pos.x + 37, _pos.y + 37));
        _window->draw(_base);
        _window->draw(*_block);
    }
    sf::Vector2i palit_title::getpos() {
        return _pos;
    };
    std::string palit_title::get_text_name() {
        return _text_name;
    };
    palit_title::~palit_title() {}
}