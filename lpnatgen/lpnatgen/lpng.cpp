#include "lpng.h"
#include "lpng_math.h"
#include <filesystem>
#include <stack>
#include <set>


std::vector<lpng::Mesh> lpng::GenerateObject::GetModel()
{
  PolygonDecomposition();
  return model;
}

void lpng::GenerateObject::Generate()
{
  seed = get_lpng_seed();
  GenerateMesh();
  PolygonDecomposition();
  GenerateNormals();
  GenerateTextureCoords();
}

void lpng::GenerateObject::GenerateNormals()
{
  for (Mesh& mesh : model)
  {
    std::vector<float3> normals = CalculateObjNormals(mesh);

    mesh.vertexNormals = std::move(normals);
    for (Face& f : mesh.faces)
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
  for (Mesh& mesh : model)
  {
    mesh.vertexTextCoords.emplace_back(0.f, 0.f);
    mesh.vertexTextCoords.emplace_back(1.f, 0.f);
    mesh.vertexTextCoords.emplace_back(0.f, 1.f);

    for (Face& f : mesh.faces)
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
  for (Mesh& mesh : model)
  {
    DecomposeObj(mesh);
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

void lpng::GenerateObject::AddObject(Mesh& mesh)
{
  model.push_back(std::move(mesh));
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
