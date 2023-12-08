#pragma once
#include <cmath>
#include "desc.h"

namespace lpng
{
#define CMP(x, y)  (fabs(x - y) <= FLT_EPSILON * fmax(1.0, fmax(fabs(x), fabs(y))))
#define ABSOLUTE_CMP(x, y) (fabs(x - y) <= FLT_EPSILON)
#define RELATIVE_CMP(x, y) (fabs(x - y) <= FLT_EPSILON * Max(fabs(x), fabs(y)))
#define RAD2DEG(x) ((x) * 57.295754f)
#define DEG2RAD(x) ((x) * 0.0174533f)

  struct Quat
  {

  };

  inline double Dot(const float3& l, const float3& r);

  float3 Cross(const float3& l, const float3& r);

  double Angle(const float3& l, const float3& r);

  double Distance(const float3& l, const float3& r);

  float3 Project(const float3& length, const float3& direction);

  float3 Perpendicular(const float3& len, const float3& dir);

  float3 Reflection(const float3& vec, const float3& normal);

  double DistFromPointToFace(const Object& obj, const Face& face, const float3& point);
  bool IsPointInTriangle(const float3& point, const float3& a, const float3& b, const float3& c);

  void SplitFaceMithPoint(Object& obj, const int faceId, const int pointId);

  void SetPeripheryFacesIds(
    const Object& obj, const std::vector<int>& facesIds, 
    std::vector<int>& peripheryFacesIds, std::vector<std::pair<int, int>>& peripheryEdges
  );

  void Scale(Object& obj, const float3& vec);

  void ExtrudeWithCap(Object& obj, const std::vector<int>& facesIds, const float3& vec);
  void ExtrudeWithCap(Object& obj, const std::vector<int>& facesIds, float dist);
  void Extrude(Object& obj, const std::vector<int>& facesIds, const float3& vec);
  void Extrude(Object& obj, const std::vector<int>& facesIds, float dist);

  void MoveFaces(Object& obj, const std::vector<int>& facesIds, const float3& vector);
  void MovePivot(Object& obj, const float3& vec);
  void RotateFaces(Object& obj, const std::vector<int>& facesIds, const Quat& quat, const float3& point);
}
