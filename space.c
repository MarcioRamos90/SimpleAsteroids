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

v2 V2Rotate(v2 Ref, double TriangleAngle)
{
  f64 radian = TriangleAngle * (M_PI / 180);
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
  v2 DiffenceCenterPoint = V2Diff(Point, Player->Position.TriagleCenter);
  
  f_v2 DifferenceMagnitude = FV2Normalize(DiffenceCenterPoint);

  f_v2 VelocityUnity = FV2ScalarMult(DifferenceMagnitude, Player->Speed);

  Player->Velocity = FV2Add(Player->Velocity, VelocityUnity);
}

void UpdateAndRender(game_struct *Game, rocket *Player, key_board_input *KeyboardInput)
{
  // Angle control
  if (KeyboardInput->KeyLeft.IsDown) Player->Position.TriangleAngle += Player->RotateSpeed;
  if (KeyboardInput->KeyRight.IsDown) Player->Position.TriangleAngle += -Player->RotateSpeed;
  if (KeyboardInput->KeyA.IsDown) Player->Position.TriangleAngle += Player->RotateSpeed;
  if (KeyboardInput->KeyD.IsDown) Player->Position.TriangleAngle += -Player->RotateSpeed;

  Player->Position.TriagleCenter.X = (Player->Position.TriangleBasePoints.Point0.X + Player->Position.TriangleBasePoints.Point1.X + Player->Position.TriangleBasePoints.Point2.X) / 3;
  Player->Position.TriagleCenter.Y = (Player->Position.TriangleBasePoints.Point0.Y + Player->Position.TriangleBasePoints.Point1.Y + Player->Position.TriangleBasePoints.Point2.Y) / 3;

  v2 Pivot = Player->Position.TriagleCenter;

  Player->Position.TriangleRelativePoints.Point0 = V2Diff(Player->Position.TriangleBasePoints.Point0, Pivot);
  Player->Position.TriangleRelativePoints.Point0 = V2Rotate(Player->Position.TriangleRelativePoints.Point0, Player->Position.TriangleAngle);
  Player->Position.TriangleRelativePoints.Point0 = V2Add(Player->Position.TriangleRelativePoints.Point0, Pivot);

  Player->Position.TriangleRelativePoints.Point1 = V2Diff(Player->Position.TriangleBasePoints.Point1, Pivot);
  Player->Position.TriangleRelativePoints.Point1 = V2Rotate(Player->Position.TriangleRelativePoints.Point1, Player->Position.TriangleAngle);
  Player->Position.TriangleRelativePoints.Point1 = V2Add(Player->Position.TriangleRelativePoints.Point1, Pivot);

  Player->Position.TriangleRelativePoints.Point2 = V2Diff(Player->Position.TriangleBasePoints.Point2, Pivot);
  Player->Position.TriangleRelativePoints.Point2 = V2Rotate(Player->Position.TriangleRelativePoints.Point2, Player->Position.TriangleAngle);
  Player->Position.TriangleRelativePoints.Point2 = V2Add(Player->Position.TriangleRelativePoints.Point2, Pivot);

  if (KeyboardInput->KeyUP.IsDown) SetRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyDown.IsDown) SetRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyW.IsDown) SetRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyS.IsDown) SetRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);

  Player->Position.TriangleBasePoints.Point0 = V2Add(Player->Position.TriangleBasePoints.Point0, ToV2(Player->Velocity));
  Player->Position.TriangleBasePoints.Point1 = V2Add(Player->Position.TriangleBasePoints.Point1, ToV2(Player->Velocity));
  Player->Position.TriangleBasePoints.Point2 = V2Add(Player->Position.TriangleBasePoints.Point2, ToV2(Player->Velocity));

  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point0, Player->Position.TriangleRelativePoints.Point1, 0X00FF0000);
  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point1, Player->Position.TriangleRelativePoints.Point2, 0X00FF0000);
  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point2, Player->Position.TriangleRelativePoints.Point0, 0X000000FF);
}

internal void GameDrawStar(game_struct* Game, rocket *Player, v2 StarPoint)
{
  GameDrawPixel(Game, StarPoint, 0X00FF0000);
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

    Player.Position.TriangleBasePoints.Point0 = {100, 100};
    Player.Position.TriangleBasePoints.Point1 = V2Add(Player.Position.TriangleBasePoints.Point0, {50, 100});
    Player.Position.TriangleBasePoints.Point2 = V2Add(Player.Position.TriangleBasePoints.Point1, {50, -100});

    GameMemory.IsInitialized = true;
  }

  u32 TilesSpaceDimentions[1][2] = {
    {Game->DisplayWidth, Game->DisplayHeight}
  };

  v2 StarsInSpace[1][5] = {
    {{100, 100}, {150, 230}, {400, 10}, {300, 450}, {250, 340}}
  };

  UpdateAndRender(Game, &Player, KeyboardInput);

  u32 TileY = 0;

  for (u32 TileX = 0; TileX < 5; ++TileX)
  {
    v2 StarPoint = StarsInSpace[TileY][TileX];
    GameDrawStar(Game, &Player, StarPoint);
  }
}