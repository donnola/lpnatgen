#pragma once

#include "desc.h"

#include <string>
#include <vector>


namespace lpng
{
  enum class ObjectTypes
  {
    TREE,
    BUSH,
    STONE
  };

  class GenerateObject
  {
  public:
    GenerateObject(float size, int seed = -1) : objectSize(size)
    {
      if (seed > 0)
        objectSeed = seed;
      else
        objectSeed = 1;
    }

    void AddObject(Object& obj);
    virtual void GenerateMesh() = 0;
    virtual void GenerateTextureCoords();
    void PolygonDecomposition();
    void GenerateNormals();
    void SaveModel(std::string file_name, std::string save_path) const;
    void SaveModel() const;
    void Generate();
    std::vector<Object> GetModel();

  private:
    float objectSize = 0;
    int objectSeed = 0;
    std::string fileFormat = ".obj";
    std::vector<Object> model;
  };

  class GenerateObjectTree: public GenerateObject
  {
  public:
    GenerateObjectTree(float size, int seed = -1) : GenerateObject(size, seed) {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(float size, int seed = -1) : GenerateObject(size, seed) {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float size, int seed = -1) : GenerateObject(size, seed) {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest() : GenerateObject(1.f) {}

    void GenerateMesh() override;
  };
}
