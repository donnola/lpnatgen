#include "lpmath.h"

#include <cmath>
#include <cfloat>

lpng::float3& lpng::float3::operator+=(const lpng::float3& r)
{
  x += r.x;
  y += r.y;
  z += r.z;
  return *this;
}
lpng::float3& lpng::float3::operator-=(const lpng::float3& r)
{
  x -= r.x;
  y -= r.y;
  z -= r.z;
  return *this;
}
lpng::float3& lpng::float3::operator*=(const lpng::float3& r)
{
  x *= r.x;
  y *= r.y;
  z *= r.z;
  return *this;
}
lpng::float3& lpng::float3::operator*=(double r)
{
  x *= r;
  y *= r;
  z *= r;
  return *this;
}
lpng::float3& lpng::float3::operator/=(double r)
{
  x /= r;
  y /= r;
  z /= r;
  return *this;
}

double lpng::float3::Magnitude() const
{
  return sqrt(Dot(*this, *this));
}

double lpng::float3::MagnitudeSq() const
{
  return Dot(*this, *this);
}

void lpng::float3::Normalize()
{
  double c = 1.0f / Magnitude();
  x *= c;
  y *= c;
  z *= c;
}

lpng::float3 lpng::float3::Normalized() const
{
  double c = Magnitude();
  return lpng::float3(x / c, y / c, z / c);
}

lpng::float3 lpng::operator+(const lpng::float3& l, const lpng::float3& r)
{
  return lpng::float3(l.x + r.x, l.y + r.y, l.z + r.z);
}

lpng::float3 lpng::operator-(const lpng::float3& l, const lpng::float3& r)
{
  return lpng::float3(l.x - r.x, l.y - r.y, l.z - r.z);
}

lpng::float3 lpng::operator*(const lpng::float3& l, const lpng::float3& r)
{
  return lpng::float3(l.x * r.x, l.y * r.y, l.z * r.z);
}

lpng::float3 lpng::operator*(const lpng::float3& l, double r)
{
  return lpng::float3(l.x * r, l.y * r, l.z * r);
}

lpng::float3 lpng::operator*(double l, const lpng::float3& r)
{
  return r * l;
}

lpng::float3 lpng::operator/(const lpng::float3& l, double r)
{
  return lpng::float3(l.x / r, l.y / r, l.z / r);
}

bool lpng::operator==(const lpng::float3& l, const lpng::float3& r)
{
  return CMP(l.x, r.x) && CMP(l.y, r.y) && CMP(l.z, r.z);
}

bool lpng::operator!=(const lpng::float3& l, const lpng::float3& r)
{
  return !(l == r);
}

inline double lpng::Dot(const lpng::float3& l, const lpng::float3& r)
{
  return l.x * r.x + l.y * r.y + l.z * r.z;
}

lpng::float3 lpng::Cross(const lpng::float3& l, const lpng::float3& r)
{
  lpng::float3 result;
  result.x = l.y * r.z - l.z * r.y;
  result.y = l.z * r.x - l.x * r.z;
  result.z = l.x * r.y - l.y * r.x;
  return result;
}

double lpng::Angle(const lpng::float3& l, const lpng::float3& r)
{
  double m = sqrtf(l.MagnitudeSq() * r.MagnitudeSq());
  return acos(Dot(l, r) / m);
}

double lpng::Distance(const lpng::float3& l, const lpng::float3& r)
{
  lpng::float3 t = l - r;
  return t.Magnitude();
}

lpng::float3 lpng::Project(const lpng::float3& dirA, const lpng::float3& dirB)
{
  double dot = Dot(dirA, dirB);
  double magSq = dirB.MagnitudeSq();
  return dirB * (dot / magSq);
}

inline lpng::float3 lpng::Perpendicular(const lpng::float3& dirA, const lpng::float3& dirB)
{
  return dirA - Project(dirA, dirB);
}

lpng::float3 lpng::Reflection(const lpng::float3& vec, const lpng::float3& normal)
{
  double d = Dot(vec, normal);
  return vec - normal * (d * 2.0f);
}
