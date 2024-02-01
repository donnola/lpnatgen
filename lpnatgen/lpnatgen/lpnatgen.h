#pragma once

#include "lpmath.h"

#include <ctime>
#include <string>


namespace lpng
{
  class GenerateObject
  {
  public:
    GenerateObject(float3 size) : objectSize(size)
    {
      std::srand(std::time(0));
    }

    GenerateObject(float size) : objectSize({0, size, 0})
    {
      std::srand(std::time(0));
    }

    void AddObject(Mesh& mesh);
    virtual void GenerateMesh() = 0;
    virtual void GenerateTextureCoords();
    void PolygonDecomposition();
    void GenerateNormals();
    void SaveModel(std::string file_name, std::string save_path) const;
    void SaveModel() const;
    void Generate();
    std::vector<Mesh> GetModel();
    
  protected:
    const float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    std::string fileFormat = ".obj";
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(float3 size = float3(1.5, 2, 1.5)) : GenerateObject(size) {}

    void GenerateMesh() override;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
