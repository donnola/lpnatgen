#include "lpmath.h"

#include <cmath>
#include <cfloat>
#include <set>
#include <algorithm>

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

double lpng::Magnitude(const float3& vec)
{
  return sqrt(Dot(vec, vec));
}

double lpng::MagnitudeSq(const float3& vec)
{
  return Dot(vec, vec);
}

void lpng::Normalize(float3& vec)
{
  double c = 1.0f / Magnitude(vec);
  vec *= c;
}

lpng::float3 lpng::Normalized(const float3& vec)
{
  double c = Magnitude(vec);
  return vec / c;
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
  double m = sqrtf(MagnitudeSq(l) * MagnitudeSq(r));
  return acos(Dot(l, r) / m);
}

double lpng::Distance(const lpng::float3& l, const lpng::float3& r)
{
  lpng::float3 t = l - r;
  return Magnitude(t);
}

lpng::float3 lpng::Project(const lpng::float3& dirA, const lpng::float3& dirB)
{
  double dot = Dot(dirA, dirB);
  double magSq = MagnitudeSq(dirB);
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

bool lpng::operator==(const Face& l, const Face& r)
{
  std::vector<int> f_l;
  for (const Vertex& v : l)
  {
    f_l.push_back(v.vi);
  }
  std::vector<int> f_r;
  for (const Vertex& v : r)
  {
    f_r.push_back(v.vi);
  }
  if (f_l.size() == f_r.size())
  {
    std::sort(f_l.begin(), f_l.end());
    std::sort(f_r.begin(), f_r.end());
    for (int i = 0; i < f_l.size(); ++i)
    {
      if (f_l[i] != f_r[i])
        return false;
    }
    return true;
  }
  return false;
}

bool lpng::operator==(const Edge& l, const Edge& r)
{
  if (l.first == r.first && l.second == r.second)
    return true;
  if (l.first == r.second && l.second == r.first)
    return true;
  return false;
}

bool lpng::IsEdgeInFace(const Edge& edge, const Face& face)
{
  for (int i = 0; i < face.size(); ++i)
  {
    int e1 = face[i].vi;
    int e2 = face[(i + 1) % face.size()].vi;
    if (edge == Edge(e1, e2))
      return true;
  }
  return false;
}

void lpng::Scale(Object& obj, const float3& vec)
{
  for (float3& v : obj.vertexCoords)
  {
    v *= vec;
  }
}

void lpng::SetPeripheryFacesIds(
  const Object& obj, const std::vector<int>& facesIds,
  std::vector<int>& peripheryFacesIds, std::vector<std::pair<int, int>>& peripheryEdges
)
{
  for (int f1_id : facesIds)
  {
    const Face& f_i = obj.faces[f1_id];
    bool isFacePeriphery = false;
    for (int j = 0; j < f_i.size(); j++)
    {
      int e1 = f_i[j].vi;
      int e2 = f_i[(j + 1) % f_i.size()].vi;
      bool isEdgeHasNeighbor = false;
      for (int f2_id : facesIds)
      {
        if (isEdgeHasNeighbor)
          break;
        if (f2_id == f1_id)
          continue;
        isEdgeHasNeighbor = IsEdgeInFace({ e1, e2 }, obj.faces[f2_id]);
      }
      if (!isEdgeHasNeighbor)
      {
        isFacePeriphery = true;
        peripheryEdges.emplace_back(e1, e2);
      }
    }
    if (isFacePeriphery)
    {
      peripheryFacesIds.push_back(f1_id);
    }
  }
}

void lpng::ExtrudeWithCap(Object& obj, const std::vector<int>& facesIds, const float3& vec)
{
  std::vector<int> periphery_faces_ids;
  std::vector<std::pair<int, int>> periphery_edges;
  SetPeripheryFacesIds(obj, facesIds, periphery_faces_ids, periphery_edges);


}

void lpng::ExtrudeWithCap(Object& obj, const std::vector<int>& facesIds, float dist)
{
  std::vector<int> periphery_faces_ids;
  std::vector<std::pair<int, int>> periphery_edges;
  SetPeripheryFacesIds(obj, facesIds, periphery_faces_ids, periphery_edges);


}

void lpng::Extrude(Object& obj, const std::vector<int>& facesIds, const float3& vec)
{
  ExtrudeWithCap(obj, facesIds, vec);
  std::vector<Face> new_faces;
  for (int i = 0; i < obj.faces.size(); ++i)
  {
    if (std::find(facesIds.begin(), facesIds.end(), i) == facesIds.end())
      new_faces.push_back(std::move(obj.faces[i]));
  }
  obj.faces = std::move(new_faces);
}

void lpng::Extrude(Object& obj, const std::vector<int>& facesIds, float dist)
{
  ExtrudeWithCap(obj, facesIds, dist);
  std::vector<Face> new_faces;
  for (int i = 0; i < obj.faces.size(); ++i)
  {
    if (std::find(facesIds.begin(), facesIds.end(), i) == facesIds.end())
      new_faces.push_back(std::move(obj.faces[i]));
  }
  obj.faces = std::move(new_faces);
}

void lpng::MoveFaces(Object& obj, const std::vector<int>& facesIds, const float3& vector)
{
  std::set<int> vertexIds;
  for (int fi : facesIds)
  {
    const Face& f = obj.faces[fi];
    for (const Vertex& v : f)
    {
      vertexIds.insert(v.vi - 1);
    }
  }
  for (int vi : vertexIds)
  {
    obj.vertexCoords[vi] += vector;
  }
}

void lpng::MovePivot(Object& obj, const float3& vec)
{
  for (float3& v : obj.vertexCoords)
  {
    v -= vec;
  }
}

void lpng::SplitFaceMithPoint(Object& obj, const int faceId, const int pointId)
{
  const Face& f = obj.faces[faceId];
  for (int i = 0; i < f.size(); ++i)
  {
    obj.faces.push_back(Face({f[i].vi, f[(i + 1) % f.size()].vi, pointId}));
  }
  obj.faces.erase(obj.faces.begin() + faceId);
}

bool lpng::IsPointInTriangle(const float3& point, const float3& a, const float3& b, const float3& c)
{
  float3 ab = b - a;
  float3 ac = c - a;
  float3 ba = a - b;
  float3 cb = b - c;

  float3 ap = point - a;
  float3 bp = point - b;
  float3 cp = point - c;

  float3 normal = Cross(ab, ac);

  float dotProduct1 = Dot(normal, Cross(ap, ac));

  if (dotProduct1 < 0)
  {
    return false;
  }

  float dotProduct2 = Dot(normal, Cross(bp, ba));

  if (dotProduct2 < 0)
  {
    return false;
  }

  float dotProduct3 = Dot(normal, Cross(cp, cb));

  if (dotProduct3 < 0)
  {
    return false;
  }

  return true;
}

double lpng::DistFromPointToFace(const Object& obj, const Face& face, const float3& point)
{
  float3 a = obj.vertexCoords[face[0].vi - 1];
  float3 b = obj.vertexCoords[face[1].vi - 1];
  float3 c = obj.vertexCoords[face[2].vi - 1];
  float3 n = Cross(a-b, c-b);
  Normalize(n);
  float d = -Dot(n, a);
  double dist = abs(Dot(n, point) + d);
  float3 proj = point - dist * n;
  if (IsPointInTriangle(proj, a, b, c))
    return dist;
  return dist;
}
