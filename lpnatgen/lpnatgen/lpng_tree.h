#pragma once

#include "lpng_plant.h"
#include <unordered_set>


namespace lpng
{
  struct Quality
  {
    float3 C;
    float3 mean;
    float3 D3;
    float D = 0;
  };

  struct TreeParams
  {
    float height = 5.f;
    float firstRad = 0.5;
    float finalRad = 0.03;
    float upCoef = 0.3;
    int edgeBase = 5;
    int branchMinCount = 5;
    int branchMaxCount = 7;
    int crownVertexNum = 50;
  };

  struct TreeRebuildParams
  {
    float2 disp = float2(3, 3.5);
    float balance = 0.1;
    float centered = 0.1;
    int rebuildNum = 0;
  };

  struct CrownCluster
  {
    std::unordered_set<int> branchIds;
    float3 center; 
    float rad = 0;
    float deltaRad = 0;
    int mainBranchId = 0;
    int pointsCount = 0;
  };

  class GenerateObjectTree : public GenerateObjectBaseTree
  {
  public:
    GenerateObjectTree() 
    {
      branchMinCoefStart = 0.25;
      branchMaxCoefStart = 0.8;
      branchMinCoefLen = 0.85;
      branchMaxCoefLen = 1.1;
      branchAngleMin = 40;
      branchAngleMax = 60;
    }
    void SetTreeParams(const TreeParams& p)
    {
      treeParams = p;
      upCoef = p.upCoef;
      branchBase = p.edgeBase;
      finalBranchRad = p.finalRad;
    }
    void SetRebuildParams(const TreeRebuildParams& p = TreeRebuildParams())
    {
      rebuildParams = p;
    }

  private:
    void GenerateMesh() override;
    void GenerateCrown() override;
    void ModifyCrown(Mesh& crown, const float3& c);
    void InitClusters();
    void CalculateQuality();
    void ClearTree();
    Quality quality;
    TreeParams treeParams;
    TreeRebuildParams rebuildParams;
    size_t buildId = 0;
    std::vector<CrownCluster> crownClusters;
  };
}