#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <SFML/Graphics.hpp>
using namespace sf;

void player_movement(char lvl[][18], float& player_x, float& player_y, float speed, const int cell_size, int Pheight, int Pwidth,const int width, const int height, Sprite& PlayerSprite, float scale,int duck_height)
{   float offset_x = player_x;
    // Move at half speed while crouching
    float current_speed = (Pheight == duck_height) ? speed * 0.5f : speed;
    // Move Left (Left Arrow or A)
    if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
    {  offset_x -= current_speed;

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
        offset_x += current_speed;

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
// duck
void player_duck(char lvl[][18], float& player_y, int& Pheight, int normal_height, int duck_height, int Pwidth, const int cell_size, bool onGround, Sprite& PlayerSprite, Texture& normalTex, Texture& duckTex)
{  bool wantsToDuck = (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S)) && onGround;

    if (wantsToDuck)
    {
        if (Pheight != duck_height)
        {
            // Move player_y down so feet stay on the ground when height shrinks
            player_y += (normal_height - duck_height);
            Pheight = duck_height;
            PlayerSprite.setTexture(duckTex, true);
            if (PlayerSprite.getScale().x < 0)
                PlayerSprite.setOrigin(PlayerSprite.getLocalBounds().width, 0);
        }
    }
    else if (Pheight == duck_height)
    {
        // Check if there is a ceiling block above before standing back up
        float stand_y = player_y - (normal_height - duck_height);
        int top_row = (int)stand_y / cell_size;
        int left_col = (int)(PlayerSprite.getPosition().x + 2) / cell_size;
        int right_col = (int)(PlayerSprite.getPosition().x + Pwidth - 2) / cell_size;

        if (top_row >= 0 && lvl[top_row][left_col] != '#' && lvl[top_row][right_col] != '#')
        {
            player_y = stand_y;
            Pheight = normal_height;
             PlayerSprite.setTexture(normalTex, true);
             if (PlayerSprite.getScale().x < 0)
                PlayerSprite.setOrigin(PlayerSprite.getLocalBounds().width, 0);
        }
    }
}

// climb
void climb_up(char lvl[][18], float player_x, float& player_y, float climb_speed, const int cell_size, int Pwidth)
{   int mid_col = (int)(player_x + Pwidth / 2) / cell_size;
    float next_y = player_y - climb_speed;
    int next_top_row = (int)next_y / cell_size;

    if (next_y >= 0 && lvl[next_top_row][mid_col] != '#')
    {
        player_y = next_y;
    }
}

void climb_down(char lvl[][18], float player_x, float& player_y, float climb_speed, const int cell_size, int Pheight, int Pwidth, const int height, bool& isLadder)
{
    int mid_col = (int)(player_x + Pwidth / 2) / cell_size;
    float next_y = player_y + climb_speed;
    int next_bottom_row = (int)(next_y + Pheight - 1) / cell_size;

    if (next_bottom_row < height && lvl[next_bottom_row][mid_col] != '#')
    {
        player_y = next_y;
    }
    else
    {
        isLadder = false; // Reached solid floor at bottom
    }
}

// UP ROUTER: If on ladder ; climb_up(), else ; player_jump()
void handle_up(char lvl[][18], float player_x, float& player_y, float& velocityY, bool& onGround, bool& isLadder, float jump_strength, float climb_speed, const int cell_size, int Pheight, int Pwidth)
{
    int mid_col = (int)(player_x + Pwidth / 2) / cell_size;
    int top_row = (int)player_y / cell_size;
    int bottom_row = (int)(player_y + Pheight - 1) / cell_size;
    bool touchingLadder = (lvl[top_row][mid_col] == 'H' || lvl[bottom_row][mid_col] == 'H');

    bool upPressed = Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W);

    if (upPressed && touchingLadder)
    {
        isLadder = true;
        onGround = false;
        velocityY = 0;
    }

    if (isLadder)
    {
        if (!touchingLadder) 
        { isLadder = false; return; }
        if (upPressed)
        climb_up(lvl, player_x, player_y, climb_speed, cell_size, Pwidth);
    }
    else
    {
        player_jump(velocityY, onGround, jump_strength);
    }
}

// DOWN ROUTER: If on ladder ; climb_down(), else ; duck()
void handle_down(char lvl[][18], float player_x, float& player_y, float& velocityY, int& Pheight, int normal_height, int duck_height, int Pwidth, const int cell_size, const int height, bool& onGround, bool& isLadder, float climb_speed, Sprite& PlayerSprite, Texture& normalTex, Texture& duckTex)
{
    int mid_col = (int)(player_x + Pwidth / 2) / cell_size;
    int top_row = (int)player_y / cell_size;
    int bottom_row = (int)(player_y + Pheight - 1) / cell_size;
    bool touchingLadder = (lvl[top_row][mid_col] == 'H' || lvl[bottom_row][mid_col] == 'H');

    bool downPressed = Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S);

    if (downPressed && touchingLadder)
    {
        isLadder = true;
        onGround = false;
        velocityY = 0;
    }

    if (isLadder)
    {
        if (!touchingLadder) { isLadder = false; return; }
        if (downPressed) climb_down(lvl, player_x, player_y, climb_speed, cell_size, Pheight, Pwidth, height, isLadder);
    }
    else
    {
        player_duck(lvl, player_y, Pheight, normal_height, duck_height, Pwidth, cell_size, onGround, PlayerSprite, normalTex, duckTex);
    }
}


#endif

