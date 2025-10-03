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

struct triangle
{
  v2 Point0;
  v2 Point1;
  v2 Point2;
};


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

v2 V2ScalarMult(v2 A, v2 B)
{
  v2 Result = {};
  Result.X = A.X * B.X;
  Result.Y = A.Y * B.Y;
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

f_v2 FV2Normalize(v2 Vec)
{
  f32 Magnitude = (f32) V2Mag(Vec);
  Assert(Magnitude > 0);

  f_v2 Result = {};
  Result.X = (Vec.X / Magnitude);
  Result.Y = (Vec.Y / Magnitude);
  return Result;
}

v2 V2Rotate(v2 Ref, double TriangleAngle)
{
  f64 radian = TriangleAngle * (M_PI / 180);
  v2 Result = {};

  f64 c = (f64) cos(radian);
  f64 s = (f64) sin(radian);

  Result.X = (i32) round((Ref.X * c) + (Ref.Y * -s));
  Result.Y = (i32) round((Ref.X * s) + (Ref.Y * c));

  return Result;
}

v2 RotateByOrigin(v2 Vec, v2 origion, f32 Angle)
{
  v2 Result = {};

  Result = V2Diff(Vec, origion);
  Result = V2Rotate(Result, Angle);
  Result = V2Add(Result, origion);

  return Result;
}

v2 V2TriangleCenter(v2 P1, v2 P2, v2 P3)
{
  v2 Result = {};
  Result.X = (P1.X + P2.X + P3.X) / 3;
  Result.Y = (P1.Y + P2.Y + P3.Y) / 3;
  return Result;
}