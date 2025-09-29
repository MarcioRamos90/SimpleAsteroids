
#if !define(SPACE_PROGRAM_H)

struct v2
{
  i32 X;
  i32 Y;
};

struct f_v2
{
  f32 X;
  f32 Y;
};

struct rocket
{
  f32 Speed = 1.1f;
  f_v2 Velocity;
  f32 RotateSpeed = 3.0f;

  i32 Width;
  i32 Height;

  struct {
    v2 ToPoint0;
    v2 ToPoint1;
    v2 ToPoint2;
    v2 Center;
    double angle_degrees;
  } OriginalPosition;
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
  bool32 IsInitialized;
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

};

#define SPACE_PROGRAM_H

#endif