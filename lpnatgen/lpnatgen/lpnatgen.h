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
    GenerateObject(
      float size, 
      std::string file_name, 
      std::string save_path, 
      int seed = -1) : objectSize(size), fileName(file_name), savePath(save_path)
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
    void SaveModel() const;
    void Generate();
    void ShowModel();

  private:
    float objectSize = 0;
    int objectSeed = 0;
    std::string fileName = "temp.obj";
    std::string savePath = "D:\\";

    std::vector<Object> model;
  };

  class GenerateObjectTree: public GenerateObject
  {
  public:
    GenerateObjectTree(
      float size,
      std::string file_name,
      std::string save_path,
      int seed = -1) : GenerateObject(size, file_name, save_path, seed) {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(
      float size,
      std::string file_name,
      std::string save_path,
      int seed = -1) : GenerateObject(size, file_name, save_path, seed)
    {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(
      float size,
      std::string file_name,
      std::string save_path,
      int seed = -1) : GenerateObject(size, file_name, save_path, seed)
    {}

    void GenerateMesh() override;
    void GenerateTextureCoords() override;
  };

  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest(
      float size,
      std::string file_name,
      std::string save_path,
      int seed = -1) : GenerateObject(size, file_name, save_path, seed)
    {}

    void GenerateMesh() override {};
  };
}
