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
    void GenerateTextureCoords();
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

  struct BranchRing
  {
    float3 vecIn;
    float3 vecOut;
    float rad;
    float3 center;
    float curLength;
    std::vector<int> facesIds;
    std::vector<int> vertexesIds;
  };

  struct Branch
  {
    std::vector<BranchRing> rings;
    std::vector<int> childsIds;
    std::vector<float2> freeDirections;
    float length = 0;
    float baseRad = 0;
    float lastRad = 0;
    int level = 0;
    size_t weight = 0;
    int edgeBase = 0;
  };

  class GenerateObjectPlant : public GenerateObject
  {
  public:
    GenerateObjectPlant() {}
  protected:
    virtual void GenerateBranch(Branch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut = float3());
    virtual void RelaxBranch(Branch& branch, size_t meshId);
    virtual void InitBranch(const size_t parent_id, Branch& branch, float3& point_start, float3& vec_in);
    virtual void GenerateCrown() = 0;
    virtual void ModifyCrown(Mesh& crown, const float3& c) = 0;
    int SelectWeightedBranch();
    std::vector<Branch> branches;
    float upCoef = 0;
    float finalBranchRad = 0.03;
    int branchBase = 5;
    float branchMinCoefStart = 0;
    float branchMaxCoefStart = 1;
    float branchMinCoefLen = 0;
    float branchMaxCoefLen = 1;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
