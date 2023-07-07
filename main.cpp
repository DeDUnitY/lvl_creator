#include <SFML/Graphics.hpp>
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

int main()
{
    std::string line;
    int start;
    sf::Vector2i window_size = { 1920,1080 }; // размер окна
    sf::Vector2i block_size = { 50,50 }; // размер блока
    std::vector<cr_lvl::Block> blocks;
    std::vector<std::string> files;
    bool flag_stop = true;
    bool mouse_left_togle = false; // нажатие левой клавиши мыши
    bool mouse_right_togle = false; // нажатие правой клавиши мыши
    int color = 1; // текущий цвет
    int temp;
    sf::Vector2i size_palit_title_block = { 250, 60 }; // размер подложки цвета
    sf::Vector2i temp_pos;
    sf::Vector2f delta_view;
    sf::Vector2i mouse_position;
    sf::Vector2f alt_pos_viev;
    sf::Vector2f rel_pos_viev;
    int page = 0;
    int rotation = 0;
    int page_count;

    // кнопка сохранения

    sf::Texture save_texture; 
    save_texture.loadFromFile("./icon/save.png");
    sf::RectangleShape save_button(sf::Vector2f(block_size.x - 2, block_size.y - 2));
    save_button.setPosition(window_size.x - 115, 5);
    save_button.setOrigin(-2, -2);
    save_button.setOutlineColor(sf::Color(89, 101, 111));
    save_button.setOutlineThickness(1);
    save_button.setTexture(&save_texture);

    // кнопка загрузки
    sf::Texture load_texture;
    load_texture.loadFromFile("./icon/load.png");
    sf::RectangleShape load_button(sf::Vector2f(block_size.x - 2, block_size.y - 2));
    load_button.setPosition(window_size.x - 175, 5);
    load_button.setOrigin(-2, -2);
    load_button.setOutlineColor(sf::Color(89, 101, 111));
    load_button.setOutlineThickness(1);
    load_button.setTexture(&load_texture);

    // Поиск тайтлов
    const std::filesystem::path my_path{ "./titles" };
    for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{ my_path }){
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
        corr_window = { 0,0 };
    window.setFramerateLimit(1200);
    sf::View view = window.getDefaultView();
    window.setView(view);
    delta_view = { view.getCenter().x, view.getCenter().y };

    cr_lvl::text_drawler texter("font.ttf", window);

    // Выделение блока
    sf::RectangleShape selection(sf::Vector2f(block_size.x-4, block_size.y-4));
    selection.setOrigin(-2, -2);
    selection.setFillColor(sf::Color(0, 0, 0, 0));
    selection.setOutlineColor(sf::Color::Yellow);
    selection.setOutlineThickness(2);

    sf::RectangleShape button(sf::Vector2f(block_size.x - 4, block_size.y - 4));
    button.setOrigin(-2, -2);
    button.setFillColor(sf::Color(125, 131, 255));
    button.setOutlineColor(sf::Color(89, 101, 111));
    button.setOutlineThickness(1);

    sf::RectangleShape cube(sf::Vector2f(24, 24));
    cube.setFillColor(sf::Color(0,0,0));
    cube.setOutlineColor(sf::Color(0, 0, 0));
    cube.setOutlineThickness(2);

    // Панель тайтлов
    sf::RectangleShape palit_title_block(sf::Vector2f(size_palit_title_block.x, window_size.y - size_palit_title_block.y-2));
    palit_title_block.setPosition(0, size_palit_title_block.y+2);
    palit_title_block.setFillColor(sf::Color(130, 212, 187));
    palit_title_block.setOutlineThickness(3);
    palit_title_block.setOutlineColor(sf::Color(89, 101, 111));
    sf::Vector2i size_title_block = { block_size.x * 2, block_size.y * 3 };

    // верхняя панель
    sf::RectangleShape tool_block(sf::Vector2f(window_size.x, size_palit_title_block.y));
    tool_block.setPosition(0, -1);
    tool_block.setFillColor(sf::Color(130, 212, 187));
    tool_block.setOutlineThickness(3);
    tool_block.setOutlineColor(sf::Color(89, 101, 111));

    std::vector<cr_lvl::palit_title> palit;
    for (int i = 0; i < files.size(); i++) {
        palit.push_back(cr_lvl::palit_title(size_title_block, { 25, 0 }, blocks[i].return_block(), files[i], window));
    }
    
    // Сетка
    bool flag_grid = false;
    sf::RectangleShape gridx(sf::Vector2f(window_size.x-size_palit_title_block.x, 2));
    gridx.setFillColor(sf::Color(130,130,130));
    sf::RectangleShape gridy(sf::Vector2f(2, window_size.y - size_palit_title_block.y));
    gridy.setFillColor(sf::Color(130, 130, 130));

    // Создание массива блоков
    std::vector<std::vector<int>> lvl_grid;
    lvl_grid.resize((window_size.x)/block_size.x+1);
    for (int i = 0; i < lvl_grid.size(); i++) {
        lvl_grid[i].resize((window_size.y) / block_size.y+1);
    }
    std::vector<std::vector<int>> blocks_rotate;
    blocks_rotate.resize((window_size.x) / block_size.x + 1);
    for (int i = 0; i < blocks_rotate.size(); i++) {
        blocks_rotate[i].resize((window_size.y) / block_size.y + 1);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
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
                if (event.key.code == sf::Keyboard::Up)
                    view.move(0.f, -50.f);
                if (event.key.code == sf::Keyboard::Down)
                    view.move(0.f, 50.f);
                if (event.key.code == sf::Keyboard::Left)
                    view.move(-50.f, 0.f);
                if (event.key.code == sf::Keyboard::Right)
                    view.move(50.f, 0.f);
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
        rel_pos_viev = { alt_pos_viev.x - delta_view.x,alt_pos_viev.y - delta_view.y, };

        // Позиция мыши
        mouse_position = sf::Mouse::getPosition();
        mouse_position = { mouse_position.x + (int)rel_pos_viev.x - window.getPosition().x + corr_window.x, mouse_position.y + (int)rel_pos_viev.y - window.getPosition().y + corr_window.y };
        // Рисование
        temp_pos = { (mouse_position.x - size_palit_title_block.x) / block_size.x,(mouse_position.y - size_palit_title_block.y) / block_size.y };
        if (mouse_position.x > size_palit_title_block.x && temp_pos.x < lvl_grid.size() && mouse_position.y > size_palit_title_block.y && (mouse_position.y - size_palit_title_block.y) / block_size.y < lvl_grid[0].size()) {
            //window.setMouseCursorVisible(false);
            selection.setPosition(temp_pos.x* block_size.x + size_palit_title_block.x, temp_pos.y * block_size.y + size_palit_title_block.y);
            if (0 <= mouse_position.x / block_size.x + 1 <= lvl_grid.size() && 0 <= mouse_position.y / block_size.y + 1 <= lvl_grid[0].size()) {
                if (mouse_left_togle) {
                    lvl_grid[temp_pos.x][temp_pos.y] = color;
                    blocks_rotate[temp_pos.x][temp_pos.y] = rotation;
                }
                if (mouse_right_togle)
                    lvl_grid[temp_pos.x][temp_pos.y] = 0;
            }
            
        }
        else selection.setPosition((int)rel_pos_viev.x, (int)rel_pos_viev.y);

        // Отрисовка
        window.clear(sf::Color(196, 177, 174));//sf::Color(255,255,255));
        for (int i = 0; i < lvl_grid.size(); i++)
            for (int j = 0; j < lvl_grid[i].size(); j++) {
                temp = lvl_grid[i][j];
                if (0 < temp && blocks.size() >= temp) {
                    blocks[temp - 1].return_block()->setPosition((i * block_size.x + size_palit_title_block.x + 25), j * block_size.y + size_palit_title_block.y + 25);
                    blocks[temp - 1].return_block()->rotate(90 * blocks_rotate[i][j]);
                    window.draw(*blocks[temp - 1].return_block());
                    blocks[temp - 1].return_block()->rotate(-90 * blocks_rotate[i][j]);
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

        window.draw(selection);
        window.draw(tool_block);
        // Отрисовка и выбор тайтлов
        window.draw(palit_title_block);
        for (int i = 8 * page; i < palit.size() && i < page * 8 + 8; i++) {
            if (check_colision(palit[i].getpos(), { 200,74 }, mouse_position) && mouse_left_togle)
                color = i + 1;
            palit[i].draw(87 + (i - 8 * page) * 100);
            texter.draw(palit[i].get_text_name(), { palit[i].getpos().x + 70,palit[i].getpos().y + 20 });
        }
        // Выбор страницы
        button.setFillColor(sf::Color(125, 131, 255));
        for (int i = 0; i < page_count; i++) {
            temp_pos = { 24 + 55 * i - i/4 * 220, 870 + (i/4 * 55) };
            button.setPosition(temp_pos.x, temp_pos.y);
            window.draw(button);
            texter.set_size(40);
            texter.draw(std::to_string(i + 1), { temp_pos.x + 16,temp_pos.y - 2 });
            texter.set_size(25);
            if (check_colision(temp_pos, { 50,50 }, mouse_position) && mouse_left_togle)
                page = i;
        }
        // Кнопка выхода
        button.setFillColor(sf::Color(255, 60, 70));
        button.setPosition(window_size.x - 55, 5);
        window.draw(button);
        texter.set_size(40);
        texter.draw("X", {window_size.x - 40, 2});
        texter.set_size(25);
        if (check_colision({ window_size.x - 55, 5 }, { 50,50 }, mouse_position) && mouse_left_togle)
            return 0;
        if (check_colision({ window_size.x - 115, 5 }, { 50,50 }, mouse_position) && mouse_left_togle) {
            std::ofstream out("test.txt");
            out << lvl_grid.size() << lvl_grid[0].size()<<std::endl;
            for (int i = 0; i < lvl_grid.size(); i++){
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
        if (check_colision({ window_size.x - 175, 5 }, { 50,50 }, mouse_position) && mouse_left_togle) {
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
            
        window.draw(save_button);
        window.draw(load_button);
        /*button.setTexture();
        button.setPosition(5, 5);
        cube.setPosition(18, 18);
        window.draw(button);
        window.draw(cube);
        button.setFillColor(sf::Color(120, 120, 120));
        button.setPosition(65, 5);
        cube.setFillColor(sf::Color(0, 0, 0, 0));
        cube.setPosition(78, 18);
        window.draw(button);
        window.draw(cube);
        cube.setFillColor(sf::Color(0, 0, 0));*/
        window.display();
        
    }
    return 0;
}