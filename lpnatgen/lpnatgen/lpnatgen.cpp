#include "lpnatgen.h"
#include <filesystem>

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
    std::vector<float3> normals(obj.vertexCoords.size());
    for (Face& f : obj.faces)
    {
      float3 a = obj.vertexCoords[f[2].vi - 1] - obj.vertexCoords[f[1].vi - 1];
      float3 b = obj.vertexCoords[f[0].vi - 1] - obj.vertexCoords[f[1].vi - 1];
      float3 normal = Cross(a, b);
      for (size_t i = 0; i < 3; ++i)
      {
        normals[f[i].vi - 1] += normal;
      }
    }
    for (float3& n : normals)
    {
      n.Normalize();
    }
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
    std::vector<Face> new_faces;
    for (Face& f : obj.faces)
    {
      if (f.size() < 3)
      {
        continue;
      }
      if (f.size() == 3)
      {
        new_faces.push_back(f);
      }
      else if (f.size() == 4)
      {
        new_faces.push_back(Face({f[0], f[1], f[2]}));
        new_faces.push_back(Face({f[2], f[3], f[0]}));
      }
      else
      {
        float3 mean_vertex_coord;
        for (Vertex& v : f)
        {
          mean_vertex_coord += obj.vertexCoords[v.vi - 1];
        }
        obj.vertexCoords.push_back(mean_vertex_coord);
        size_t center_index = obj.vertexCoords.size();
        Vertex mean_vertex(center_index);
        for (int i = 0; i < f.size(); ++i)
        {
          new_faces.push_back(Face({f[i], f[(i+1) % f.size()], mean_vertex}));
        }
      }
    }
    obj.faces = std::move(new_faces);
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
