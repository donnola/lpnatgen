#include "lpnatgen.h"
#include <iostream>

void lpng::GenerateObject::Generate()
{
  GenerateMesh();
  PolygonDecomposition();
  GenerateNormals();
  GenerateTextureCoords();
}

void lpng::GenerateObject::GenerateNormals()
{
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
        f[i].vti = i+1;
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
          mean_vertex_coord += obj.vertexCoords[v.vi];
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

void lpng::GenerateObject::SaveModel() const
{

}

void lpng::GenerateObject::AddObject(Object& obj)
{
  model.push_back(std::move(obj));
}

void lpng::GenerateObject::ShowModel()
{

}
