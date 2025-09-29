rocket Player;
game_memory GameMemory;

internal void GameDrawPixel(game_struct* Game, v2 Position, u32 Color)
{
      if (Position.X <= 0) return;
      if (Position.X >= (i32)Game->DisplayBuffer.Width) return;
      if (Position.Y <= 0) return;
      if (Position.Y >= (i32)Game->DisplayBuffer.Height) return;
      
      *((u32 *) ((u8 *)Game->DisplayBuffer.Memory + 
                      (Position.Y * Game->DisplayBuffer.Pitch) + 
                      (Position.X * Game->DisplayBuffer.BytesPerPixel))
        ) = (u32) Color;
}

internal void GameDrawRect(game_struct* Game, u32 MinX, u32 MinY, u32 MaxX, u32 MaxY, u32 Color)
{
    if (MinX < 0) MinX = 0;
    if (MaxX > Game->DisplayBuffer.Width) MaxX = Game->DisplayBuffer.Width;
    if (MinY < 0) MinY = 0;
    if (MaxY > Game->DisplayBuffer.Height) MaxY = Game->DisplayBuffer.Height;

    for (u32 Y = MinY; Y < MaxY; Y++)
    {
        for (u32 X = MinX; X < MaxX; X++)
        {
            GameDrawPixel(Game, {(i32)X, (i32)Y}, Color);
        }
    }
}

internal void GameDrawLine(game_struct* Game, v2 FromPoint, v2 ToPoint, u32 Color)
{

    i32 DeltaX = abs((i32)(ToPoint.X - FromPoint.X));
    i32 SX = FromPoint.X < ToPoint.X ? 1 : -1;
    i32 DeltaY = -1*abs((i32)(ToPoint.Y - FromPoint.Y));
    i32 SY = FromPoint.Y < ToPoint.Y ? 1 : -1;

    i32 Error =  DeltaX + DeltaY;

    while(1)
    {
      GameDrawPixel(Game, FromPoint, Color);

      i32 E2 = 2 * Error;
      
      if (E2 >= DeltaY)
      {
        if (FromPoint.X == ToPoint.X) break;
        Error += DeltaY;
        FromPoint.X = (u32)(FromPoint.X + SX);
      }
      if (E2 <= DeltaX)
      {
        if (FromPoint.Y == ToPoint.Y) break;
        Error += DeltaX;
        FromPoint.Y = (u32)(FromPoint.Y + SY);
      }
    }
}

internal void CleanScreen(game_struct* Game)
{
    GameDrawRect(Game, 0, 0, Game->DisplayBuffer.Width, Game->DisplayBuffer.Height, 0X000000); // Cleaning screen
}

v2 ToV2(f_v2 FVec)
{
  v2 Result = {};
  Result.X = (i32) round(FVec.X);
  Result.Y = (i32) round(FVec.Y);
  return Result;
}

v2 V2Add(v2 A, v2 B)
{
  v2 Result = {};
  Result.X = A.X + B.X;
  Result.Y = A.Y + B.Y;
  return Result;
}

f_v2 FV2Add(f_v2 A, f_v2 B)
{
  f_v2 Result = {};
  Result.X = A.X + B.X;
  Result.Y = A.Y + B.Y;
  return Result;
}

v2 V2Diff(v2 A, v2 B)
{
  v2 Result = {};
  Result.X = A.X - B.X;
  Result.Y = A.Y - B.Y;
  return Result;
}

f64 V2Mag(v2 Vec)
{
  f64 Result = {};
  Result = sqrt(Vec.X * Vec.X + Vec.Y * Vec.Y);
  return Result;
}

f_v2 FV2ScalarMult(f_v2 Vec, f32 Scale)
{
  f_v2 Result = {};
  Result.X = (f32) round(Vec.X * Scale); 
  Result.Y = (f32) round(Vec.Y *Scale);
  return Result;
}

v2 V2Rotate(v2 Ref, double angle_degrees)
{
  f64 radian = angle_degrees * (M_PI / 180);
  v2 Result = {};

  f64 c = (f64) cos(radian);
  f64 s = (f64) sin(radian);
  /*
    ROTATION
      [x']   =  [cos(θ)  -sin(θ)]  *  [x]
      [y']      [sin(θ)   cos(θ)]     [y]
  */
  Result.X = (i32) round((Ref.X * c) + (Ref.Y * -s));
  Result.Y = (i32) round((Ref.X * s) + (Ref.Y * c));

  return Result;
}

f_v2 FV2Normalize(v2 Vec)
{
  f32 Magnitude = (f32) V2Mag(Vec);
  Assert(Magnitude > 0);

  f_v2 Result = {};
  Result.X = (Vec.X / Magnitude);
  Result.Y = (Vec.Y / Magnitude);
  return Result;
}

void SetRocketVelocity(rocket *Player, v2 Point)
{
  v2 DiffenceCenterPoint = V2Diff(Point, Player->OriginalPosition.Center);
  
  f_v2 DifferenceMagnitude = FV2Normalize(DiffenceCenterPoint);

  f_v2 VelocityUnity = FV2ScalarMult(DifferenceMagnitude, Player->Speed);

  Player->Velocity = FV2Add(Player->Velocity, VelocityUnity);
}

void UpdateAndRender(game_struct *Game, rocket *Player, key_board_input *KeyboardInput)
{
  // Angle control
  if (KeyboardInput->KeyLeft.IsDown) Player->OriginalPosition.angle_degrees += Player->RotateSpeed;
  if (KeyboardInput->KeyRight.IsDown) Player->OriginalPosition.angle_degrees += -Player->RotateSpeed;
  if (KeyboardInput->KeyA.IsDown) Player->OriginalPosition.angle_degrees += Player->RotateSpeed;
  if (KeyboardInput->KeyD.IsDown) Player->OriginalPosition.angle_degrees += -Player->RotateSpeed;

  Player->OriginalPosition.Center.X = (Player->OriginalPosition.ToPoint0.X + Player->OriginalPosition.ToPoint1.X + Player->OriginalPosition.ToPoint2.X) / 3;
  Player->OriginalPosition.Center.Y = (Player->OriginalPosition.ToPoint0.Y + Player->OriginalPosition.ToPoint1.Y + Player->OriginalPosition.ToPoint2.Y) / 3;

  v2 Pivot = Player->OriginalPosition.Center;

  v2 TempToPoint0 = V2Diff(Player->OriginalPosition.ToPoint0, Pivot);
  TempToPoint0 = V2Rotate(TempToPoint0, Player->OriginalPosition.angle_degrees);
  TempToPoint0 = V2Add(TempToPoint0, Pivot);

  v2 TempToPoint1 = V2Diff(Player->OriginalPosition.ToPoint1, Pivot);
  TempToPoint1 = V2Rotate(TempToPoint1, Player->OriginalPosition.angle_degrees);
  TempToPoint1 = V2Add(TempToPoint1, Pivot);

  v2 TempToPoint2 = V2Diff(Player->OriginalPosition.ToPoint2, Pivot);
  TempToPoint2 = V2Rotate(TempToPoint2, Player->OriginalPosition.angle_degrees);
  TempToPoint2 = V2Add(TempToPoint2, Pivot);

  if (KeyboardInput->KeyUP.IsDown) SetRocketVelocity(Player, TempToPoint1);
  if (KeyboardInput->KeyDown.IsDown) SetRocketVelocity(Player, TempToPoint1);
  if (KeyboardInput->KeyW.IsDown) SetRocketVelocity(Player, TempToPoint1);
  if (KeyboardInput->KeyS.IsDown) SetRocketVelocity(Player, TempToPoint1);

  Player->OriginalPosition.ToPoint0 = V2Add(Player->OriginalPosition.ToPoint0, ToV2(Player->Velocity));
  Player->OriginalPosition.ToPoint1 = V2Add(Player->OriginalPosition.ToPoint1, ToV2(Player->Velocity));
  Player->OriginalPosition.ToPoint2 = V2Add(Player->OriginalPosition.ToPoint2, ToV2(Player->Velocity));

  GameDrawLine(Game, TempToPoint0, TempToPoint1, 0X00FF0000);
  GameDrawLine(Game, TempToPoint1, TempToPoint2, 0X00FF0000);
  GameDrawLine(Game, TempToPoint2, TempToPoint0, 0X000000FF);
}

void MainGame(game_struct* Game, key_board_input *KeyboardInput)
{
  CleanScreen(Game);

  if (!GameMemory.IsInitialized)
  {
    Game->DisplayWidth = 960;
    Game->DisplayHeight = 540;

    Player.Width = 20;
    Player.Height = 20;

    Player.OriginalPosition.ToPoint0 = {100, 100};
    Player.OriginalPosition.ToPoint1 = V2Add(Player.OriginalPosition.ToPoint0, {50, 100});
    Player.OriginalPosition.ToPoint2 = V2Add(Player.OriginalPosition.ToPoint1, {50, -100});

    GameMemory.IsInitialized = true;
  }

  UpdateAndRender(Game, &Player, KeyboardInput);
}