#include <iostream>
#include <fstream>
#include <cmath>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include "movement.h"

using namespace sf;
using namespace std;

int screen_x = 1136;
int screen_y = 896;


void display_level(RenderWindow& window, char lvl[][18], Texture& bgTex, Sprite& bgSprite, Texture& blockTexture, Sprite& blockSprite, Sprite& ladderSprite, const int height, const int width, const int cell_size)

{
    window.draw(bgSprite);

    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            if (lvl[i][j] == '#')
            {
                blockSprite.setPosition(j * cell_size, i * cell_size);
                window.draw(blockSprite);
            }
            else if (lvl[i][j] == 'H') 
            {
              ladderSprite.setPosition(j * cell_size, i * cell_size);
               window.draw(ladderSprite);
             }
        }
    }
}

void player_gravity(char lvl[][18], float& offset_y, float& velocityY, bool& onGround, const float& gravity, float& terminal_Velocity, float& player_x, float& player_y, const int cell_size, int& Pheight, int& Pwidth, const int height,bool isLadder)
{   if (isLadder) return;
    offset_y = player_y + velocityY;

    int left_col = (int)(player_x + 2) / cell_size;
    int mid_col = (int)(player_x + Pwidth / 2) / cell_size;
    int right_col = (int)(player_x + Pwidth - 2) / cell_size;

    // 1. Moving UPWARD (Jumping)
    if (velocityY < 0)
    {
        onGround = false;

        // Prevent jumping above the top of the screen
        if (offset_y <= 0)
        {
            player_y = 0;
            velocityY = 0;
            return;
        }

        int top_row = (int)offset_y / cell_size;
        if (lvl[top_row][left_col] == '#' || lvl[top_row][mid_col] == '#' || lvl[top_row][right_col] == '#')
        {
            // Hit head on ceiling block
            player_y = (top_row + 1) * cell_size;
            velocityY = 0;
        }
        else
        {
            player_y = offset_y;
        }
    }
    // 2. Moving DOWNWARD (Falling or Standing)
    else
    {
        int bottom_row = (int)(offset_y + Pheight) / cell_size;

        if (bottom_row < height &&
            (lvl[bottom_row][left_col] == '#' || lvl[bottom_row][mid_col] == '#' || lvl[bottom_row][right_col] == '#'))
        {
            onGround = true;
            velocityY = 0;
            // Snap cleanly onto the top of the floor/platform
            player_y = (bottom_row * cell_size) - Pheight;
        }
        else
        {
            player_y = offset_y;
            onGround = false;
        }
    }

    // Apply gravity while in the air
    if (!onGround)
    {
        velocityY += gravity;
        if (velocityY >= terminal_Velocity) velocityY = terminal_Velocity;
    }
}

int main()
{
    RenderWindow window(VideoMode(screen_x, screen_y), "VICE: Project Doom", Style::Resize);
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(60);

    // Level specifics
    const int cell_size = 64;
    const int height = 14;
    const int width = 18;
    char lvl[height][width];


    // Level and background textures and sprites
    Texture bgTex;
    Sprite bgSprite;
    Texture blockTexture;
    Sprite blockSprite;

    bgTex.loadFromFile("Data/bg.png");
    bgSprite.setTexture(bgTex);
    bgSprite.setPosition(0, 0);

    blockTexture.loadFromFile("Data/block1.png");
    blockSprite.setTexture(blockTexture);

    // Music initialisation
    Music lvlMusic;
    lvlMusic.openFromFile("Data/mus.ogg");
    lvlMusic.setVolume(20);
    lvlMusic.play();
    lvlMusic.setLoop(true);

    // Player data
    float player_x = 780;
    float player_y = 60;
    float player_speed = 6.0f;
    float player_scale = 3.0f;
    float jump_strength = 20.0f; // Increase or decrease to change jump height

    const float gravity = 1;

    Texture PlayerTexture;
    Texture PlayerDuckTexture;
    Sprite PlayerSprite;

    bool onGround = false;
    
    bool isLadder = false;
float climb_speed = 4.0f;

Texture ladderTexture;
Sprite ladderSprite;
ladderTexture.loadFromFile("Data/ladder.png");
ladderSprite.setTexture(ladderTexture);


    float offset_y = 0;
    float velocityY = 0;

    float terminal_Velocity = 20;

    int PlayerHeight = 96;
    int PlayerWidth = 54;
    int duckHeight = 48;
    int normal_height = 96;

    PlayerTexture.loadFromFile("Data/player.png");
    PlayerDuckTexture.loadFromFile("Data/player_duck.png");
    PlayerSprite.setTexture(PlayerTexture);
    PlayerSprite.setScale(player_scale, player_scale);
    PlayerSprite.setPosition(player_x, player_y);

    // Creating and initializing level array
    for (int i = 0; i < height; i += 1)
    {
        for (int j = 0; j < width; j += 1)
        {
            lvl[i][j] = ' ';
        }
    }

    // Bottom floor (row 13)
    for (int j = 0; j < width; j += 1)
    {
        lvl[13][j] = '#';
    }
    for (int i = 8; i <= 12; i++)
{
    lvl[i][5] = 'H';
}


    Event ev;
    // Main loop
    while (window.isOpen())
    {
        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
            {
                window.close();
            }
        }

        // Pressing escape to close
        if (Keyboard::isKeyPressed(Keyboard::Escape))
        {
            window.close();
        }

        window.clear();
        display_level(window, lvl, bgTex, bgSprite, blockTexture, blockSprite, ladderSprite, height, width, cell_size);


        // Call movement and jump from movement.h
        handle_up(lvl, player_x, player_y, velocityY, onGround, isLadder, jump_strength, climb_speed, cell_size, PlayerHeight, PlayerWidth);
handle_down(lvl, player_x, player_y, velocityY, PlayerHeight, normal_height, duckHeight, PlayerWidth, cell_size, height, onGround, isLadder, climb_speed, PlayerSprite, PlayerTexture, PlayerDuckTexture);
        player_movement(lvl, player_x, player_y, player_speed, cell_size, PlayerHeight, PlayerWidth, width, height, PlayerSprite, player_scale,duckHeight);
        player_gravity(lvl, offset_y, velocityY, onGround, gravity, terminal_Velocity, player_x, player_y, cell_size, PlayerHeight, PlayerWidth, height,isLadder);

        PlayerSprite.setPosition(player_x, player_y);
        window.draw(PlayerSprite);

        window.display();
    }

    // Stopping music and deleting level array
    lvlMusic.stop();
   

    return 0;
}

