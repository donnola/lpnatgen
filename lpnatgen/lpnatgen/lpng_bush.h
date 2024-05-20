#pragma once

#include "lpng_plant.h"


namespace lpng
{
  struct BushParams
  {
    float height = 1.f;
    float firstRad = 0.1;
    float finalRad = 0.03;
    float upCoef = 0.3;
    int edgeBase = 5;
    int baseBranchMinCount = 3;
    int baseBranchMaxCount = 5;
    int childrenMinCount = 5;
    int childrenMaxCount = 8;
    int crownVertexNum = 45;
  };

  class GenerateObjectBush : public GenerateObjectBaseTree
  {
  public:
    GenerateObjectBush() 
    {
      branchMinCoefStart = 0.2;
      branchMaxCoefStart = 0.7;
      branchMinCoefLen = 0.85;
      branchMaxCoefLen = 1;
      branchAngleMin = 30;
      branchAngleMax = 45;
    }
    void SetBushParams(const BushParams& p)
    {
      bushParams = p;
      upCoef = p.upCoef;
      branchBase = p.edgeBase;
      finalBranchRad = p.finalRad;
    }

  private:
    void GenerateMesh() override;
    void GenerateCrown() override;
    void ModifyCrown(Mesh& crown, const float3& c);
    BushParams bushParams;
    int baseBranchAngleMin = 15;
    int baseBranchAngleMax = 45;
  };
}