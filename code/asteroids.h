
#if !define(SPACE_PROGRAM_H)
#include "vector2d.h"

i32 COLOR_RED = 0X00FF0000;
i32 COLOR_GREEN = 0X00FF0000;
i32 COLOR_BLACK = 0X000000;
i32 COLOR_WHITE = 0X0FFFFFF;

struct rocket
{
  f32 Speed = 1.1f;
  f_v2 Velocity;
  f32 RotateSpeed = 3.0f;

  i32 Width;
  i32 Height;

  struct {
    triangle TriangleScreenBasePoints;
    triangle TriangleScreenRelativePoints;
    v2 TriagleCenter;
    v2 TriagleInitialCenter;
    f32 TriangleAngle;
  } Position;
};

struct key_state
{
  bool32 IsDown;
};

struct key_board_input
{
  key_state KeyW;
  key_state KeyS;
  key_state KeyD;
  key_state KeyA;
  key_state KeyUP;
  key_state KeyLeft;
  key_state KeyDown;
  key_state KeyRight;
};

struct game_memory
{
  bool32 IsInitialized = false;
};

struct screen_buffer
{
  u32 Width;
  u32 Height;
  u32 Pitch;
  u32 BytesPerPixel;

  void* Memory;
};

struct game_struct
{
  screen_buffer DisplayBuffer;
  u32 DisplayWidth;
  u32 DisplayHeight;
  v2 WorldWindowDimensions;
  v2 WorldWindowPosition;
};

#define SPACE_PROGRAM_H

#endif