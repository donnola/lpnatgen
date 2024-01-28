#pragma once

#include "lpnatgen.h"

namespace lpng
{
  struct TreeRing
  {
    float3 vecIn;
    float3 vecOut;
    float rad;
    std::vector<int> facesIds;
    std::vector<int> pointsIds;
  };

  struct TreeBranch
  {
    std::vector<TreeRing> rings;
    std::vector<TreeBranch*> childs;
    size_t childsCount;
    float3 weight;
  };

  class GenerateObjectTree : public GenerateObject
  {
  public:
    GenerateObjectTree(float3 size = float3(0.35, 2, 0.35)) : GenerateObject(size) {}

    void GenerateMesh() override;

  private:
    std::vector<TreeBranch> tree;
    TreeBranch mainBranch;
  };
}