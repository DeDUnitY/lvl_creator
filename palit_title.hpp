#pragma once
#include "block.hpp"
#include <SFML/Graphics.hpp>


namespace cr_lvl {

    class palit_title
    {
    private:
        sf::RectangleShape* _block;
        sf::RenderWindow* _window;
        sf::RectangleShape _base = sf::RectangleShape(sf::Vector2f(200,74));
        std::string _text_name="";
        sf::Vector2i _pos = {0,0};
    public:
        palit_title::palit_title(sf::Vector2i size, sf::Vector2i pos, sf::RectangleShape *block, std::string text_name, sf::RenderWindow &window);
        void draw(int pos_y);
        sf::Vector2i getpos();
        std::string get_text_name();
        ~palit_title();
    };
}