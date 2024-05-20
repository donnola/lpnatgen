#pragma once

#include "lpng.h"


namespace lpng
{
  class GenerateObjectStone : public GenerateObject
  {
  public:
    GenerateObjectStone() {}
    void SetVertexCount(size_t c) { pointsCount = c; };
  private:
    void GenerateMesh() override;
    void ModifyStone(Mesh& stone);
    size_t pointsCount = 40;
  };
}