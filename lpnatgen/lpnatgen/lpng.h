#pragma once

#include "lpng_math.h"
#include "lpng_rand.h"

#include <filesystem>
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
    void GenerateTextureCoords();
    void PolygonDecomposition();
    void GenerateNormals();
    void SaveModel(std::string file_name, const std::filesystem::path& save_path) const;
    void SaveModel(std::string file_name, const std::string& save_path) const;
    void SaveModel(std::string file_name) const;
    void SaveModel() const;
    void Generate();
    std::vector<Mesh> GetModel();
    unsigned int GetModelSeed() { return seed; }
    
  protected:
    float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    std::string fileFormat = ".obj";
    std::string modelsFolder = "models";
    unsigned int seed = 0;
  };

  struct IdsOffset
  {
    int v;
    int vt;
    int vn;
    IdsOffset() : v(0), vt(0), vn(0) {}
    IdsOffset(int a, int b, int c) : v(a), vt(b), vn(c) {}
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& writeFace(std::ostream& out, const Face& v, const IdsOffset& offs);

  std::ostream& writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name);

  std::ostream& operator<<(std::ostream& out, const std::vector<Mesh>& m);
}
