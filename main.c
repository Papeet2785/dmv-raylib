#include <raylib.h>
#include <math.h>

#define BACKGROUND_COLOR (Color){186, 149, 127}
#define CAR_COLOR BLACK
#define CAMERA_FOLLOW_THRESH_X 400
#define CAMERA_FOLLOW_THRESH_Y 250
#define MAX_SKIDMARKS 500
#define SKIDMARK_TIME 3

typedef struct {
    float left_tire_x;
    float left_tire_y;
    float right_tire_x;
    float right_tire_y;
    double time;
} Skidmark;

int main() {
    const int WIDTH = 1024;
    const int HEIGHT = 768;
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_WINDOW_HIGHDPI);
    InitWindow(WIDTH, HEIGHT, "Racer");
    SetTargetFPS(60);
    const float WORLD_SIZE = 10000;
    Image city_image = LoadImage("assets/city.png");
    ImageRotateCW(&city_image);
    Texture2D city_texture = LoadTextureFromImage(city_image);
    const float CITY_SCALE = 2.0f;
    Image car_image = LoadImage("assets/car.png");
    Texture2D car_texture = LoadTextureFromImage(car_image);
    Rectangle car_texture_rec = {
        .x = 0,
        .y = 0,
        .width = car_texture.width,
        .height = car_texture.height,
    };
    const float CAR_WIDTH = 60;
    const float CAR_LENGTH = 120;
    float car_x = WORLD_SIZE / 2;
    float car_y = WORLD_SIZE / 2;
    float car_speed = 0;
    float car_max_speed = 14;
    float car_angle = 0;
    float car_speedup = 10;
    float car_slowdown = 0.97;
    float drift_angle = car_angle;
    float drift_bias = 15;
    float steering = 0;
    float steering_speed = 2;
    float max_steering = 4;
    float steer_back_speed = 0.04;
    Skidmark skidmarks[MAX_SKIDMARKS];
    int skidmark_count = 0;
    Camera2D camera = {
        .offset = (Vector2){0, 0},
        .target = (Vector2){0, 0},
        .rotation = 0,
        .zoom = 1.0,
    };
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (IsKeyDown(KEY_W)) {
            car_speed += car_speedup * dt;

            if (car_speed > car_max_speed) {
                car_speed = car_max_speed;
            }
        }
        else if (IsKeyDown(KEY_S)) {
            car_speed -= car_speedup * dt;

            if (car_speed < -car_max_speed) {
                car_speed = -car_max_speed;
            }
        }
        else {
            car_speed *= car_slowdown;
        }
        if (IsKeyDown(KEY_A)) {
            steering -= steering_speed * dt * fabsf(car_speed);

            if (steering < -max_steering) {
                steering = -max_steering;
            }
        }
        else if (IsKeyDown(KEY_D)) {
            steering += steering_speed * dt * fabsf(car_speed);

            if (steering > max_steering) {
                steering = max_steering;
            }
        }
        steering *= (1 - steer_back_speed);
        car_angle += steering;
        drift_angle =
            (car_angle + drift_angle * drift_bias)
            / (1 + drift_bias);
        float drift_diff = drift_angle - car_angle;
        bool drifting = fabsf(drift_diff) > 30;
        float radians = PI * (drift_angle - 90) / 180;
        car_x += car_speed * cosf(radians);
        car_y += car_speed * sinf(radians);
        if (car_x < CAR_WIDTH / 2) {
            car_x = CAR_WIDTH / 2;
        }
        if (car_x > WORLD_SIZE - CAR_WIDTH / 2) {
            car_x = WORLD_SIZE - CAR_WIDTH / 2;
        }
        if (car_y < CAR_LENGTH / 2) {
            car_y = CAR_LENGTH / 2;
        }
        if (car_y > WORLD_SIZE - CAR_LENGTH / 2) {
            car_y = WORLD_SIZE - CAR_LENGTH / 2;
        }
        float car_screen_x = car_x + camera.offset.x;
        float car_screen_y = car_y + camera.offset.y;
        if (car_screen_x < CAMERA_FOLLOW_THRESH_X) {
            camera.offset.x = -car_x + CAMERA_FOLLOW_THRESH_X;
        }
        if (car_screen_x > WIDTH - CAMERA_FOLLOW_THRESH_X) {
            camera.offset.x = -car_x + (WIDTH - CAMERA_FOLLOW_THRESH_X);
        }
        if (car_screen_y < CAMERA_FOLLOW_THRESH_Y) {
            camera.offset.y = -car_y + CAMERA_FOLLOW_THRESH_Y;
        }
        if (car_screen_y > HEIGHT - CAMERA_FOLLOW_THRESH_Y) {
            camera.offset.y = -car_y + (HEIGHT - CAMERA_FOLLOW_THRESH_Y);
        }
        if (camera.offset.x > 0) {
            camera.offset.x = 0;
        }
        if (camera.offset.x < WIDTH - WORLD_SIZE) {
            camera.offset.x = WIDTH - WORLD_SIZE;
        }
        if (camera.offset.y > 0) {
            camera.offset.y = 0;
        }
        if (camera.offset.y < HEIGHT - WORLD_SIZE) {
            camera.offset.y = HEIGHT - WORLD_SIZE;
        }
        if (drifting) {
            radians = PI * (car_angle - 240) / 180;
            float left_tire_x = car_x;
            left_tire_x += CAR_LENGTH / 2.6 * cosf(radians);
            float left_tire_y = car_y;
            left_tire_y += CAR_LENGTH / 2.6 * sinf(radians);
            radians = PI * (car_angle - 300) / 180;
            float right_tire_x = car_x;
            right_tire_x += CAR_LENGTH / 2.6 * cosf(radians);
            float right_tire_y = car_y;
            right_tire_y += CAR_LENGTH / 2.6 * sinf(radians);
            skidmarks[skidmark_count % MAX_SKIDMARKS] =
                (Skidmark){
                    left_tire_x,
                    left_tire_y,
                    right_tire_x,
                    right_tire_y,
                    GetTime(),
                };
            skidmark_count++;
        }
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        BeginMode2D(camera);
        int tile_count_col =
            ceil(WORLD_SIZE / (city_texture.width * CITY_SCALE));
        int tile_count_row =
            ceil(WORLD_SIZE / (city_texture.height * CITY_SCALE));
        for (int x = 0; x < tile_count_col; x++) {
            for (int y = 0; y < tile_count_row; y++) {
                DrawTextureEx(
                    city_texture,
                    (Vector2){
                        x * city_texture.width * CITY_SCALE,
                        y * city_texture.height * CITY_SCALE,
                    },
                    0.0f,
                    CITY_SCALE,
                    WHITE
                );
            }
        }
        double current_time = GetTime();
        for (int i = 0;
             i < skidmark_count && i < MAX_SKIDMARKS;
             i++) {
            Skidmark skidmark = skidmarks[i];
            if (current_time - skidmark.time > SKIDMARK_TIME) {
                continue;
            }
            DrawCircle(
                skidmark.left_tire_x,
                skidmark.left_tire_y,
                6,
                BLACK
            );
            DrawCircle(
                skidmark.right_tire_x,
                skidmark.right_tire_y,
                6,
                BLACK
            );
        }
        Rectangle car_rec = {
            .x = car_x,
            .y = car_y,
            .width = CAR_WIDTH,
            .height = CAR_LENGTH,
        };
        Vector2 car_origin = {
            .x = CAR_WIDTH / 2,
            .y = CAR_LENGTH / 2,
        };
        DrawTexturePro(
            car_texture,
            car_texture_rec,
            car_rec,
            car_origin,
            car_angle,
            WHITE
        );
        EndMode2D();
        EndDrawing();
    }
    UnloadImage(car_image);
    UnloadTexture(car_texture);
    UnloadImage(city_image);
    UnloadTexture(city_texture);
    CloseWindow();
    return 0;
}
