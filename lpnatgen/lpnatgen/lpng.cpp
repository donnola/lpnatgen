#include "lpng.h"
#include "lpng_math.h"
#include <filesystem>
#include <stack>


std::vector<lpng::Mesh> lpng::GenerateObject::GetModel()
{
  PolygonDecomposition();
  return model;
}

void lpng::GenerateObject::Generate()
{
  //fast_lpng_rand(1780338363);
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
      f.vni = f.vi;
    }
  }
}

void lpng::GenerateObject::GenerateTextureCoords()
{
  for (Mesh& mesh : model)
  {
    mesh.vertexTextCoords.emplace_back(0.f, 0.f);
    mesh.vertexTextCoords.emplace_back(0.9, 0.f);
    mesh.vertexTextCoords.emplace_back(0.f, 0.9);

    for (Face& f : mesh.faces)
    {
      for (size_t i = 0; i < 3; ++i)
      {
        f.vti = {1, 2, 3};
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
  for (int i = 0; i < f.vi.size(); ++i)
  {
    Vertex v(f.vi[i], f.vti[i], f.vni[i]);
    out << ' ' << v;
  }
  return out << "\n";
}
