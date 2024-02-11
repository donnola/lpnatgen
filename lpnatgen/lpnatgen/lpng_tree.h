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
    size_t childsCount = 0;
    size_t weight = 0;
    float length = 0;
    float rad = 0;
  };

  struct Quality
  {
    float3 C;
    float3 mean;
    float3 D3;
    float D;
  };

  class GenerateObjectTree : public GenerateObject
  {
  public:
    GenerateObjectTree(float h = 5.f, float r = 0.5, size_t c = 6, size_t s = 5) : GenerateObject(h)
    {
      treeHeight = h;
      treeRad = r;
      branchCount = c;
      segNum = s;
    }

    void GenerateMesh() override;

  private:
    void GenerateBranch(TreeBranch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut = float3());
    void RelaxBranch(TreeBranch& branch, size_t meshId);
    float3 GenOutVec(const float3& vecIn, int angleDelta, int add = 0) const;
    size_t SelectWeightedBranch();
    void InitBranch(const size_t parent_id, TreeBranch& branch, float3& point_start, float3& vec_in);
    void CalculateQuality();
    std::vector<TreeBranch> tree;
    float treeRad;
    float treeHeight;
    size_t branchCount = 8;
    size_t segNum = 5;
    Quality quality;
  };
}