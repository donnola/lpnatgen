#pragma once

#include "lpng.h"
#include <unordered_set>

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
    std::vector<int> childsIds;
    std::vector<float2> freeDirections;
    float length = 0;
    float rad = 0;
    int level = 0;
    size_t weight = 0;
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
    float height = 5.f;
    float firstRad = 0.5;
    float lastRad = 0;
    float upCoef = 0;
    float2 disp;
    float sqBalance = 0;
    float sqCentered = 0;
    size_t branchCount = 7;
    size_t edgeBase = 5;
    size_t rebuildNum = 0;
  };

  struct CrownCluster
  {
    std::unordered_set<int> branchIds;
    float3 center; 
    float rad = 0;
    float deltaRad = 0;
    int mainBranchId = 0;
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
      params.disp = float2(3, 3.4);
      params.sqBalance = 0.05;
      params.sqCentered = 0.1;
      params.branchCount = 7;
      params.edgeBase = 5;
      params.rebuildNum = 5;
    }

    void GenerateMesh() override;

  private:
    void GenerateBranch(TreeBranch& branch, const float3& pointStart, const float3& vecIn, const float3& vecOut = float3());
    void RelaxBranch(TreeBranch& branch, size_t meshId);
    float3 GenOutVec(const float3& vecIn, int fromAngle, int toAngle) const;
    float3 GenOutVec(std::vector<float2>& dirs, const float3& vecIn, int fromAngle, int toAngle) const;
    int SelectWeightedBranch();
    std::vector<float2> GetNDirections(int n);
    void InitBranch(const size_t parent_id, TreeBranch& branch, float3& point_start, float3& vec_in);
    void GenerateCrown();
    void InitClusters();
    void ModifyCrown(Mesh& crown, const float3& c);
    void RelaxCrown();
    void CalculateQuality();
    void ClearTree();
    std::vector<TreeBranch> tree;
    Quality quality;
    TreeParams params;
    size_t buildId = 0;
    std::vector<CrownCluster> crownClusters;
  };
}