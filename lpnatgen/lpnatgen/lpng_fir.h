#pragma once

#include "lpng_plant.h"


namespace lpng
{
  struct FirParams
  {
    float height = 5.f;
    float firstRad = 0.5;
    float finalRad = 0.03;
    float upCoef = 0.8;
    float crownMinCoefStart = 0.2;
    float crownMaxCoefStart = 0.35;
    float baseCrownRadMinCoefStart = 4.2;
    float baseCrownRadMaxCoefStart = 5.f;
    int edgeBase = 5;
    int crownBase = 5;
    int crownCountMin = 3;
    int crownCountMax = 4;
  };

  struct CrownRing
  {
    float3 vecIn;
    float3 vecOut;
    float3 center;
    float rad;
    float curLength;
    std::vector<int> facesIds;
    std::vector<int> verticesIds;
  };

  struct Crown
  {
    std::vector<CrownRing> rings;
    float length = 0;
    float baseRad = 0;
    float lastRad = 0.02;
    int edgeBase = 0;
  };

  class GenerateObjectFir : public GenerateObjectBaseTree
  {
  public:
    GenerateObjectFir() {}
    void SetFirParams(const FirParams& p)
    {
      firParams = p;
      upCoef = p.upCoef;
      branchBase = p.edgeBase;
      finalBranchRad = p.finalRad;
    }

  private:
    void GenerateMesh() override;
    void GenerateCrown() override;
    void GenerateCrownPart(float heightStart, float baseRad, float lastRad, float height);
    void RelaxCrown(const Crown& crown, size_t meshId);
    float3 FindRingCenter(float heightStart, int& ceilRingId);
    std::vector<Crown> crowns;
    FirParams firParams;
    int crownCount = 1;
  };
}