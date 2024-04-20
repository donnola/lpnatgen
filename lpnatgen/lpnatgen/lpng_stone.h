#pragma once

#include "lpng.h"

namespace lpng
{
  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone() {}
    void GenerateMesh() override;
    void SetVertexCount(size_t c) { pointsCount = c; };
  private:
    void ModifyStone(Mesh& stone);
    size_t pointsCount = 30;
  };
}