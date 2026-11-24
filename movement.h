#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <SFML/Graphics.hpp>
using namespace sf;

void player_movement(char** lvl, float& player_x, float& player_y, float speed,
                     const int cell_size, int Pheight, int Pwidth,
                     const int width, const int height, Sprite& PlayerSprite, float scale)
{
    float offset_x = player_x;

    // Move Left (Left Arrow or A)
    if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
    {
        offset_x -= speed;

        // Flip sprite to face left
        PlayerSprite.setOrigin(PlayerSprite.getLocalBounds().width, 0);
        PlayerSprite.setScale(-scale, scale);

        // Check left screen boundary
        if (offset_x >= 0)
        {
            int left_col = (int)offset_x / cell_size;
            int top_row = (int)player_y / cell_size;
            int mid_row = (int)(player_y + Pheight / 2) / cell_size;
            int bottom_row = (int)(player_y + Pheight - 1) / cell_size;

            if (lvl[top_row][left_col] != '#' &&
                lvl[mid_row][left_col] != '#' &&
                lvl[bottom_row][left_col] != '#')
            {
                player_x = offset_x;
            }
        }
    }

    // Move Right (Right Arrow or D)
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
    {
        offset_x += speed;

        // Flip sprite to face right
        PlayerSprite.setOrigin(0, 0);
        PlayerSprite.setScale(scale, scale);

        // Check right screen boundary
        if (offset_x + Pwidth < width * cell_size)
        {
            int right_col = (int)(offset_x + Pwidth) / cell_size;
            int top_row = (int)player_y / cell_size;
            int mid_row = (int)(player_y + Pheight / 2) / cell_size;
            int bottom_row = (int)(player_y + Pheight - 1) / cell_size;

            if (lvl[top_row][right_col] != '#' &&
                lvl[mid_row][right_col] != '#' &&
                lvl[bottom_row][right_col] != '#')
            {
                player_x = offset_x;
            }
        }
    }
}
// jump
void player_jump(float& velocityY, bool& onGround, float jump_strength)
{
    // Jump when Up Arrow, W, or Space is pressed and player is standing on ground
    if ((Keyboard::isKeyPressed(Keyboard::Up) ||
         Keyboard::isKeyPressed(Keyboard::W) ||
         Keyboard::isKeyPressed(Keyboard::Space)) && onGround)
    {
        velocityY = -jump_strength; // Negative velocity moves upward in SFML
        onGround = false;
    }
}

#endif

