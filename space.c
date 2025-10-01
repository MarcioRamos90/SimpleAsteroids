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
    GameDrawRect(Game, 0, 0, Game->DisplayBuffer.Width, Game->DisplayBuffer.Height, COLOR_BLACK); // Cleaning screen
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

f_v2 FV2Diff(f_v2 A, f_v2 B)
{
  f_v2 Result = {};
  Result.X = A.X - B.X;
  Result.Y = A.Y - B.Y;
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

void AddRocketVelocity(rocket *Player, v2 Point)
{
  v2 DiffenceCenterPoint = V2Diff(Point, Player->Position.TriagleCenter);
  
  f_v2 DifferenceMagnitude = FV2Normalize(DiffenceCenterPoint);

  f_v2 VelocityUnity = FV2ScalarMult(DifferenceMagnitude, Player->Speed);

  Player->Velocity = FV2Add(Player->Velocity, VelocityUnity);
}

void SubRocketVelocity(rocket *Player, v2 Point)
{
  v2 DiffenceCenterPoint = V2Diff(Point, Player->Position.TriagleCenter);
  
  f_v2 DifferenceMagnitude = FV2Normalize(DiffenceCenterPoint);

  f_v2 VelocityUnity = FV2ScalarMult(DifferenceMagnitude, Player->Speed);

  Player->Velocity = FV2Diff(Player->Velocity, VelocityUnity);
}

void GameDelimitingEdges(game_struct* Game, rocket* Player)
{
  v2 VectorEdges;
  bool32 TraspassedEdge = true;
  bool32 IsDiff = false;
  if (Player->Position.TriangleRelativePoints.Point1.X > (i32)Game->DisplayWidth) 
  {
    VectorEdges = {(i32)Game->DisplayWidth, 0};
    IsDiff = true;
  }
  else if (Player->Position.TriangleRelativePoints.Point1.X < 0)
  {
    VectorEdges = {(i32)Game->DisplayWidth, 0};
    IsDiff = false;
  }
  else if (Player->Position.TriangleRelativePoints.Point1.Y > (i32) Game->DisplayHeight)
  {
    VectorEdges = {0, (i32)Game->DisplayHeight};
    IsDiff = true;
  }
  else if (Player->Position.TriangleRelativePoints.Point1.Y < 0)
  {
    VectorEdges = {0, (i32)Game->DisplayHeight};
    IsDiff = false;
  }
  else
  {
    TraspassedEdge = false;
  }

  if (TraspassedEdge)
  {
    v2 CopyPoint0;
    v2 CopyPoint1;
    v2 CopyPoint2;

    if (IsDiff)
    {
      CopyPoint0 = V2Diff(Player->Position.TriangleRelativePoints.Point0, VectorEdges);
      CopyPoint1 = V2Diff(Player->Position.TriangleRelativePoints.Point1, VectorEdges);
      CopyPoint2 = V2Diff(Player->Position.TriangleRelativePoints.Point2, VectorEdges);
    }
    else
    {
      CopyPoint0 = V2Add(Player->Position.TriangleRelativePoints.Point0, VectorEdges);
      CopyPoint1 = V2Add(Player->Position.TriangleRelativePoints.Point1, VectorEdges);
      CopyPoint2 = V2Add(Player->Position.TriangleRelativePoints.Point2, VectorEdges);
    }
    if ((Player->Position.TriangleRelativePoints.Point0.X > (i32)Game->DisplayWidth && Player->Position.TriangleRelativePoints.Point2.X > (i32)Game->DisplayWidth) ||
        (Player->Position.TriangleRelativePoints.Point0.X < 0 && Player->Position.TriangleRelativePoints.Point2.X < 0) ||
        (Player->Position.TriangleRelativePoints.Point0.Y > (i32)Game->DisplayHeight && Player->Position.TriangleRelativePoints.Point2.Y > (i32)Game->DisplayHeight) ||
        (Player->Position.TriangleRelativePoints.Point0.Y < 0 && Player->Position.TriangleRelativePoints.Point2.Y < 0)
    )
    {
      Player->Position.TriangleRelativePoints.Point0 = CopyPoint0;
      Player->Position.TriangleRelativePoints.Point1 = CopyPoint1;
      Player->Position.TriangleRelativePoints.Point2 = CopyPoint2;

      if (IsDiff)
      {
        Player->Position.TriangleBasePoints.Point0 = V2Diff(Player->Position.TriangleBasePoints.Point0, VectorEdges);
        Player->Position.TriangleBasePoints.Point1 = V2Diff(Player->Position.TriangleBasePoints.Point1, VectorEdges);
        Player->Position.TriangleBasePoints.Point2 = V2Diff(Player->Position.TriangleBasePoints.Point2, VectorEdges);
      }
      else 
      {
        Player->Position.TriangleBasePoints.Point0 = V2Add(Player->Position.TriangleBasePoints.Point0, VectorEdges);
        Player->Position.TriangleBasePoints.Point1 = V2Add(Player->Position.TriangleBasePoints.Point1, VectorEdges);
        Player->Position.TriangleBasePoints.Point2 = V2Add(Player->Position.TriangleBasePoints.Point2, VectorEdges);
      }
    }

    GameDrawLine(Game,CopyPoint0,CopyPoint1, COLOR_RED);
    GameDrawLine(Game,CopyPoint1,CopyPoint2, COLOR_RED);
    GameDrawLine(Game,CopyPoint2,CopyPoint0, COLOR_GREEN);
  }
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

  if (KeyboardInput->KeyUP.IsDown) AddRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyDown.IsDown) SubRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyW.IsDown) AddRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);
  if (KeyboardInput->KeyS.IsDown) SubRocketVelocity(Player, Player->Position.TriangleRelativePoints.Point1);

  Player->Position.TriangleBasePoints.Point0 = V2Add(Player->Position.TriangleBasePoints.Point0, ToV2(Player->Velocity));
  Player->Position.TriangleBasePoints.Point1 = V2Add(Player->Position.TriangleBasePoints.Point1, ToV2(Player->Velocity));
  Player->Position.TriangleBasePoints.Point2 = V2Add(Player->Position.TriangleBasePoints.Point2, ToV2(Player->Velocity));

  GameDelimitingEdges(Game, Player);

  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point0, Player->Position.TriangleRelativePoints.Point1, COLOR_RED);
  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point1, Player->Position.TriangleRelativePoints.Point2, COLOR_RED);
  GameDrawLine(Game, Player->Position.TriangleRelativePoints.Point2, Player->Position.TriangleRelativePoints.Point0, COLOR_GREEN);

}

internal void GameDrawStar(game_struct* Game, rocket *Player, v2 StarPoint)
{
  GameDrawPixel(Game, StarPoint, COLOR_RED);
}

void MainGame(game_struct* Game, key_board_input *KeyboardInput)
{
  CleanScreen(Game);

  Game->DisplayWidth = 960;
  Game->DisplayHeight = 540;
  if (!GameMemory.IsInitialized)
  {

    Player.Width = 20;
    Player.Height = 20;

    Player.Position.TriangleBasePoints.Point0 = {100, 100};
    Player.Position.TriangleBasePoints.Point1 = V2Add(Player.Position.TriangleBasePoints.Point0, {50, 100});
    Player.Position.TriangleBasePoints.Point2 = V2Add(Player.Position.TriangleBasePoints.Point1, {50, -100});

    GameMemory.IsInitialized = true;
  }

  u32 TilesSpaceDimentions[1][2] = {
    {Game->DisplayWidth * 10, Game->DisplayHeight * 10}
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