#pragma once

#include "lpnatgen.h"

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
    size_t weight;
    float length;
    float rad;
  };

  class GenerateObjectTree : public GenerateObject
  {
  public:
    GenerateObjectTree(float h = 5.f, float r = 0.5, size_t c = 6, size_t s = 5) : GenerateObject(h)
    {
      treeHeight = h;
      treeRad = r;
      branchCount = c;
      mainBranch.weight = int(pow(2, branchCount) / 2);
      mainBranch.length = h;
      mainBranch.rad = r;
      segNum = s;
    }

    void GenerateMesh() override;

  private:
    void GenerateBranch(TreeBranch& branch, const float3& vecIn, const float3& pointStart);
    void RelaxBranch(TreeBranch& branch, size_t meshId);
    std::vector<TreeBranch> tree;
    TreeBranch mainBranch;
    float treeRad;
    float treeHeight;
    size_t branchCount = 8;
    size_t segNum = 5;
  };
}