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

inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::float3& v)
{
  return out << v.x << ' ' << v.y << ' ' << v.z << "\n";
}

inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::float2& v)
{
  return out << v.x << ' ' << v.y << "\n";
}

inline std::ostream& lpng::operator<<(std::ostream& out, const lpng::Vertex& v)
{
  return out << v.vi << '/' << v.vti << '/' << v.vni;
}

std::ostream& lpng::operator<<(std::ostream& out, const lpng::Face& f)
{
  out << 'f';
  for (const Vertex& v : f)
  {
    out << ' ' << v;
  }
  return out << "\n";
}
