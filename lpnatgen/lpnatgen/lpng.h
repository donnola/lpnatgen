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
    void GenerateNormals(bool smoothness);
    void SaveModel(const std::string& file_name, std::filesystem::path save_path) const;
    void SaveModel(const std::string& file_name, std::string save_path) const;
    void SaveModel(const std::string& file_name) const;
    void SaveModel() const;
    void Generate(bool smoothness = true);
    std::vector<Mesh> GetModel();
    unsigned int GetModelSeed() { return seed; }
    
  protected:
    float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    const std::string fileFormat = ".obj";
    const std::string modelsFolder = "models";
    unsigned int seed = 0;
  };


  class ModelMaterial
  {
  public:
    static void CreateModelTexture(const std::filesystem::path& save_path);
    static void CreateModelMaterial(const std::filesystem::path& save_path);
    static const inline std::string MatName = "obj_mat";
    static const inline std::string MatFile = "material.mtl";
    static const inline std::string TextureName = "texture.png";
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

  void writeFace(std::ostream& out, const Face& v, const IdsOffset& offs);

  void writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name, const std::string& mat_name);

  void writeModel(std::ostream& out, const std::vector<Mesh>& m, const std::string& mat_file, const std::string& mat_name);
}
