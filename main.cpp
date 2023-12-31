﻿#include <SFML/Graphics.hpp>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <thread>
#include <time.h>
#include "block.hpp"
#include "palit_title.hpp"
#include <fstream>


// min + (rand() % static_cast<int>(max - min + 1))
bool check_colision(sf::Vector2i pos_block, sf::Vector2i size_object, sf::Vector2i colision_object) {
    if (colision_object.x > pos_block.x && colision_object.x < pos_block.x + size_object.x) {
        if (colision_object.y > pos_block.y && colision_object.y < pos_block.y + size_object.y)
            return true;
    }
    return false;
}

class Button {
    sf::Texture* _texture = new sf::Texture;
    sf::RectangleShape* _button;
    std::string _name;
    sf::Vector2f _pos = {0,0};
public:
    Button(sf::Vector2f block_size, sf::Vector2f pos, sf::Color color, sf::Color out_color = sf::Color(89, 101, 111)) {
        _pos = pos;
        _button = new sf::RectangleShape(sf::Vector2f(block_size.x - 2, block_size.y - 2));
        _button->setPosition(_pos.x , _pos.y);
        _button->setFillColor(color);
        _button->setOrigin(-2, -2);
        _button->setOutlineColor(out_color);
        _button->setOutlineThickness(1);
    }
    Button(sf::Vector2f block_size, sf::Vector2f pos, std::string name, sf::Color out_color = sf::Color(89, 101, 111)) {
        _pos = pos;
        _name = name;
        _texture->loadFromFile("./"+ name);
        _button = new sf::RectangleShape(sf::Vector2f(block_size.x - 2, block_size.y - 2));
        _button->setPosition(_pos.x, _pos.y);
        _button->setOrigin(-2, -2);
        _button->setOutlineColor(out_color);
        _button->setOutlineThickness(1);
        _button->setTexture(_texture);
    }
    void move(sf::Vector2f pos) {
        _pos += pos;
        _button->setPosition(_button->getPosition() + pos);
    }

    void set_outline(int size) {
        _button->setOutlineThickness(size);
    }

    sf::Vector2f get_pos() {
        return _button->getPosition();
    }

    sf::RectangleShape* ret() {
        return _button;
    }
};

class Instrument{
    public:
    std::vector<cr_lvl::palit_title> _palit;
    std::vector<Button> _buttons;
    sf::RenderWindow* _window;
    cr_lvl::text_drawler* _texter;
    
    sf::RectangleShape* _palit_title_block;
    sf::Vector2i _pos = { 0,0 }; 
    Instrument(sf::Vector2i pos, sf::RenderWindow* window, std::vector<Button> buttons, cr_lvl::text_drawler *texter) {
        _texter = texter;
        _buttons = buttons;
        _window = window;
        _pos = pos;
    }

    void move(sf::Vector2i pos) {
        _pos += pos;
        //for (int i = 0; i < _palit.size(); i++) {
        //    _palit[i].move(_palit[i].getpos() + pos);
        //}
        //for (int i = 0; i < _buttons.size(); i++) {
        //    std::cout << 1 << std::endl;
        //    _buttons[i].move(sf::Vector2f(pos));
        //}
        
    }
    void draw() {
        for (int i = 0; i < _buttons.size(); i++) {
            _window->draw(*_buttons[i].ret());
        }
        for (int i = 0; i < _palit.size(); i++) {
            _palit[i].draw();
            _texter->draw(_palit[i].get_text_name(), { _palit[i].getpos().x + 70, _palit[i].getpos().y + 20 });
        }
        
    }

    std::vector <cr_lvl::palit_title>& return_palit() {
        return _palit;
    }
};

void fill_matrix(std::vector<std::vector<int>>& matr, sf::Vector2i start, sf::Vector2i end, int num, bool box) {
    for(int i = start.x; i <= end.x && i< matr.size(); i++){
        for (int j = start.y; j <= end.y && j < matr[0].size(); j++) {
            if ((box && (j== start.y|| j == end.y))|| !box)
                matr[i][j] = num;
            else {
                matr[start.x][j] = num;
                matr[end.x][j] = num;
            }
        }
    }
};

int main() {
    std::string line;
    int start;
    sf::Vector2i window_size = { 1920, 1080 }; // размер окна
    sf::Vector2i block_size = { 50, 50 }; // размер блока
    std::vector<cr_lvl::Block> blocks;
    std::vector<std::string> files;
    bool flag_stop = true;
    bool mouse_left_togle = false; // нажатие левой клавиши мыши
    bool mouse_right_togle = false; // нажатие правой клавиши мыши
    int color = 1; // текущий цвет
    int instrument= 0;
    int temp;
    std::string name_file;
    sf::Vector2i pos_canvas = {0,0};
    sf::Vector2i size_palit_title_block = { 250, 60 }; // размер подложки цвета
    sf::Vector2i temp_pos;
    sf::Vector2f delta_view;
    sf::Vector2i mouse_position;
    sf::Vector2f alt_pos_viev;
    sf::Vector2f rel_pos_viev;
    int page = 0;
    int rotation = 0;
    int page_count;
    sf::Clock timer;
    double fps = 60;

    // Поиск тайтлов
    const std::filesystem::path my_path{ "./titles" };
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ my_path }) {
        if (dir_entry.is_regular_file())
            files.push_back(dir_entry.path().string());
    }
    for (int i = 0; i < files.size(); i++) {
        if (files[i].find_last_of('\\') != std::string::npos)
            start = files[i].find_last_of('\\') + 1;
        else start = 2;
        files[i] = files[i].substr(start, files[i].size() - 1);
    }

    for (int i = 0; i < files.size(); i++) {
        blocks.push_back(cr_lvl::Block(block_size, files[i]));
    }

    if (blocks.size() % 8 != 0)
        page_count = blocks.size() / 8 + 1;
    else page_count = blocks.size() / 8;

    // создание окна
    sf::RenderWindow window(sf::VideoMode(window_size.x, window_size.y), "Lvl creator", sf::Style::Fullscreen);
    sf::Vector2i corr_window = { 0, 0 };//{ -10, -30 };
    if (window.getSize().y == 1080)
        corr_window = { 0, 0 };
    //window.setFramerateLimit(1200);
    sf::View view = window.getDefaultView();
    window.setView(view);
    delta_view = { view.getCenter().x, view.getCenter().y };

    cr_lvl::text_drawler texter("font.ttf", window);
    
    // выделение размера холста
    sf::RectangleShape canvas(sf::Vector2f(window_size.x-10, window_size.y-20));
    canvas.setFillColor(sf::Color(0, 0, 0, 0));
    canvas.setOutlineColor(sf::Color::Black);
    canvas.setOutlineThickness(4);
    canvas.setPosition(sf::Vector2f( size_palit_title_block.x+6, size_palit_title_block.y+7 ));
    // Выделение блока
    sf::RectangleShape selection(sf::Vector2f(block_size.x - 4, block_size.y - 4));
    selection.setOrigin(-2, -2);
    selection.setFillColor(sf::Color(0, 0, 0, 0));
    selection.setOutlineColor(sf::Color::Yellow);
    selection.setOutlineThickness(2);

    sf::RectangleShape cube(sf::Vector2f(24, 24));
    cube.setFillColor(sf::Color(0, 0, 0));
    cube.setOutlineColor(sf::Color(0, 0, 0));
    cube.setOutlineThickness(2);

    //Button button(sf::Vector2f(block_size.x-2, block_size.y-2), sf::Vector2f(window_size.x - 115, 5), "icon/save.png", sf::Color(89, 101, 111));
    sf::RectangleShape button(sf::Vector2f(block_size.x - 2, block_size.y - 2));
    button.setOrigin(-2, -2);
    button.setFillColor(sf::Color(125, 131, 255));
    button.setOutlineColor(sf::Color(89, 101, 111));
    button.setOutlineThickness(1);

    // кнопка сохранения
    Button save_button(sf::Vector2f(block_size.x, block_size.y), sf::Vector2f(window_size.x - 115, 5), "icon/save.png", sf::Color(89, 101, 111));

    // кнопка загрузки
    Button load_button(sf::Vector2f(block_size.x, block_size.y), sf::Vector2f(window_size.x - 175, 5), "icon/load.png", sf::Color(89, 101, 111));
    Button tool_button1(sf::Vector2f(block_size.x, block_size.y), sf::Vector2f(300, 5), "icon/1tool.png", sf::Color(89, 101, 111));
    // Панель тайтлов
    Button palit_title_block(sf::Vector2f(size_palit_title_block.x, window_size.y - size_palit_title_block.y - 2), sf::Vector2f(0, size_palit_title_block.y + 2), sf::Color(130, 212, 187), sf::Color(89, 101, 111));
    palit_title_block.set_outline(3);
    sf::Vector2i size_title_block = { block_size.x * 2, block_size.y * 3 };

    // верхняя панель
    Button tool_block(sf::Vector2f(window_size.x+2, size_palit_title_block.y+2), sf::Vector2f(0, -1), sf::Color(130, 212, 187), sf::Color(89, 101, 111));
    tool_block.set_outline(3);
    
    // Панели инструментов
    Instrument new_tools({ 0,0 }, &window, { palit_title_block, tool_block, save_button, load_button }, &texter);
    for (int i = 0; i < files.size(); i++) {
        new_tools.return_palit().push_back(cr_lvl::palit_title(size_title_block, { 25 , 87 + i * 100 - i / 8 * 800 }, blocks[i].return_block(), files[i], window));
    }
    // Сетка
    bool flag_grid = false;
    sf::RectangleShape gridx(sf::Vector2f(window_size.x, 2));
    gridx.setFillColor(sf::Color(130, 130, 130));
    sf::RectangleShape gridy(sf::Vector2f(2, window_size.y));
    gridy.setFillColor(sf::Color(130, 130, 130));

    // Создание массива блоков
    std::vector<std::vector<int>> lvl_grid;
    lvl_grid.resize((window_size.x) / block_size.x + 1);
    for (int i = 0; i < lvl_grid.size(); i++) {
        lvl_grid[i].resize((window_size.y) / block_size.y + 1);
    }
    fill_matrix(lvl_grid, {3,3}, {6,5}, 2, false);

    std::vector<std::vector<int>> blocks_rotate;
    blocks_rotate.resize((window_size.x) / block_size.x + 1);
    for (int i = 0; i < blocks_rotate.size(); i++) {
        blocks_rotate[i].resize((window_size.y) / block_size.y + 1);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape)
                    window.close();
                if (event.key.code == sf::Keyboard::Space)
                    flag_stop = !flag_stop;
                if (event.key.code == sf::Keyboard::G)
                    flag_grid = !flag_grid;
                if (event.key.code == sf::Keyboard::E)
                    rotation += 1;
                if (event.key.code == sf::Keyboard::Q)
                    rotation -= 1;
                // Перемещение камеры
                if (event.key.code == sf::Keyboard::Up && pos_canvas.y!=0){
                    pos_canvas.y -= 1;
                    canvas.setPosition(canvas.getPosition() + sf::Vector2f(0, 1 * block_size.x));
                    //view.move(0.f, -70.f);
                    //new_tools.move({ 0, -70 });
                }
                if (event.key.code == sf::Keyboard::Down && pos_canvas.y <1){
                    pos_canvas.y += 1;
                    canvas.setPosition(canvas.getPosition() + sf::Vector2f(0, -1 * block_size.x));
                    //view.move(0.f, 70.f);
                    //new_tools.move({ 0, 70 });
                }
                if (event.key.code == sf::Keyboard::Left && pos_canvas.x != 0){
                    pos_canvas.x -= 5;
                    canvas.setPosition(canvas.getPosition() + sf::Vector2f(5 * block_size.x, 0));
                    //view.move(-260.f, 0.f);
                    //new_tools.move({ -260, 0 });
                }
                if (event.key.code == sf::Keyboard::Right && pos_canvas.x <5){
                    pos_canvas.x += 5;
                    canvas.setPosition(canvas.getPosition() + sf::Vector2f(-5 * block_size.x, 0));
                    //view.move(260.f, 0.f);
                    //new_tools.move({ 260, 0 });
                }
                window.setView(view);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
                for (int i = 0; i < lvl_grid.size(); i++) {
                    for (int j = 0; j < lvl_grid[i].size(); j++) {
                        lvl_grid[i][j] = 0;
                    }
                }
            }
        }
        // Проверка нажатия мыши
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
            mouse_left_togle = true;
        else
            mouse_left_togle = false;

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
            mouse_right_togle = true;
        else
            mouse_right_togle = false;

        // Позиция камеры
        alt_pos_viev = view.getCenter();
        rel_pos_viev = { alt_pos_viev.x - delta_view.x, alt_pos_viev.y - delta_view.y, };

        // Позиция мыши
        mouse_position = sf::Mouse::getPosition(window);
        //mouse_position = { mouse_position.x + (int)rel_pos_viev.x, mouse_position.y + (int)rel_pos_viev.y};
        // Рисование
        temp_pos = { (mouse_position.x - size_palit_title_block.x) / block_size.x+ pos_canvas.x,
                    (mouse_position.y - size_palit_title_block.y) / block_size.y +pos_canvas.y };
        if (mouse_position.x > size_palit_title_block.x && temp_pos.x < lvl_grid.size() &&
            mouse_position.y > size_palit_title_block.y &&
            (mouse_position.y - size_palit_title_block.y) / block_size.y < lvl_grid[0].size()) {
            //window.setMouseCursorVisible(false);
            selection.setPosition(((temp_pos.x - pos_canvas.x) * block_size.x + size_palit_title_block.x),
                (temp_pos.y - pos_canvas.y) * block_size.y + size_palit_title_block.y);
            if (0 <= mouse_position.x / block_size.x + 1 <= lvl_grid.size() &&
                0 <= mouse_position.y / block_size.y + 1 <= lvl_grid[0].size()) {
                if (mouse_left_togle) {
                    lvl_grid[temp_pos.x][temp_pos.y] = color;
                    blocks_rotate[temp_pos.x][temp_pos.y] = rotation;
                }
                if (mouse_right_togle)
                    lvl_grid[temp_pos.x][temp_pos.y] = 0;
            }

        }
        else selection.setPosition((int)rel_pos_viev.x, (int)rel_pos_viev.y);

        if (check_colision(sf::Vector2i(load_button.get_pos()), { 50, 50 }, mouse_position) && mouse_left_togle) {
            std::ifstream in("test.txt");
            in >> line;
            for (int i = 0; i < lvl_grid.size(); i++) {
                for (int j = 0; j < lvl_grid[0].size(); j++)
                    in >> lvl_grid[i][j];
            }
            in >> line;
            for (int i = 0; i < blocks_rotate.size(); i++) {
                for (int j = 0; j < blocks_rotate[0].size(); j++)
                    in >> blocks_rotate[i][j];
            }
        }
        if (check_colision(sf::Vector2i(save_button.get_pos()), { 50, 50 }, mouse_position) && mouse_left_togle) {
            std::ofstream out("test.txt");
            out << lvl_grid.size() << lvl_grid[0].size() << std::endl;
            for (int i = 0; i < lvl_grid.size(); i++) {
                for (int j = 0; j < lvl_grid[0].size(); j++)
                    out << lvl_grid[i][j] << " ";
                out << "\n";
            }
            out << blocks_rotate.size() << blocks_rotate[0].size() << std::endl;
            for (int i = 0; i < blocks_rotate.size(); i++) {
                for (int j = 0; j < blocks_rotate[0].size(); j++)
                    out << blocks_rotate[i][j] << " ";
                out << "\n";
            }
        }
        // выбор тайтлов
        for (int i = 8 * page; i < new_tools.return_palit().size() && i < page * 8 + 8; i++)
            if (check_colision(new_tools.return_palit()[i].getpos(), { 200, 74 }, mouse_position) && mouse_left_togle)
                color = i + 1;
        if (check_colision({ window_size.x - 55 + new_tools._pos.x, 5 + new_tools._pos.y }, { 50, 50 }, mouse_position) && mouse_left_togle)
            return 0;

        if (timer.getElapsedTime().asMilliseconds() >= 1000/fps)
        {
            timer.restart();
        
        // Отрисовка
        window.clear(sf::Color(196, 177, 174));//sf::Color(255,255,255));
        for (int i = pos_canvas.x; i < lvl_grid.size(); i++)
            for (int j = pos_canvas.y; j < lvl_grid[i].size(); j++) {
                temp = lvl_grid[i][j]-1;
               if (0 <= temp && blocks.size() >= temp) {
                    blocks[temp].return_block()->setPosition(((i- pos_canvas.x) * block_size.x + size_palit_title_block.x + 25),
                        (j- pos_canvas.y) * block_size.y + size_palit_title_block.y + 25);
                    blocks[temp].return_block()->rotate(90 * blocks_rotate[i][j]);
                    window.draw(*blocks[temp].return_block());
                    blocks[temp].return_block()->rotate(-90 * blocks_rotate[i][j]);
                }
            }

       
        // Сетка
        if (flag_grid) {
            for (int i = 0; i < lvl_grid.size(); i++) {
                gridy.setPosition(size_palit_title_block.x + i * block_size.x, size_palit_title_block.y);
                window.draw(gridy);
            }
            for (int i = 0; i < lvl_grid[0].size(); i++) {
                gridx.setPosition(size_palit_title_block.x, size_palit_title_block.y + i * block_size.y);
                window.draw(gridx);
            }
        }

        window.draw(canvas);
        window.draw(selection);
        new_tools.draw();
        
        // Выбор страницы
        button.setFillColor(sf::Color(125, 131, 255));
        for (int i = 0; i < page_count; i++) {
            temp_pos = { 24 + 55 * i - i / 4 * 220 + new_tools._pos.x, 870 + (i / 4 * 55) + new_tools._pos.y };
            button.setPosition(temp_pos.x, temp_pos.y);
            window.draw(button);
            texter.set_size(40);
            texter.draw(std::to_string(i + 1), { temp_pos.x + 16, temp_pos.y - 2 });
            texter.set_size(25);
            if (check_colision(temp_pos, { 50, 50 }, mouse_position) && mouse_left_togle)
                page = i;
        }
        // Кнопка выхода
        button.setFillColor(sf::Color(255, 60, 70));
        button.setPosition(window_size.x - 55 + new_tools._pos.x, 5 + new_tools._pos.y);
        window.draw(button);
        texter.set_size(40);
        texter.draw("X", { window_size.x - 40 + new_tools._pos.x, 2 + new_tools._pos.y});
        texter.set_size(25);
        
        
        button.setPosition(8, 5);
        button.setFillColor(sf::Color(195, 195, 195));
        cube.setPosition(21, 18);
        window.draw(button);
        window.draw(cube);
        button.setPosition(63, 5);
        cube.setFillColor(sf::Color(0, 0, 0, 0));
        cube.setPosition(76, 18);
        window.draw(button);
        window.draw(cube);
        cube.setFillColor(sf::Color(0, 0, 0));
        window.draw(*save_button.ret());
        window.draw(*load_button.ret());
        window.draw(*tool_button1.ret());
        window.display();
        }
    }
    return 0;
}