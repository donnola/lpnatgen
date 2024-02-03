#pragma once

#include "lpng_math.h"
#include "lpng_rand.h"

#include <string>


namespace lpng
{
  class GenerateObject
  {
  public:
    GenerateObject(float3 size) : objectSize(size) {}

    GenerateObject(float size) : objectSize({0, size, 0}) {}

    void AddObject(Mesh& mesh);
    virtual void GenerateMesh() = 0;
    virtual void GenerateTextureCoords();
    void PolygonDecomposition();
    void GenerateNormals();
    void SaveModel(std::string file_name, std::string save_path) const;
    void SaveModel() const;
    void Generate();
    std::vector<Mesh> GetModel();
    unsigned int GetModelSeed() { return seed; }
    
  protected:
    const float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    std::string fileFormat = ".obj";
    unsigned int seed;
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
