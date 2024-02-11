#pragma once

#include "lpng.h"

namespace lpng
{
  struct TreeRing
  {
    float3 vecIn;
    float3 vecOut;
    float rad;
    float3 center;
    float curLength;
    std::vector<int> facesIds;
    std::vector<int> vertexesIds;
  };

  struct TreeBranch
  {
    std::vector<TreeRing> rings;
    std::vector<TreeBranch*> childs;
    std::vector<float2> freeDirections;
    size_t weight = 0;
    float length = 0;
    float rad = 0;
  };

  struct Quality
  {
    float3 C;
    float3 mean;
    float3 D3;
    float D = 0;
  };

  struct TreeParams
  {
    float height = 0;
    float firstRad = 0;
    float lastRad = 0;
    float upCoef = 0;
    size_t branchCount = 0;
    size_t edgeBase = 0;
  };

  class GenerateObjectTree : public GenerateObject
  {
  public:
    GenerateObjectTree(const TreeParams& p) : GenerateObject(p.height), params(p) {}

    GenerateObjectTree() : GenerateObject(5.f)
    {
      params.height = 5.f;
      params.firstRad = 0.5;
      params.lastRad = 0.03;
      params.upCoef = 0.3;
      params.branchCount = 6;
      params.edgeBase = 5;
    }

    void GenerateMesh() override;

  private:
    void GenerateBranch(TreeBranch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut = float3());
    void RelaxBranch(TreeBranch& branch, size_t meshId);
    float3 GenOutVec(const float3& vecIn, int fromAngle, int toAngle) const;
    size_t SelectWeightedBranch();
    void InitBranch(const size_t parent_id, TreeBranch& branch, float3& point_start, float3& vec_in);
    void CalculateQuality();
    std::vector<TreeBranch> tree;
    Quality quality;
    TreeParams params;
  };
}