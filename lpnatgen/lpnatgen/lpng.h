#pragma once

#include "lpng_math.h"
#include "lpng_rand.h"

#include <string>


namespace lpng
{
  class GenerateObject
  {
  public:
    GenerateObject() {}

    void SetModelSeed(int seed) { fast_lpng_rand(seed); }
    void SetSize(float3 size) { objectSize = size; }
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
    float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    std::string fileFormat = ".obj";
    unsigned int seed = 0;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(){}

    void GenerateMesh() override;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
