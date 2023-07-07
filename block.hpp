#pragma once
#include <SFML/Graphics.hpp>

namespace cr_lvl {

    class text_drawler
    {
    private:
        sf::Font _font;
        sf::Text _text;
        sf::RenderWindow* _window;
    public:
        text_drawler::text_drawler(std::string font_name, sf::RenderWindow& window);
        void draw(std::string text, sf::Vector2i pos);
        void text_drawler::set_size(int size);
        ~text_drawler();
    };

    class Block
    {
    private:
        sf::RectangleShape* block;
        sf::Texture* texture = new sf::Texture;
    public:
        Block::Block(sf::Vector2i size, std::string text_name);
        sf::RectangleShape* return_block();
        ~Block();
    };
}