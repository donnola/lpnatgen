#pragma once

#include "lpnatgen.h"

namespace lpng
{
  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float3 size = float3(1.5, 2, 1.5), int pc = 25) : GenerateObject(size), pointsCount(pc) {}

    void GenerateMesh() override;
    void SetPointsCount(int c) { pointsCount = c; };
  private:
    size_t pointsCount;
  };
}