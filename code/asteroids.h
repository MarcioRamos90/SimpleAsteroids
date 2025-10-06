
#if !define(SPACE_PROGRAM_H)
#include "vector2d.h"

i32 COLOR_RED = 0X00FF0000;
i32 COLOR_GREEN = 0X00FF0000;
i32 COLOR_BLACK = 0X000000;
i32 COLOR_WHITE = 0X0FFFFFF;

struct world_struct
{
  i32 Width;
  i32 Height;
  v2 StarsPositions[100];
};

struct projectile
{
  f32 Speed = 30;
  i32 Width = 1;
  i32 Height = 10;
  bool32 IsActive = false;
  struct  
  {
    v2 Point0;
    v2 Point1;
  } Position;

  u32 Damage;
};

#define PROJECTILE_MAX_ARRAY 10
struct rocket
{
  f32 Speed = 1.1f;
  f_v2 Velocity;
  f32 RotateSpeed = 3.0f;
  bool32 ToFire = false;

  i32 Width;
  i32 Height;

  struct {
    triangle TriangleScreenBasePoints;
    triangle TriangleScreenRelativePoints;
    v2 TriagleCenter;
    v2 TriagleInitialCenter;
    f32 TriangleAngle;
  } Position;

  projectile Projectiles[PROJECTILE_MAX_ARRAY];
  u32 MaxProjectiles = PROJECTILE_MAX_ARRAY;
  u32 ProjectilesHead = 0;
};

struct key_state
{
  bool32 IsDown;
  bool32 WasDown;
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
  key_state KeySpace;
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