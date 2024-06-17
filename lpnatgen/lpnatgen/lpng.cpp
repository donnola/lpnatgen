#include "lpng.h"
#include "lpng_math.h"
#include <stack>
#include <fstream>


const std::vector<lpng::Mesh>& lpng::GenerateObject::GetModel()
{
  Triangulation();
  return model;
}


void lpng::GenerateObject::Generate()
{
  if (seed_set)
    fast_lpng_rand(seed);
  else
    seed = get_lpng_seed();
  GenerateMesh();
  for (Mesh& m : model)
  {
    DeleteUnusedVertices(m);
  }
  Triangulation();
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
      for (int i = 0; i < mesh.faces.size(); ++i)
      {
        mesh.faces[i].vni = std::move(std::vector<int>(mesh.faces[i].vi.size(), i + 1));
      }
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


void lpng::GenerateObject::Triangulation()
{
  for (Mesh& mesh : model)
  {
    TriangulateMesh(mesh);
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
  writeModel(ofs, model);
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


void lpng::writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name)
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
  out << "usemtl " << ModelMaterial::MatNames.at(int(m.matType)) << "\n";
  for (const Face& f : m.faces)
  {
    writeFace(out, f, offs);
  }
}


void lpng::writeModel(std::ostream& out, const std::vector<Mesh>& m)
{
  out << "# This file uses meters as units for non - parametric coordinates.\n\n";
  out << "mtllib " << ModelMaterial::MatFile << "\n";
  int size = m.size();
  IdsOffset offs;
  for (int i = 0; i < size; ++i)
  {
    writeMeshObj(out, m[i], offs, "obj_" + std::to_string(i+1));
    offs.v += m[i].vertexCoords.size();
    offs.vt += m[i].vertexTextCoords.size();
    offs.vn += m[i].vertexNormals.size();
  }
}


void lpng::ModelMaterial::CreateModelTexture(const std::filesystem::path& save_path)
{
  if (!std::filesystem::exists(save_path))
  {
    std::filesystem::create_directories(save_path);
  }
  const unsigned char base[3] = { 125, 125, 125 };
  const unsigned char brown[3] = { 14, 34, 77 };
  const unsigned char green[3] = { 38, 129, 23 };
  const unsigned char grey[3] = { 84, 84, 84 };

  for (const auto& name : TextureNames)
  {
    std::filesystem::path file_path = save_path / name.second;
    if(std::filesystem::exists(file_path))
    {
      continue;
    }
    std::ofstream ofs(file_path, std::ios::binary);
    const int width = 128;
    const int height = 128;
    uint8_t header[18] = { 0,0,2,0,0,0,0,0,0,0,0,0, (uint8_t)(width % 256), (uint8_t)(width / 256), (uint8_t)(height % 256), (uint8_t)(height / 256), 24, 0x20 };
    ofs.write(reinterpret_cast<const char*>(&header), 18);
    for (int y = 0; y < height; ++y)
    {
      for (int x = 0; x < width; ++x)
      {
        if (name.first == int(MaterialTypes::NONE))
        {
          ofs.write(reinterpret_cast<const char*>(base), sizeof(base));
        }
        else if (name.first == int(MaterialTypes::WOOD))
        {
          ofs.write(reinterpret_cast<const char*>(brown), sizeof(brown));
        }
        else if (name.first == int(MaterialTypes::CROWN))
        {
          ofs.write(reinterpret_cast<const char*>(green), sizeof(green));
        }
        else if (name.first == int(MaterialTypes::STONE))
        {
          ofs.write(reinterpret_cast<const char*>(grey), sizeof(grey));
        }    
      }
    }
    ofs.close();
  }
}


void lpng::ModelMaterial::CreateModelMaterial(const std::filesystem::path& save_path)
{
  if (!std::filesystem::exists(save_path))
  {
    std::filesystem::create_directories(save_path);
  }
  std::filesystem::path file_path = save_path / MatFile;
  if (std::filesystem::exists(file_path))
  {
    return;
  }
  std::ofstream ofs(file_path);
  for (const auto& mat : MatNames)
  {
    ofs << "newmtl " << mat.second << "\n";
    ofs << "illum 1\n";
    ofs << "Kd 1.00 1.00 1.00\n";
    ofs << "Ka 1.00 1.00 1.00\n";
    ofs << "Ni 1.00\n";
    ofs << "Tr 1.00\n";
    ofs << "map_Kd " << TextureNames.at(mat.first) << "\n";
  }
  ofs.close();
}