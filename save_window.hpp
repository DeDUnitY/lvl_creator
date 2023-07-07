#pragma once
#include "block.hpp"
#include <SFML/Graphics.hpp>

namespace cr_lvl {

    class save_file
    {
    private:
        sf::Font _font;
        sf::Text _text;
        sf::RenderWindow* _window;
    public:
        save_file::save_file(std::string font_name, sf::RenderWindow& window);
        void draw(std::string text, sf::Vector2i pos);
        void save_file::set_size(int size);
        ~save_file();
    };

}