#pragma once
#include <SFML/Graphics.hpp>
#include "block.hpp"

namespace cr_lvl {

    text_drawler::text_drawler(std::string font_name, sf::RenderWindow& window) {
        _window = &window;
        _font.loadFromFile(font_name);
        _text.setFont(_font);
        _text.setStyle(sf::Text::Regular);
        _text.setColor(sf::Color(210, 210, 210));
        _text.setCharacterSize(25);
    };
    void text_drawler::draw(std::string text, sf::Vector2i pos) {
        _text.setString(text);
        _text.setPosition(pos.x, pos.y);
        _window->draw(_text);
    };
    void text_drawler::set_size(int size) {
        _text.setCharacterSize(size);
    };
    text_drawler::~text_drawler() {}



    Block::Block(sf::Vector2i size, std::string text_name) {
        block = new sf::RectangleShape(sf::Vector2f(size.x, size.y));
        texture->loadFromFile("./titles/" + text_name);
        block->setTexture(texture);
        block->setOrigin(25, 25);
    };
    sf::RectangleShape* Block::return_block(){ return block; };
    Block::~Block() {}
}