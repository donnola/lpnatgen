#pragma once

#include "lpng.h"

namespace lpng
{
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
    virtual int SelectWeightedBranch();
    virtual void InitBranch(const size_t parent_id, Branch& branch, float3& point_start, float3& vec_in);
    virtual void GenerateBranch(Branch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut = float3());
    virtual void RelaxBranch(Branch& branch, size_t meshId);
    virtual void GenerateCrown() = 0;
    virtual void ModifyCrown(Mesh& crown, const float3& c) = 0;
    std::vector<Branch> branches;
    float upCoef = 0;
    float finalBranchRad = 0.03;
    int branchBase = 5;
    float branchMinCoefStart = 0;
    float branchMaxCoefStart = 1;
    float branchMinCoefLen = 0;
    float branchMaxCoefLen = 1;
    int branchAngleMin = 0;
    int branchAngleMax = 180;
  };
}
