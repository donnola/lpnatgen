#define _USE_MATH_DEFINES
#include "lpng_stone.h"
#include "cgal_math.h"


void lpng::GenerateObjectStone::GenerateMesh()
{
  std::vector<float3> points = GenerateEllipsoidUniformPoints(objectSize, pointsCount);
  Mesh stone = std::move(BuildMeshWithPoints(points));
  stone.matType = MaterialTypes::STONE;
  model.push_back(std::move(stone));
}