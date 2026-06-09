#ifndef TEST_RAYLIB_H
#define TEST_RAYLIB_H

#include <stdbool.h>

#define DEG2RAD 0.017453292519943295f

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef struct Rectangle {
    float x;
    float y;
    float width;
    float height;
} Rectangle;

typedef struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} Color;

typedef struct Texture {
    unsigned int id;
    int width;
    int height;
    int mipmaps;
    int format;
} Texture;

typedef Texture Texture2D;

enum {
    KEY_W = 87,
    KEY_A = 65,
    KEY_S = 83,
    KEY_D = 68,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265
};

static const Color WHITE = {255, 255, 255, 255};

bool IsKeyDown(int key);
void DrawCircleV(Vector2 center, float radius, Color color);
void DrawCircleLines(int centerX, int centerY, float radius, Color color);
void DrawLineEx(Vector2 startPos, Vector2 endPos, float thick, Color color);
void DrawTextureEx(Texture2D texture, Vector2 position, float rotation, float scale, Color tint);

#endif
