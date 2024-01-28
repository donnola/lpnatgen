#pragma once

#include "lpnatgen.h"

namespace lpng
{
  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest(float3 size = float3(1, 2, 1)) : GenerateObject(size) {}

    void GenerateMesh() override;
  };
}