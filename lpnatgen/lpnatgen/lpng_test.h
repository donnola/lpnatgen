#pragma once

#include "lpng.h"

namespace lpng
{
  class GenerateObjectTest : public GenerateObject
  {
  public:
    GenerateObjectTest() {}

    void GenerateMesh() override;
  };
}