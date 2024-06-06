#define _USE_MATH_DEFINES

#include "lpng_math.h"
#include "lpng_rand.h"

#include <cfloat>
#include <unordered_map>
#include <algorithm>


lpng::Quat::Quat(float a, float b, float c, float d)
{
  float3 vec(a, b, c);
  Normalize(vec);
  w = cos(d / 2);
  x = vec.x * sin(d / 2);
  y = vec.y * sin(d / 2);
  z = vec.z * sin(d / 2);
}


lpng::Quat::Quat(float3 vec, float d)
{
  Normalize(vec);
  w = cos(d / 2);
  x = vec.x * sin(d / 2);
  y = vec.y * sin(d / 2);
  z = vec.z * sin(d / 2);
}


void lpng::Invert(Quat& q)
{
  q.x = -q.x;
  q.y = -q.y;
  q.z = -q.z;
}


lpng::Quat lpng::Inverted(const Quat& q)
{
  Quat iq = q;
  iq.x = -iq.x;
  iq.y = -iq.y;
  iq.z = -iq.z;
  return iq;
}


lpng::Quat lpng::operator*(const Quat& l, const float3& r)
{
  Quat res;
  res.w = -l.x * r.x - l.y * r.y - l.z * r.z;
  res.x = l.w * r.x + l.y * r.z - l.z * r.y;
  res.y = l.w * r.y - l.x * r.z + l.z * r.x;
  res.z = l.w * r.z + l.x * r.y - l.y * r.x;
  return res;
}


lpng::Quat lpng::operator*(const Quat& l, const Quat& r)
{
  Quat res;
  res.w = l.w * r.w - l.x * r.x - l.y * r.y - l.z * r.z;
  res.x = l.w * r.x + l.x * r.w + l.y * r.z - l.z * r.y;
  res.y = l.w * r.y - l.x * r.z + l.y * r.w + l.z * r.x;
  res.z = l.w * r.z + l.x * r.y - l.y * r.x + l.z * r.w;
  return res;
}


lpng::float3 lpng::QuatTransformVec(const Quat& l, const float3& r)
{
  Quat res = l * r * Inverted(l);
  return float3(res.x, res.y, res.z);
}


lpng::float2& lpng::float2::operator+=(const float2& r)
{
  x += r.x;
  y += r.y;
  return *this;
}


lpng::float2& lpng::float2::operator-=(const float2& r)
{
  x -= r.x;
  y -= r.y;
  return *this;
}


lpng::float2& lpng::float2::operator*=(const float2& r)
{
  x *= r.x;
  y *= r.y;
  return *this;
}


lpng::float2& lpng::float2::operator/=(const float2& r)
{
  x /= r.x;
  y /= r.y;
  return *this;
}


lpng::float2& lpng::float2::operator*=(double r)
{
  x *= r;
  y *= r;
  return *this;
}


lpng::float2& lpng::float2::operator/=(double r)
{
  x /= r;
  y /= r;
  return *this;
}


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


double lpng::Magnitude(const float2& vec)
{
  return sqrt(Dot(vec, vec));
}


double lpng::MagnitudeSq(const float2& vec)
{
  return Dot(vec, vec);
}


double lpng::Magnitude(const float3& vec)
{
  return sqrt(Dot(vec, vec));
}


double lpng::MagnitudeSq(const float3& vec)
{
  return Dot(vec, vec);
}


void lpng::Normalize(float2& vec)
{
  float m = Magnitude(vec);
  if (m == 0)
    return;
  vec /= m;
}


lpng::float2 lpng::Normalized(const float2& vec)
{
  double m = Magnitude(vec);
  if (m == 0)
    return float2();
  return vec / m;
}


void lpng::Normalize(float3& vec)
{
  float m = Magnitude(vec);
  if (m == 0)
    return;
  vec /= m;
}


lpng::float3 lpng::Normalized(const float3& vec)
{
  double m = Magnitude(vec);
  if (m == 0)
    return float3();
  return vec / m;
}

lpng::float2 lpng::operator+(const lpng::float2& l, const lpng::float2& r)
{
  return lpng::float2(l.x + r.x, l.y + r.y);
}


lpng::float2 lpng::operator-(const lpng::float2& l, const lpng::float2& r)
{
  return lpng::float2(l.x - r.x, l.y - r.y);
}


lpng::float2 lpng::operator*(const lpng::float2& l, const lpng::float2& r)
{
  return lpng::float2(l.x * r.x, l.y * r.y);
}


lpng::float2 lpng::operator/(const float2& l, const float2& r)
{
  return lpng::float2(l.x / r.x, l.y / r.y);
}


lpng::float2 lpng::operator*(const lpng::float2& l, double r)
{
  return lpng::float2(l.x * r, l.y * r);
}


lpng::float2 lpng::operator*(double l, const lpng::float2& r)
{
  return r * l;
}


lpng::float2 lpng::operator/(const lpng::float2& l, double r)
{
  return lpng::float2(l.x / r, l.y / r);
}


bool lpng::operator==(const lpng::float2& l, const lpng::float2& r)
{
  return CMP(l.x, r.x) && CMP(l.y, r.y);
}


bool lpng::operator!=(const lpng::float2& l, const lpng::float2& r)
{
  return !(l == r);
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


inline double lpng::Dot(const lpng::float2& l, const lpng::float2& r)
{
  return double(l.x) * double(r.x) + double(l.y) * double(r.y);
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


double lpng::Angle(const lpng::float2& l, const lpng::float2& r)
{
  double m = sqrtf(MagnitudeSq(l) * MagnitudeSq(r));
  if (ABSOLUTE_CMP(m, 0))
    return -1;
  double a = std::clamp(Dot(l, r) / m, -1.0, 1.0);
  return acos(a);
}


double lpng::Angle(const lpng::float3& l, const lpng::float3& r)
{
  double m = sqrtf(MagnitudeSq(l) * MagnitudeSq(r));
  if (ABSOLUTE_CMP(m, 0))
    return -1;
  double a = std::clamp(Dot(l, r) / m, -1.0, 1.0);
  return acos(a);
}


double lpng::Distance(const lpng::float2& l, const lpng::float2& r)
{
  lpng::float2 t = l - r;
  return Magnitude(t);
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
  std::unordered_set<int> f_l;
  for (int v : l.vi)
    f_l.insert(v);
  std::unordered_set<int> f_r;
  for (int v : r.vi)
    f_r.insert(v);
  if (f_l.size() == f_r.size())
  {
    for (int v : f_l)
    {
      if (auto it = f_r.find(v); it == f_r.end())
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
  for (size_t i = 0; i < face.vi.size(); ++i)
  {
    int e1 = face.vi[i];
    int e2 = face.vi[(i + 1) % face.vi.size()];
    if (edge == Edge(e1, e2))
      return true;
  }
  return false;
}


int lpng::TakeThirdPointFromTriangle(const Face& face, const Edge& edge)
{
  if (face.vi.size() != 3)
    return -1;
  if (Edge(face.vi[0], face.vi[1]) == edge)
    return face.vi[2];
  if (Edge(face.vi[1], face.vi[2]) == edge)
    return face.vi[0];
  if (Edge(face.vi[2], face.vi[0]) == edge)
    return face.vi[1];
  return -1;
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
  if (dotProduct1 < 0) return false;


  float dotProduct2 = Dot(normal, Cross(bp, ba));
  if (dotProduct2 < 0) return false;

  float dotProduct3 = Dot(normal, Cross(cp, cb));
  if (dotProduct3 < 0) return false;

  return true;
}


int lpng::FindFaceWithEdge(const std::vector<Face>& faces, const Edge& e)
{
  for (size_t i = 0; i < faces.size(); ++i)
  {
    if (IsEdgeInFace(e, faces[i]))
      return i;
  }
  return -1;
}


std::vector<size_t> lpng::FindFacesInMesh(const Mesh& mesh, const size_t& v_id)
{
  std::vector<size_t> res;
  for (int i = 0; i < mesh.faces.size(); ++i)
  {
    if (std::find(mesh.faces[i].vi.begin(), mesh.faces[i].vi.end(), v_id) != mesh.faces[i].vi.end())
      res.push_back(i);
  }
  return res;
}


lpng::float3 lpng::FaceNormal(const Mesh& mesh, const Face& f)
{
  float3 a = mesh.vertexCoords[f.vi[2] - 1] - mesh.vertexCoords[f.vi[1] - 1];
  float3 b = mesh.vertexCoords[f.vi[0] - 1] - mesh.vertexCoords[f.vi[1] - 1];
  return Normalized(Cross(a, b));
}


std::vector<int> lpng::GetVerticesIds(const Mesh& mesh, const std::vector<int>& facesIds)
{
  std::unordered_set<int> vertices_ids;
  for (int id : facesIds)
  {
    for (int v : mesh.faces[id].vi)
      vertices_ids.insert(v - 1);
  }
  return { vertices_ids.begin(), vertices_ids.end() };
}


void lpng::ScaleObj(Mesh& mesh, const float3& vec)
{
  for (float3& v : mesh.vertexCoords)
    v *= vec;
}


void lpng::ScaleObj(Mesh& mesh, const float3& vec, const float3& O)
{
  for (float3& v : mesh.vertexCoords)
    v = (v - O) * vec + O;
}


void lpng::ScaleVertices(Mesh& mesh, const float3& vec, const std::vector<int>& verticesIds)
{
  float3 mean_point;
  for (int id : verticesIds)
    mean_point += mesh.vertexCoords[id];
  mean_point /= verticesIds.size();
  for (int id : verticesIds)
    mesh.vertexCoords[id] = (mesh.vertexCoords[id] - mean_point) * vec + mean_point;
}


void lpng::ScaleVertices(Mesh& mesh, const float3& vec, const std::vector<int>& verticesIds, const float3& O)
{
  for (int id : verticesIds)
    mesh.vertexCoords[id] = (mesh.vertexCoords[id] - O) * vec + O;
}


void lpng::ScaleFaces(Mesh& mesh, const float3& vec, const std::vector<int>& facesIds)
{
  ScaleVertices(mesh, vec, GetVerticesIds(mesh, facesIds));
}


void lpng::ScaleFaces(Mesh& mesh, const float3& vec, const std::vector<int>& facesIds, const float3& O)
{
  ScaleVertices(mesh, vec, GetVerticesIds(mesh, facesIds), O);
}


void lpng::RotateFaces(Mesh& mesh, const std::vector<int>& facesIds, const Quat& quat, const float3& O)
{
  RotateVertices(mesh, GetVerticesIds(mesh, facesIds), quat, O);
}


void lpng::RotateVertices(Mesh& mesh, const std::vector<int>& verticesIds, const Quat& quat, const float3& O)
{
  for (int pId : verticesIds)
  {
    float3& p = mesh.vertexCoords[pId];
    p -= O;
    p = QuatTransformVec(quat, p);
    p += O;
  }
}


void lpng::MoveFaces(Mesh& mesh, const std::vector<int>& facesIds, const float3& vector)
{
  for (int vi : GetVerticesIds(mesh, facesIds))
  {
    mesh.vertexCoords[vi] += vector;
  }
}


void lpng::MoveObj(Mesh& mesh, const float3& vec)
{
  for (float3& v : mesh.vertexCoords)
    v += vec;
}


void lpng::DecomposeMesh(Mesh& mesh)
{
  std::vector<Face> new_faces;
  for (Face& f : mesh.faces)
  {
    if (f.vi.size() < 3)
      continue;
    if (f.vi.size() == 3)
      new_faces.push_back(std::move(f));
    else if (f.vi.size() == 4)
    {
      new_faces.push_back(Face({ f.vi[0], f.vi[1], f.vi[2] }));
      new_faces.push_back(Face({ f.vi[2], f.vi[3], f.vi[0] }));
    }
    else
    {
      float3 mean_vertex_coord;
      for (size_t v : f.vi)
        mean_vertex_coord += mesh.vertexCoords[v - 1];
      mean_vertex_coord /= f.vi.size();
      mesh.vertexCoords.push_back(mean_vertex_coord);
      int center_index = mesh.vertexCoords.size();
      for (size_t i = 0; i < f.vi.size(); ++i)
        new_faces.push_back(Face({ f.vi[i], f.vi[(i + 1) % f.vi.size()], center_index }));
    }
  }
  mesh.faces = std::move(new_faces);
}


std::vector<lpng::float3> lpng::CalculateObjNormals(const Mesh& mesh)
{
  std::vector<float3> normals;
  for (const Face& f : mesh.faces)
  {
    float3 a = mesh.vertexCoords[f.vi[2] - 1] - mesh.vertexCoords[f.vi[1] - 1];
    float3 b = mesh.vertexCoords[f.vi[0] - 1] - mesh.vertexCoords[f.vi[1] - 1];
    normals.emplace_back(Normalized(Cross(a, b)));
  }
  return normals;
}


std::vector<lpng::float3> lpng::CalculateSmoothObjNormals(const Mesh& mesh)
{
  std::vector<float3> normals(mesh.vertexCoords.size());
  for (const Face& f : mesh.faces)
  {
    float3 a = mesh.vertexCoords[f.vi[2] - 1] - mesh.vertexCoords[f.vi[1] - 1];
    float3 b = mesh.vertexCoords[f.vi[0] - 1] - mesh.vertexCoords[f.vi[1] - 1];
    float3 normal = Cross(a, b);
    for (size_t i = 0; i < 3; ++i)
      normals[f.vi[i] - 1] += normal;
  }
  for (float3& n : normals)
    Normalize(n);
  return normals;
}


double lpng::DistFromPointToFace(const float3& point, const float3& a, const float3& b, const float3& c)
{
  float3 n = Cross(a - b, c - b);
  Normalize(n);
  float d = -Dot(n, a);
  return abs(Dot(n, point) + d);
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
    for (size_t j = 0; j < f_i.vi.size(); j++)
    {
      int e1 = f_i.vi[j];
      int e2 = f_i.vi[(j + 1) % f_i.vi.size()];
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
        peripheryEdges.emplace_back(e1, e2);
    }
  }
}


std::vector<int> lpng::ExtrudeWithCap(Mesh& mesh, const std::vector<int>& facesIds, const float3& vec)
{
  std::vector<Edge> periphery_edges;
  SetPeripheryEdges(mesh, facesIds, periphery_edges);
  std::unordered_map<int, int> extruded_vertex_ids;
  std::vector<int> extruded_faces_ids;

  for (int fi : facesIds)
  {
    Face new_face = mesh.faces[fi];
    for (int& v : new_face.vi)
    {
      if (!extruded_vertex_ids.contains(v))
      {
        mesh.vertexCoords.emplace_back(mesh.vertexCoords[v - 1] + vec);
        extruded_vertex_ids[v] = mesh.vertexCoords.size();
      }
      v = extruded_vertex_ids[v];
    }
    std::reverse(mesh.faces[fi].vi.begin(), mesh.faces[fi].vi.end());
    mesh.faces.push_back(new_face);
    extruded_faces_ids.push_back(mesh.faces.size() - 1);
  }

  for (const Edge& e : periphery_edges)
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
  for (size_t i = 0; i < mesh.faces.size(); ++i)
  {
    if (std::find(facesIds.begin(), facesIds.end(), i) == facesIds.end())
      new_faces.push_back(std::move(mesh.faces[i]));
  }
  mesh.faces = std::move(new_faces);
  for (int& f_id : extruded_faces_ids)
    f_id -= facesIds.size();
  return extruded_faces_ids;
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
    float x = fast_lpng_rand(from_x * 10000.f, to_x * 10000.f) / 10000.f;
    float y = fast_lpng_rand(from_y * 10000.f, to_y * 10000.f) / 10000.f;
    float z = fast_lpng_rand(from_z * 10000.f, to_z * 10000.f) / 10000.f;
    float3 p(x, y, z);
    float3 p_sq = p * p;
    float3 t = p_sq / rad_sq;
    if (t.x + t.y + t.z <= 1 && t.x + t.y + t.z > 0.5)
      points.push_back(p);
  }
  for (float3& p : points)
  {
    p /= rad;
    Normalize(p);
    p *= rad;
  }
  FilterNearestPoints(points);
  return points;
}


void lpng::FilterNearestPoints(std::vector<float3>& points, float d)
{
  std::vector<float3> new_points;
  new_points.push_back(std::move(points[0]));
  for (size_t i = 1; i < points.size(); ++i)
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
      new_points.push_back(std::move(points[i]));
  }
  points = std::move(new_points);
}


void lpng::DeleteUnusedVertices(Mesh& mesh)
{
  std::unordered_set<int> used_vertices_id;
  int max_used = 0;
  for (const Face& f : mesh.faces)
  {
    for (int v : f.vi)
    {
      used_vertices_id.insert(v - 1);
      if (v - 1 > max_used)
        max_used = v - 1;
    }
  }
  for (int i = mesh.vertexCoords.size() - 1; i >= 0; --i)
  {
    if (i > max_used)
      mesh.vertexCoords.erase(mesh.vertexCoords.begin() + i);
    else if (auto it = used_vertices_id.find(i); it == used_vertices_id.end())
    {
      for (Face& f : mesh.faces)
      {
        for (int& v : f.vi)
        {
          if (v - 1 > i)
            v -= 1;
        }
      }
      mesh.vertexCoords.erase(mesh.vertexCoords.begin() + i);
    }
  }
}


bool lpng::SortEdges(std::vector<Edge>& edges)
{
  std::vector<Edge> sorted_edges;
  sorted_edges.push_back(edges.back());
  edges.pop_back();
  while (edges.size() > 0)
  {
    const Edge& e = sorted_edges.back();
    auto e_it = std::find_if(edges.begin(), edges.end(), [&](Edge edge) { return e.second == edge.first; });
    if (e_it != edges.end())
    {
      sorted_edges.push_back(*e_it);
      edges.erase(e_it);
    }
    else
      return false;
  }
  edges = std::move(sorted_edges);
  return true;
}


void lpng::CupFromEdges(const Mesh& mesh, std::vector<Face>& faces, std::vector<Edge> edges)
{
  if (edges.size() > 3)
  {
    float min_angle = 7;
    int e = 0;
    float3 normal;
    int f_id = FindFaceWithEdge(faces, edges[0]);
    if (f_id >= 0)
    {
      normal = FaceNormal(mesh, faces[f_id]);
    }
    else
    {
      f_id = FindFaceWithEdge(mesh.faces, edges[0]);
      normal = FaceNormal(mesh, mesh.faces[f_id]);
    }
    for (int e1 = 1; e1 < edges.size() - 1; ++e1)
    {
      float3 a = mesh.vertexCoords[edges[0].first - 1] - mesh.vertexCoords[edges[0].second - 1];
      float3 b = mesh.vertexCoords[edges[e1].second - 1] - mesh.vertexCoords[edges[0].second - 1];
      float3 new_normal = Normalized(Cross(b, a));
      float angle = Angle(new_normal, normal);
      if (angle < min_angle)
      {
        min_angle = angle;
        e = e1;
      }
    }
    faces.push_back(Face({ edges[0].first, edges[0].second, edges[e].second }));
    if (e == 1)
    {
      Edge new_edge(edges[0].first, edges[e].second);
      edges.erase(edges.begin() + e);
      edges[0] = new_edge;
      CupFromEdges(mesh, faces, edges);
    }
    else if (e == edges.size() - 2)
    {
      Edge new_edge(edges[e].second, edges[0].second);
      edges.erase(edges.begin() + e + 1);
      edges[0] = new_edge;
      CupFromEdges(mesh, faces, edges);
    }
    else
    {
      Edge new_edge1(edges[e].second, edges[0].second);
      Edge new_edge2(edges[0].first, edges[e].second);
      std::vector<Edge> edges1;
      std::vector<Edge> edges2;
      edges1.insert(edges1.end(), edges.begin() + 1, edges.begin() + e + 1);
      edges1.push_back(new_edge1);
      edges2.insert(edges2.end(), edges.begin() + e + 1, edges.end());
      edges2.push_back(new_edge2);
      CupFromEdges(mesh, faces, edges1);
      CupFromEdges(mesh, faces, edges2);
    }
  }
  else
  {
    faces.push_back(Face({ edges[0].first, edges[0].second, edges[1].second }));
  }
  return;
}


void lpng::FilterMeshWithPoints(Mesh& mesh, const std::unordered_set<size_t>& vertices_ids)
{
  std::unordered_set<size_t> error_vertices;
  for (int i = 0; i < mesh.vertexCoords.size(); ++i)
  {
    if (auto it = vertices_ids.find(i); it == vertices_ids.end())
    {
      std::vector<size_t> faces_ids = FindFacesInMesh(mesh, i + 1);
      if (faces_ids.size() < 3)
      {
        error_vertices.insert(i);
        continue;
      }
      std::sort(faces_ids.begin(), faces_ids.end(), std::greater<size_t>());
      std::vector<Edge> edges(faces_ids.size());
      for (int j = 0; j < faces_ids.size(); ++j)
      {
        const Face& f = mesh.faces[faces_ids[j]];
        int v_id = std::find(f.vi.begin(), f.vi.end(), i + 1) - f.vi.begin();
        edges[j] = {f.vi[(v_id + 1) % f.vi.size()], f.vi[(v_id + 2) % f.vi.size()]};
      }
      if (!SortEdges(edges))
      {
        error_vertices.insert(i);
        continue;
      }
      std::vector<Face> new_faces;
      CupFromEdges(mesh, new_faces, edges);
      for (int j = 0; j < new_faces.size(); ++j)
      {
        mesh.faces[faces_ids[j]] = std::move(new_faces[j]);
      }
      int k = 1;
      for (int j = new_faces.size(); j < faces_ids.size(); ++j)
      {
        mesh.faces[faces_ids[j]] = std::move(mesh.faces[mesh.faces.size() - k]);
        ++k;
      }
      mesh.faces.resize(mesh.faces.size() - k + 1);
    }
  }
  DeleteUnusedVertices(mesh);
}


std::vector<lpng::float2> lpng::GetNDirections(int n)
{
  std::vector<lpng::float2> dirs;
  float seg_angle = 2 * M_PI / n;
  float angle_cum = DEG2RAD(fast_lpng_rand(0, 360));
  for (int i = 0; i < n; ++i)
  {
    dirs.emplace_back(sin(angle_cum), cos(angle_cum));
    float angle = seg_angle * (fast_lpng_rand(700, 1300) / 1000.f);
    angle_cum += angle;
  }
  return dirs;
}


lpng::float3 lpng::GenOutVec(const float3& vecIn, int fromAngle, int toAngle)
{
  float3 vec(fast_lpng_rand(-50, 51), 0, fast_lpng_rand(-50, 51));
  Normalize(vec);
  if (vec == float3())
    vec.y = 1;
  else
  {
    int angle = fast_lpng_rand(fromAngle, toAngle);
    if (angle == 0)
      vec = float3(0, 1, 0);
    else
      vec.y = 1 / tan(DEG2RAD(angle));
  }
  float angle = Angle(float3(0, 1, 0), vecIn);
  if (angle > FLT_EPSILON)
  {
    Quat q(Cross(float3(0, 1, 0), vecIn), angle);
    vec = QuatTransformVec(q, vec);
  }
  return Normalized(vec);
}


lpng::float3 lpng::GenOutVec(std::vector<float2>& dirs, const float3& vecIn, int fromAngle, int toAngle)
{
  int d_id = fast_lpng_rand(0, dirs.size());
  float2 dir = dirs[d_id];
  dirs.erase(dirs.begin() + d_id);
  float3 vec = float3(dir.x, 0, dir.y);
  Normalize(vec);
  int branch_angle = fast_lpng_rand(fromAngle, toAngle);
  if (branch_angle == 0)
    vec = float3(0, 1, 0);
  else
    vec.y = 1 / tan(DEG2RAD(branch_angle));
  float angle = Angle(float3(0, 1, 0), vecIn);
  if (angle > FLT_EPSILON)
  {
    Quat q(Cross(float3(0, 1, 0), vecIn), angle);
    vec = QuatTransformVec(q, vec);
  }
  return Normalized(vec);
}