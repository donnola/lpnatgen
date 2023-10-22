#pragma once

namespace lpng
{
#define CMP(x, y)  (fabs(x - y) <= FLT_EPSILON * fmax(1.0, fmax(fabs(x), fabs(y))))
#define ABSOLUTE(x, y) (fabs(x - y) <= FLT_EPSILON)
#define RELATIVE(x, y) (fabs(x - y) <= FLT_EPSILON * Max(fabs(x), fabs(y)))
#define RAD2DEG(x) ((x) * 57.295754f)
#define DEG2RAD(x) ((x) * 0.0174533f)

  struct float2
  {
    float x;
    float y;

    float2(float a, float b) : x(a), y(b) {}
  };

  struct float3
  {
    float x;
    float y;
    float z;

    float3() : x(0), y(0), z(0) {}
    float3(float a, float b, float c) : x(a), y(b), z(c) {}

    float3& operator+=(const float3& r);
    float3& operator-=(const float3& r);
    float3& operator*=(const float3& r);
    float3& operator*=(double r);
    float3& operator/=(double r);

    double Magnitude() const;
    double MagnitudeSq() const;
    void Normalize();
    float3 Normalized() const;
  };

  float3 operator+(const float3& l, const float3& r);

  float3 operator-(const float3& l, const float3& r);

  float3 operator*(const float3& l, const float3& r);

  float3 operator*(const float3& l, double r);

  float3 operator*(double l, const float3& r);

  float3 operator/(const float3& l, double r);

  bool operator==(const float3& l, const float3& r);

  bool operator!=(const float3& l, const float3& r);

  inline double Dot(const float3& l, const float3& r);

  float3 Cross(const float3& l, const float3& r);

  double Angle(const float3& l, const float3& r);

  double Distance(const float3& l, const float3& r);

  float3 Project(const float3& length, const float3& direction);

  float3 Perpendicular(const float3& len, const float3& dir);

  float3 Reflection(const float3& vec, const float3& normal);
}
