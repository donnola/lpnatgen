#pragma once

#include "lpmath.h"

//#include <cstdlib>
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

  class GenerateObjectTree: public GenerateObject
  {
  public:
    GenerateObjectTree(float3 size) : GenerateObject(size) {}

    void GenerateMesh() override;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(float3 size) : GenerateObject(size) {}

    void GenerateMesh() override;
  };

  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float3 size = float3(1.5, 2, 1.5)) : GenerateObject(size) {}

    void GenerateMesh() override;
  };

  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest(float3 size = float3(1.5, 3, 1.5)) : GenerateObject(size) {}

    void GenerateMesh() override;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
