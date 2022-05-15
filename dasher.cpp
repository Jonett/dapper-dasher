#include "raylib.h"

struct animation_data
{
    Rectangle rec;
    Vector2 pos;
    int frame;
    float update_time;
    float running_time;
};

bool is_on_ground(animation_data data, int window_height)
{
    return data.pos.y >= window_height - data.rec.height;
}

animation_data update_animation_data(animation_data data, float delta_time, int max_frame)
{
    // update running time
    data.running_time += delta_time;
    if (data.running_time >= data.update_time)
    {
        data.running_time = 0.0;
        // update animation frame
        data.rec.x = data.frame * data.rec.width;
        data.frame++;
        if (data.frame > max_frame)
        {
            data.frame = 0;
        }
    }
    return data;
}

int main()
{
    // window dimensions
    int window_dimensions[2];
    window_dimensions[0] = 512; // width
    window_dimensions[1] = 380; // height

    // initialize the window
    InitWindow(window_dimensions[0], window_dimensions[1], "Joni's Dapper-Dasher!");
    // acceleration due to gravity (pixels/s) / s
    const int gravity{1000};
    // Nebula variables
    Texture2D nebula = LoadTexture("textures/12_nebula_spritesheet.png");
    const int size_of_nebulea{6};
    animation_data nebulea[size_of_nebulea]{};

    int nebula_starting_pos{900};
    for (int i = 0; i < size_of_nebulea; i++)
    {
        nebulea[i].rec.x = 0.0;
        nebulea[i].rec.y = 0.0;
        nebulea[i].rec.width = nebula.width / 8;
        nebulea[i].rec.height = nebula.height / 8;

        nebulea[i].pos.x = nebula_starting_pos;
        nebula_starting_pos += 300;
        nebulea[i].pos.y = window_dimensions[1] - nebula.height / 8;

        nebulea[i].frame = 0;
        nebulea[i].running_time = 0.0;
        nebulea[i].update_time = 0.0;
    }

    float finish_line{nebulea[size_of_nebulea - 1].pos.x};

    // nebula X velocity (pixels / second)
    int nebula_velocity{-200};

    // scarfy variables
    Texture2D scarfy = LoadTexture("textures/scarfy.png");
    animation_data scarfy_data;
    scarfy_data.rec.width = scarfy.width / 6;
    scarfy_data.rec.height = scarfy.height;
    scarfy_data.rec.x = 0;
    scarfy_data.rec.y = 0;
    scarfy_data.pos.x = window_dimensions[0] / 2 - scarfy_data.rec.width / 2;
    scarfy_data.pos.y = window_dimensions[1] - scarfy_data.rec.height;
    scarfy_data.frame = 0;
    scarfy_data.update_time = 1.0 / 12.0;
    scarfy_data.running_time = 0.0;

    // jump velocity (pixels / second)
    const int jump_velocity{-600};
    // is the rectangle in the air
    bool is_in_air{false};

    int velocity{0};

    Texture2D background = LoadTexture("textures/far-buildings.png");
    float background_pos_x{};

    Texture2D midground = LoadTexture("textures/back-buildings.png");
    float midground_pos_x{};

    Texture2D foreground = LoadTexture("textures/foreground.png");
    float foreground_pos_x{};

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        // delta time ( time since last frame )
        const float dT{GetFrameTime()};

        // start drawing
        BeginDrawing();
        ClearBackground(WHITE);

        // scroll background
        background_pos_x -= 20 * dT;
        if (background_pos_x <= -background.width * 2)
        {
            background_pos_x = 0.0;
        }

        // scroll midground
        midground_pos_x -= 40 * dT;
        if (midground_pos_x <= -midground.width * 2)
        {
            midground_pos_x = 0.0;
        }

        // scroll foreground
        foreground_pos_x -= 80 * dT;
        if (foreground_pos_x <= -foreground.width * 2)
        {
            foreground_pos_x = 0.0;
        }

        // draw the background
        Vector2 background_1_pos{background_pos_x, 0.0};
        DrawTextureEx(background, background_1_pos, 0.0, 2.0, WHITE);
        Vector2 background_2_pos{background_pos_x + background.width * 2.0, 0.0};
        DrawTextureEx(background, background_2_pos, 0.0, 2.0, WHITE);

        // draw the midground
        Vector2 midground_1_pos{midground_pos_x, 0.0};
        DrawTextureEx(midground, midground_1_pos, 0.0, 2.0, WHITE);
        Vector2 midground_2_pos{midground_pos_x + midground.width * 2.0, 0.0};
        DrawTextureEx(midground, midground_2_pos, 0.0, 2.0, WHITE);

        // draw the foreground
        Vector2 foreground_1_pos{foreground_pos_x, 0.0};
        DrawTextureEx(foreground, foreground_1_pos, 0.0, 2.0, WHITE);
        Vector2 foreground_2_pos{foreground_pos_x + foreground.width * 2.0, 0.0};
        DrawTextureEx(foreground, foreground_2_pos, 0.0, 2.0, WHITE);

        // ground check
        if (is_on_ground(scarfy_data, window_dimensions[1]))
        {
            // rectangle is on the ground
            velocity = 0;
            is_in_air = false;
        }
        else
        {
            // rectangle is on the air
            //  apply gravity
            velocity += gravity * dT;
            is_in_air = true;
        }

        // check for jumping
        if (IsKeyPressed(KEY_SPACE) && !is_in_air)
        {
            velocity += jump_velocity;
        }

        // update nebula position
        for (int i = 0; i < size_of_nebulea; i++)
        {
            nebulea[i].pos.x += nebula_velocity * dT;
        }

        // update finish line
        finish_line += nebula_velocity * dT;

        // update scarfy position
        scarfy_data.pos.y += velocity * dT;

        // if scarfy is jumping we update animation frame
        if (!is_in_air)
        {
            // update scarfys animation frame and running time
            scarfy_data = update_animation_data(scarfy_data, dT, 5);
        }

        for (int i = 0; i < size_of_nebulea; i++)
        {
            // update nebula animation frame and running time
            nebulea[i] = update_animation_data(nebulea[i], dT, 7);
        }
        bool collision{};
        for (animation_data nebula : nebulea)
        {
            float padding{50};
            Rectangle nebula_rectangle{
                nebula.pos.x + padding,
                nebula.pos.y + padding,
                nebula.rec.width - 2 * padding,
                nebula.rec.height - 2 * padding};
            Rectangle scarfy_rectangle{
                scarfy_data.pos.x,
                scarfy_data.pos.y,
                scarfy_data.rec.width,
                scarfy_data.rec.height};
            if (CheckCollisionRecs(nebula_rectangle, scarfy_rectangle))
            {
                collision = true;
            }
        }
        if (collision)
        {
            // lose the game
            DrawText("Game Over!", window_dimensions[0] / 4, window_dimensions[1] / 2, 60, RED);
        }
        else if (scarfy_data.pos.x >= finish_line)
        {
            DrawText("You Win!", window_dimensions[0] / 4, window_dimensions[1] / 2, 60, BLUE);
        }
        else
        {
            for (int i = 0; i < size_of_nebulea; i++)
            {
                // draw nebula
                DrawTextureRec(nebula, nebulea[i].rec, nebulea[i].pos, WHITE);
            }

            // draw scarfy
            DrawTextureRec(scarfy, scarfy_data.rec, scarfy_data.pos, WHITE);
        }

        // stop drawing
        EndDrawing();
    }

    UnloadTexture(scarfy);
    UnloadTexture(nebula);
    UnloadTexture(background);
    UnloadTexture(midground);
    UnloadTexture(foreground);
    CloseWindow();
}