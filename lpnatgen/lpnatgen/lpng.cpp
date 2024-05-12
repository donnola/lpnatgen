#include "lpng.h"
#include "lpng_math.h"
#include <stack>
#include <fstream>


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


void lpng::GenerateObject::SaveModel(std::string file_name, const std::filesystem::path& save_path) const
{
  std::filesystem::create_directories(save_path);
  std::filesystem::path file_path = save_path / (file_name + fileFormat);
  std::ofstream ofs;
  while (std::filesystem::exists(file_path))
  {
    file_path = file_path.parent_path();
    file_name += "_copy";
    file_path /= file_name + fileFormat;
  }
  ofs.open(file_path);
  ofs << model;
  ofs.close();
}


void lpng::GenerateObject::SaveModel(std::string file_name, const std::string& save_path) const
{
  std::filesystem::path dir_path(save_path);
  SaveModel(file_name, dir_path);
}


void lpng::GenerateObject::SaveModel(std::string file_name) const
{
  std::filesystem::path dir_path = std::filesystem::current_path();
  dir_path /= modelsFolder;
  SaveModel(file_name, dir_path);
}


void lpng::GenerateObject::SaveModel() const
{
  SaveModel("test_object");
}


void lpng::GenerateObject::AddObject(Mesh& mesh)
{
  model.push_back(std::move(mesh));
}


inline std::ostream& lpng::operator<<(std::ostream& out, const float3& v)
{
  return out << v.x << ' ' << v.y << ' ' << v.z << "\n";
}


inline std::ostream& lpng::operator<<(std::ostream& out, const float2& v)
{
  return out << v.x << ' ' << v.y << "\n";
}


inline std::ostream& lpng::operator<<(std::ostream& out, const Vertex& v)
{
  return out << v.vi << '/' << v.vti << '/' << v.vni;
}


std::ostream& lpng::writeFace(std::ostream& out, const Face& f, const IdsOffset& offs)
{
  out << "f";
  for (int i = 0; i < f.vi.size(); ++i)
  {
    Vertex v(f.vi[i] + offs.v, f.vti[i] + offs.vt, f.vni[i] + offs.vn);
    out << " " << v;
  }
  return out << "\n";
}


std::ostream& lpng::writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name)
{
  out << "g default\n";
  for (float3 v : m.vertexCoords)
    out << "v " << v;
  for (float2 vt : m.vertexTextCoords)
    out << "vt " << vt;
  for (float3 vn : m.vertexNormals)
    out << "vn " << vn;
  out << "s off\n";
  out << name << "\n";
  for (const Face& f : m.faces)
  {
    writeFace(out, f, offs);
  }
  return out;
}


std::ostream& lpng::operator<<(std::ostream& out, const std::vector<Mesh>& m)
{
  out << "# This file uses meters as units for non - parametric coordinates.\n";
  int size = m.size();
  IdsOffset offs;
  for (int i = 0; i < size; ++i)
  {
    writeMeshObj(out, m[i], offs, "obj_" + std::to_string(i+1));
    offs.v += m[i].vertexCoords.size();
    offs.vt += m[i].vertexTextCoords.size();
    offs.vn += m[i].vertexNormals.size();
  }
  return out;
}
