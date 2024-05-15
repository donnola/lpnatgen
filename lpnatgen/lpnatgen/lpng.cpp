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
  if (seed_set)
    fast_lpng_rand(seed);
  else
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
    if (smoothness)
    {
      std::vector<float3> normals = CalculateSmoothObjNormals(mesh);
      mesh.vertexNormals = std::move(normals);
      for (Face& f : mesh.faces)
      {
        f.vni = f.vi;
      }
    }
    else
    {
      std::vector<float3> normals = CalculateObjNormals(mesh);
      mesh.vertexNormals = std::move(normals);

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


void lpng::GenerateObject::SaveModel(const std::string& file_name, std::filesystem::path save_path) const
{
  std::string model_folder = file_name;
  save_path /= model_folder;
  while (std::filesystem::exists(save_path))
  {
    save_path = save_path.parent_path();
    model_folder += "_copy";
    save_path /= model_folder;
  }
  std::filesystem::create_directories(save_path);
  std::filesystem::path file_path = save_path / (file_name + fileFormat);
  std::ofstream ofs(file_path);
  writeModel(ofs, model, ModelMaterial::MatFile, ModelMaterial::MatName);
  ofs.close();
  ModelMaterial::CreateModelMaterial(save_path);
  ModelMaterial::CreateModelTexture(save_path);
}


void lpng::GenerateObject::SaveModel(const std::string& file_name, std::string save_path) const
{
  std::filesystem::path dir_path(save_path);
  SaveModel(file_name, dir_path);
}


void lpng::GenerateObject::SaveModel(const std::string& file_name) const
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


void lpng::writeFace(std::ostream& out, const Face& f, const IdsOffset& offs)
{
  out << "f";
  for (int i = 0; i < f.vi.size(); ++i)
  {
    Vertex v(f.vi[i] + offs.v, f.vti[i] + offs.vt, f.vni[i] + offs.vn);
    out << " " << v;
  }
  out << "\n";
}


void lpng::writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name, const std::string& mat_name)
{
  out << "g default\n";
  for (float3 v : m.vertexCoords)
    out << "v " << v;
  for (float2 vt : m.vertexTextCoords)
    out << "vt " << vt;
  for (float3 vn : m.vertexNormals)
    out << "vn " << vn;
  out << "s off\n";
  out << "g " << name << "\n";
  out << "usemtl " << mat_name << "\n";
  for (const Face& f : m.faces)
  {
    writeFace(out, f, offs);
  }
}


void lpng::writeModel(std::ostream& out, const std::vector<Mesh>& m, const std::string& mat_file, const std::string& mat_name)
{
  out << "# This file uses meters as units for non - parametric coordinates.\n\n";
  out << "mtllib " << mat_file << "\n";
  int size = m.size();
  IdsOffset offs;
  for (int i = 0; i < size; ++i)
  {
    writeMeshObj(out, m[i], offs, "obj_" + std::to_string(i+1), mat_name);
    offs.v += m[i].vertexCoords.size();
    offs.vt += m[i].vertexTextCoords.size();
    offs.vn += m[i].vertexNormals.size();
  }
}


void lpng::ModelMaterial::CreateModelTexture(const std::filesystem::path& save_path)
{
  std::filesystem::path file_path = save_path / TextureName;
  std::ofstream ofs(file_path, std::ios::binary);
  const int width = 256;
  const int height = 256;

  uint8_t header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0, (uint8_t)(width % 256), (uint8_t)(width / 256), (uint8_t)(height % 256), (uint8_t)(height / 256), 24, 0x20 };

  // Write the header to the file
  ofs.write(reinterpret_cast<const char*>(&header), 18);

  // Magenta color (24-bit)
  const unsigned char magenta[3] = { 255, 0, 255 };
  const unsigned char white[3] = { 255, 255, 255 };

  // Write the magenta texture to the file
  for (int y = 0; y < height; ++y)
  {
    for (int x = 0; x < width; ++x)
    {
      if (x % 2 == 1)
        ofs.write(reinterpret_cast<const char*>(magenta), sizeof(magenta));
      else
        ofs.write(reinterpret_cast<const char*>(white), sizeof(white));
    }
  }
  ofs.close();
}


void lpng::ModelMaterial::CreateModelMaterial(const std::filesystem::path& save_path)
{
  std::filesystem::path file_path = save_path / MatFile;
  std::ofstream ofs(file_path);
  ofs << "newmtl " << MatName << "\n";
  ofs << "illum 1\n";
  ofs << "Kd 1.00 1.00 1.00\n";
  ofs << "Ka 0.00 0.00 0.00\n";
  ofs << "Tf 1.00 1.00 1.00\n";
  ofs << "Ni 1.00\n";
  ofs << "Tr 1.00\n";
  ofs << "map_Kd " << TextureName << "\n";
  ofs.close();
}