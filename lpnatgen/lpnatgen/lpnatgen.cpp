#include "lpnatgen.h"
#include "lpmath.h"
#include <filesystem>
#include <stack>
#include <set>


std::vector<lpng::Object> lpng::GenerateObject::GetModel()
{
  PolygonDecomposition();
  return model;
}

void lpng::GenerateObject::Generate()
{
  GenerateMesh();
  PolygonDecomposition();
  GenerateNormals();
  GenerateTextureCoords();
}

void lpng::GenerateObject::GenerateNormals()
{
  for (Object& obj : model)
  {
    std::vector<float3> normals = CalculateObjNormals(obj);

    obj.vertexNormals = std::move(normals);
    for (Face& f : obj.faces)
    {
      for (size_t i = 0; i < 3; ++i)
      {
        f[i].vni = f[i].vi;
      }
    }
  }
}

void lpng::GenerateObject::GenerateTextureCoords()
{
  for (Object& obj : model)
  {
    obj.vertexTextCoords.emplace_back(0.f, 0.f);
    obj.vertexTextCoords.emplace_back(1.f, 0.f);
    obj.vertexTextCoords.emplace_back(0.f, 1.f);

    for (Face& f : obj.faces)
    {
      for (size_t i = 0; i < 3; ++i)
      {
        f[i].vti = i + 1;
      }
    }
  }
}

void lpng::GenerateObject::PolygonDecomposition()
{
  for (Object& obj : model)
  {
    DecomposeObj(obj);
  }
}

void lpng::GenerateObject::SaveModel(std::string file_name, std::string save_path) const
{

}

void lpng::GenerateObject::SaveModel() const
{
  std::string file_name = "test_object";
  std::string save_path = std::filesystem::current_path().string();
}

void lpng::GenerateObject::AddObject(Object& obj)
{
  model.push_back(std::move(obj));
}

std::vector<lpng::Face> lpng::GenerateObject::GenerateMinConvexHull(const std::vector<float3>& points)
{
  return {};
}

std::vector<lpng::Face> lpng::GenerateObject::GenerateConvexHullFull(const std::vector<float3>& points)
{
  std::vector<Face> faces = GenerateMinConvexHull(points);
  std::set<int> vertexIds;
  for (const Face& f : faces)
  {
    for (const Vertex& v : f)
    {
      vertexIds.insert(v.vi - 1);
    }
  }
  for (int i = 0; i < points.size(); ++i)
  {
    if (vertexIds.find(i) == vertexIds.end())
    {
      double minDisToFace = FLT_MAX;
      int nearesFaceId = -1;
      for (int j = 0; j < faces.size(); ++j)
      {
        float3 a = points[faces[j][0].vi - 1];
        float3 b = points[faces[j][1].vi - 1];
        float3 c = points[faces[j][2].vi - 1];
        double dist = DistFromPointToFace(points[i], a, b, c);
        if (dist > 0 && dist < minDisToFace)
        {
          minDisToFace = dist;
          nearesFaceId = j;
        }
      }
      SplitFaceMithPoint(faces, nearesFaceId, i);
    }
  }
  return faces;
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
  return points;
}