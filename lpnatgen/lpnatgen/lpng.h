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

    void SetModelSeed(unsigned int s)
    {
      seed = s;
      seed_set = true;
    }
    void SetModelSmoothness(bool s)
    {
      smoothness = s;
    }
    void SetSize(float3 size) { objectSize = size; }
    void SaveModel(const std::string& file_name, std::filesystem::path save_path) const;
    void SaveModel(const std::string& file_name, std::string save_path) const;
    void SaveModel(const std::string& file_name) const;
    void SaveModel() const;
    void Generate();
    const std::vector<Mesh>& GetModel();
    unsigned int GetModelSeed() { return seed; }
    
  protected:
    virtual void GenerateMesh() = 0;
    void AddObject(Mesh& mesh);
    void GenerateTextureCoords();
    void ModelPolygonDecomposition();
    void GenerateNormals();
    float3 objectSize = float3(1,1,1);
    std::vector<Mesh> model;
    const std::string fileFormat = ".obj";
    const std::string modelsFolder = "models";
    unsigned int seed = 0;
    bool seed_set = false;
    bool smoothness = true;
  };


  class ModelMaterial
  {
  public:
    static void CreateModelTexture(const std::filesystem::path& save_path);
    static void CreateModelMaterial(const std::filesystem::path& save_path);
    static const inline std::string MatFile = "material.mtl";
    static const inline std::string TextureName = "texture.tga";
    static const inline std::unordered_map<int, std::string> TextureNames =
    {
      {int(MaterialTypes::NONE), "obj.tga"},
      {int(MaterialTypes::WOOD), "wood.tga"},
      {int(MaterialTypes::CROWN), "crown.tga"},
      {int(MaterialTypes::STONE), "stone.tga"}
    };
    static const inline std::unordered_map<int, std::string> MatNames =
    {
      {int(MaterialTypes::NONE), "obj_mat"},
      {int(MaterialTypes::WOOD), "wood_mat"},
      {int(MaterialTypes::CROWN), "crown_mat"},
      {int(MaterialTypes::STONE), "stone_mat"}
    };
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

  void writeMeshObj(std::ostream& out, const Mesh& m, const IdsOffset& offs, const std::string& name);

  void writeModel(std::ostream& out, const std::vector<Mesh>& m);
}
