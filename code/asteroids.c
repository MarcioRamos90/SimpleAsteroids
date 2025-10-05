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

f_v2 V2Move(v2 Point0, v2 Point1, f32 Speed)
{
  v2 DiffenceCenterPoint = V2Diff(Point1, Point0);
  
  f_v2 DifferenceMagnitude = FV2Normalize(DiffenceCenterPoint);

  f_v2 VelocityUnity = FV2ScalarMult(DifferenceMagnitude, Speed);

 return VelocityUnity;
}

void UpdateAndRender(game_struct *Game, rocket *Player, key_board_input *KeyboardInput)
{
  // Angle control
  if (KeyboardInput->KeyLeft.IsDown) Player->Position.TriangleAngle += Player->RotateSpeed;
  if (KeyboardInput->KeyRight.IsDown) Player->Position.TriangleAngle += -Player->RotateSpeed;
  if (KeyboardInput->KeyA.IsDown) Player->Position.TriangleAngle += Player->RotateSpeed;
  if (KeyboardInput->KeyD.IsDown) Player->Position.TriangleAngle += -Player->RotateSpeed;

  Player->Position.TriagleCenter = V2TriangleCenter(Player->Position.TriangleScreenBasePoints.Point0, Player->Position.TriangleScreenBasePoints.Point1, Player->Position.TriangleScreenBasePoints.Point2);

  Player->Position.TriangleScreenRelativePoints.Point0 = RotateByOrigin(Player->Position.TriangleScreenBasePoints.Point0, Player->Position.TriagleCenter, Player->Position.TriangleAngle);
  Player->Position.TriangleScreenRelativePoints.Point1 = RotateByOrigin(Player->Position.TriangleScreenBasePoints.Point1, Player->Position.TriagleCenter, Player->Position.TriangleAngle);
  Player->Position.TriangleScreenRelativePoints.Point2 = RotateByOrigin(Player->Position.TriangleScreenBasePoints.Point2, Player->Position.TriagleCenter, Player->Position.TriangleAngle);

  if (KeyboardInput->KeyUP.IsDown) AddRocketVelocity(Player, Player->Position.TriangleScreenRelativePoints.Point1);
  if (KeyboardInput->KeyDown.IsDown) SubRocketVelocity(Player, Player->Position.TriangleScreenRelativePoints.Point1);
  if (KeyboardInput->KeyW.IsDown) AddRocketVelocity(Player, Player->Position.TriangleScreenRelativePoints.Point1);
  if (KeyboardInput->KeyS.IsDown) SubRocketVelocity(Player, Player->Position.TriangleScreenRelativePoints.Point1);

  Game->WorldWindowPosition = V2Add(Game->WorldWindowPosition, ToV2(Player->Velocity));

  GameDrawLine(Game, Player->Position.TriangleScreenRelativePoints.Point0, Player->Position.TriangleScreenRelativePoints.Point1, COLOR_RED);
  GameDrawLine(Game, Player->Position.TriangleScreenRelativePoints.Point1, Player->Position.TriangleScreenRelativePoints.Point2, COLOR_RED);
  GameDrawLine(Game, Player->Position.TriangleScreenRelativePoints.Point2, Player->Position.TriangleScreenRelativePoints.Point0, COLOR_GREEN);

  if (KeyboardInput->KeySpace.IsDown && !Player->ToFire)
  {
    Player->ToFire = true;
    i32 ProjectileIndex = -1;
    
    for (i32 i = 0; i < (i32) Player->MaxProjectiles; ++i)
    {
      if (!Player->Projectiles[i].IsActive) ProjectileIndex = i;
    }
    if (ProjectileIndex < (i32) Player->MaxProjectiles)
    {
      Player->Projectiles[ProjectileIndex].Position.Point0 = Player->Position.TriagleCenter;
      Player->Projectiles[ProjectileIndex].Position.Point1 = Player->Position.TriangleScreenRelativePoints.Point1;
      Player->Projectiles[ProjectileIndex].IsActive = true;
    }
  }
  else if (KeyboardInput->KeySpace.WasDown && Player->ToFire)
  {
    Player->ToFire = false;
  } 

  for (u32 i = 0; i < Player->MaxProjectiles; ++i)
  { 
    if (Player->Projectiles[i].IsActive)
    {
      f_v2 VelUnity = V2Move(Player->Projectiles[i].Position.Point0, Player->Projectiles[i].Position.Point1, Player->Projectiles[i].Speed);
      Player->Projectiles[i].Position.Point0 = V2Add(Player->Projectiles[i].Position.Point0, ToV2(VelUnity));
      Player->Projectiles[i].Position.Point1 = V2Add(Player->Projectiles[i].Position.Point1, ToV2(VelUnity));
      
      f64 Distance = V2Mag(V2Diff(Player->Position.TriangleScreenRelativePoints.Point0, Player->Projectiles[i].Position.Point0));

      if (Distance >  600.0f)
      {
        Player->Projectiles[i].IsActive = false;
      }
      GameDrawLine(Game, Player->Projectiles[i].Position.Point0, Player->Projectiles[i].Position.Point1, COLOR_RED);
    }
  }
}

internal void GameDrawStar(game_struct* Game, rocket *Player, v2 StarPoint)
{
  GameDrawPixel(Game, StarPoint, COLOR_WHITE);
}

void MainGame(game_struct* Game, key_board_input *KeyboardInput)
{
  CleanScreen(Game);

  if (!GameMemory.IsInitialized)
  {
    Game->DisplayWidth = 960;
    Game->DisplayHeight = 540;

    Game->WorldWindowPosition = {(i32)Game->DisplayWidth, (i32)Game->DisplayHeight};

    Player.Width = 20;
    Player.Height = 20;

    Player.Position.TriangleScreenBasePoints.Point0 = {((i32)Game->DisplayWidth/2) - 50, ((i32)Game->DisplayHeight/2) - 50};
    Player.Position.TriangleScreenBasePoints.Point1 = V2Add(Player.Position.TriangleScreenBasePoints.Point0, {50, 100});
    Player.Position.TriangleScreenBasePoints.Point2 = V2Add(Player.Position.TriangleScreenBasePoints.Point1, {50, -100});

    GameMemory.IsInitialized = true;
  }

  v2 StarsInSpace[15] = {
    {100, 100}, {150, 230}, {400, 10}, {300, 450}, {250, 340},
    {1000, 100}, {1500, 230}, {1400, 10}, {1300, 450}, {1250, 340},
    {100, 600}, {150, 650}, {400, 910}, {300, 850}, {250, 740},
  };

  UpdateAndRender(Game, &Player, KeyboardInput);

  v2 PointDiffs = V2Diff(Game->WorldWindowPosition, {(i32)Game->DisplayWidth,(i32)Game->DisplayHeight});
  
  for (u32 TileX = 0; TileX < 15; ++TileX)
  {
  
    v2 StarPoint = StarsInSpace[TileX];

    GameDrawStar(Game, &Player, V2Diff(StarPoint, PointDiffs));
  }
}