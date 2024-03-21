#pragma once

#include "lpng.h"

namespace lpng
{
  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone(float3 size = float3(1.5, 2, 1.5), int pc = 30) : GenerateObject(size), pointsCount(pc) {}

    void GenerateMesh() override;
    void SetPointsCount(int c) { pointsCount = c; };
  private:
    void flatten_stone();
    size_t pointsCount;
  };
}