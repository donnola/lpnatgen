#pragma once

#include "desc.h"

#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>


namespace lpng
{
  class GenerateObject
  {
  public:
    GenerateObject(float size) : objectSize(size)
    {
      std::srand(std::time(0));
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

  protected:
    const float objectSize = 0;
    std::vector<Object> model;
    std::string fileFormat = ".obj";
  };

  class GenerateObjectTree: public GenerateObject
  {
  public:
    GenerateObjectTree(float size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(float size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override; 
  };

  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override;
  private:
    void GenerateUniformPoints(Object& obj, int pointsNum = 10);
    void GenerateRandomConvexHull(Object& obj, const float3& size = float3(1,1,1));
    void AddFreeVertexesToModel(Object& obj);
  };

  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest() : GenerateObject(1.f) {}

    void GenerateMesh() override;
  };
}
