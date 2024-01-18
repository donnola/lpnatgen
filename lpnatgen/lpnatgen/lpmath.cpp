#include "lpmath.h"

#include <cfloat>
#include <set>
#include <map>
#include <algorithm>


lpng::float3& lpng::float3::operator+=(const float3& r)
{
  x += r.x;
  y += r.y;
  z += r.z;
  return *this;
}


lpng::float3& lpng::float3::operator-=(const float3& r)
{
  x -= r.x;
  y -= r.y;
  z -= r.z;
  return *this;
}


lpng::float3& lpng::float3::operator*=(const float3& r)
{
  x *= r.x;
  y *= r.y;
  z *= r.z;
  return *this;
}


lpng::float3& lpng::float3::operator/=(const float3& r)
{
  x /= r.x;
  y /= r.y;
  z /= r.z;
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


lpng::float3 lpng::operator/(const float3& l, const float3& r)
{
  return lpng::float3(l.x / r.x, l.y / r.y, l.z / r.z);
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
  return double(l.x) * double(r.x) + double(l.y) * double(r.y) + double(l.z) * double(r.z);
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
      {
        return false;
      }
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


int lpng::TakeThirdPointFromTriangle(const Face& face, const Edge& edge)
{
  if (face.size() != 3)
    return -1;
  if (Edge(face[0].vi, face[1].vi) == edge)
    return face[2].vi;
  if (Edge(face[1].vi, face[2].vi) == edge)
    return face[0].vi;
  if (Edge(face[2].vi, face[0].vi) == edge)
    return face[1].vi;
  return -1;
}


void lpng::ScaleWorldCoord(Mesh& mesh, const float3& vec)
{
  for (float3& v : mesh.vertexCoords)
  {
    v *= vec;
  }
  mesh.pivot *= vec;
}


void lpng::ScaleLocalCoord(Mesh& mesh, const float3& vec)
{
  for (float3& v : mesh.vertexCoords)
  {
    v = (v - mesh.pivot) * vec + mesh.pivot;
  }
}


void lpng::SetPeripheryEdges(
  const Mesh& mesh, const std::vector<int>& facesIds,
  std::vector<Edge>& peripheryEdges
)
{
  peripheryEdges.clear();
  for (int f1_id : facesIds)
  {
    const Face& f_i = mesh.faces[f1_id];
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
        isEdgeHasNeighbor = IsEdgeInFace({ e1, e2 }, mesh.faces[f2_id]);
      }
      if (!isEdgeHasNeighbor)
      {
        peripheryEdges.emplace_back(e1, e2);
      }
    }
  }
}


std::vector<int> lpng::ExtrudeWithCap(Mesh& mesh, const std::vector<int>& facesIds, const float3& vec)
{
  std::vector<Edge> periphery_edges;
  SetPeripheryEdges(mesh, facesIds, periphery_edges);
  std::map<int, int> extruded_vertex_ids;
  std::vector<int> extruded_faces_ids;

  for (int fi : facesIds)
  {
    Face new_face = mesh.faces[fi];
    for (Vertex& v : new_face)
    {
      if (!extruded_vertex_ids.contains(v.vi))
      {
        mesh.vertexCoords.emplace_back(mesh.vertexCoords[v.vi-1] + vec);
        extruded_vertex_ids[v.vi] = mesh.vertexCoords.size();
      }
      v.vi = extruded_vertex_ids[v.vi];
    }
    std::reverse(mesh.faces[fi].begin(), mesh.faces[fi].end());
    mesh.faces.push_back(new_face);
    extruded_faces_ids.push_back(mesh.faces.size() - 1);
  }

  for (const Edge& e: periphery_edges)
  {
    mesh.faces.push_back(Face({ e.first, e.second, extruded_vertex_ids[e.second] }));
    mesh.faces.push_back(Face({ e.first, extruded_vertex_ids[e.second],  extruded_vertex_ids[e.first] }));
  }
  return extruded_faces_ids;
}


std::vector<int> lpng::Extrude(Mesh& mesh, const std::vector<int>& facesIds, const float3& vec)
{
  std::vector<int> extruded_faces_ids = ExtrudeWithCap(mesh, facesIds, vec);
  std::vector<Face> new_faces;
  for (int i = 0; i < mesh.faces.size(); ++i)
  {
    if (std::find(facesIds.begin(), facesIds.end(), i) == facesIds.end())
      new_faces.push_back(std::move(mesh.faces[i]));
  }
  mesh.faces = std::move(new_faces);
  for (int& f_id : extruded_faces_ids)
  {
    f_id -= facesIds.size();
  }
  return extruded_faces_ids;
}


void lpng::MoveFaces(Mesh& mesh, const std::vector<int>& facesIds, const float3& vector)
{
  std::set<int> vertexIds;
  for (int fi : facesIds)
  {
    const Face& f = mesh.faces[fi];
    for (const Vertex& v : f)
    {
      vertexIds.insert(v.vi - 1);
    }
  }
  for (int vi : vertexIds)
  {
    mesh.vertexCoords[vi] += vector;
  }
}


void lpng::MovePivot(Mesh& mesh, const float3& vec)
{
  mesh.pivot += vec;
}


void lpng::MoveObj(Mesh& mesh, const float3& vec)
{
  for (float3& v : mesh.vertexCoords)
  {
    v += vec;
  }
  mesh.pivot += vec;
}


void lpng::SplitFaceMithPoint(std::vector<Face>& faces, const int faceId, const int pointId)
{
  if (faceId == -1 || pointId == -1)
    return;
  const Face& f = faces[faceId];
  for (int i = 0; i < f.size(); ++i)
  {
    faces.push_back(Face({ f[i].vi, f[(i + 1) % f.size()].vi, pointId }));
  }
  faces.erase(faces.begin() + faceId);
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


double lpng::DistFromPointToFace(const float3& point, const float3& a, const float3& b, const float3& c)
{
  float3 n = Cross(a-b, c-b);
  Normalize(n);
  float d = -Dot(n, a);
  return abs(Dot(n, point) + d);
}


std::vector<lpng::float3> lpng::CalculateObjNormals(const Mesh& mesh)
{
  std::vector<float3> normals(mesh.vertexCoords.size());
  for (const Face& f : mesh.faces)
  {
    float3 a = mesh.vertexCoords[f[2].vi - 1] - mesh.vertexCoords[f[1].vi - 1];
    float3 b = mesh.vertexCoords[f[0].vi - 1] - mesh.vertexCoords[f[1].vi - 1];
    float3 normal = Cross(a, b);
    for (size_t i = 0; i < 3; ++i)
    {
      normals[f[i].vi - 1] += normal;
    }
  }
  for (float3& n : normals)
  {
    Normalize(n);
  }
  return normals;
}


void lpng::DecomposeObj(Mesh& mesh)
{
  std::vector<Face> new_faces;
  for (Face& f : mesh.faces)
  {
    if (f.size() < 3)
    {
      continue;
    }
    if (f.size() == 3)
    {
      new_faces.push_back(std::move(f));
    }
    else if (f.size() == 4)
    {
      new_faces.push_back(Face({ f[0], f[1], f[2] }));
      new_faces.push_back(Face({ f[2], f[3], f[0] }));
    }
    else
    {
      float3 mean_vertex_coord;
      for (const Vertex& v : f)
      {
        mean_vertex_coord += mesh.vertexCoords[v.vi - 1];
      }
      mesh.vertexCoords.push_back(mean_vertex_coord);
      size_t center_index = mesh.vertexCoords.size();
      Vertex mean_vertex(center_index);
      for (int i = 0; i < f.size(); ++i)
      {
        new_faces.push_back(Face({ f[i], f[(i + 1) % f.size()], mean_vertex }));
      }
    }
  }
  mesh.faces = std::move(new_faces);
}


void lpng::FilterNearesPoints(std::vector<float3>& points, float d)
{
  std::vector<float3> new_points;
  new_points.push_back(std::move(points[0]));
  for (int i = 1; i < points.size(); ++i)
  {
    bool is_add = true;
    for (const float3& p : new_points)
    {
      if (Distance(p, points[i]) <= d)
      {
        is_add = false;
        break;
      }
    }
    if (is_add)
    {
      new_points.push_back(std::move(points[i]));
    }
  }
  points = std::move(new_points);
}


std::vector<lpng::float3> lpng::GenerateEllipsoidUniformPoints(const float3& size, int pointsNum)
{
  std::vector<float3> points;
  float from_x = -size.x * 0.5;
  float to_x = size.x * 0.5;
  float from_y = -size.y * 0.5;
  float to_y = size.y * 0.5;
  float from_z = -size.z * 0.5;
  float to_z = size.z * 0.5;

  float3 rad(to_x, to_y, to_z);
  float3 rad_sq = rad * rad;

  while (points.size() < pointsNum)
  {
    float x = from_x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to_x - from_x)));
    float y = from_y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to_y - from_y)));
    float z = from_z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (to_z - from_z)));
    float3 p(x, y, z);
    float3 p_sq = p * p;
    float3 t = p_sq / rad_sq;
    if (t.x + t.y + t.z <= 1 && t.x + t.y + t.z > 0.5)
    {
      points.push_back(p);
    }
  }
  for (float3& p : points)
  {
    p /= rad;
    Normalize(p);
    p *= rad;
  }
  FilterNearesPoints(points);
  return points;
}

void lpng::FixMeshOrientation(Mesh& mesh)
{

}