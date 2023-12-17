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
    GenerateObject(float3 size) : objectSize(size)
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
    std::vector<Face> GenerateMinConvexHull(const std::vector<float3>& points);
    std::vector<Face> GenerateConvexHullFull(const std::vector<float3>& points);
    std::vector<Object> GetModel();

  protected:
    const float3 objectSize = float3(1,1,1);
    std::vector<Object> model;
    std::string fileFormat = ".obj";
  };

  class GenerateObjectTree: public GenerateObject
  {
  public:
    GenerateObjectTree(float3 size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override;
  };

  class GenerateObjectBush : public GenerateObject
  {
  public:
    GenerateObjectBush(float3 size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override; 
  };

  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float3 size) : GenerateObject(size) {}

    void GenerateMesh() override;
    //void GenerateTextureCoords() override;
  };

  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest(float3 size = float3(1, 2, 1)) : GenerateObject(size) {}

    void GenerateMesh() override;
  };


  std::vector<float3> GenerateEllipsoidUniformPoints(const float3& size, int pointsNum = 10);
}
