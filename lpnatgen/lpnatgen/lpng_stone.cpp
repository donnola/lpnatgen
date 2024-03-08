#define _USE_MATH_DEFINES
#include "lpng_stone.h"
#include "lpng_primitive.h"
#include <set>


void lpng::GenerateObjectStone::GenerateMesh()
{
  std::set<size_t> points;
  Sphere* s = Sphere::GetInstance();
  if (s->GetVertexCount() < pointsCount * 3)
  {
    s->RaiseToMinSmoothness(pointsCount * 3);
  }
  while (points.size() < pointsCount)
  {
    points.insert(fast_lpng_rand(0, s->GetVertexCount()));
  }
  Mesh stone = GenerateMeshFromSphere(points);
  ScaleLocalCoord(stone, objectSize / s->GetSizeCoef());
  stone.matType = MaterialTypes::STONE;
  model.push_back(std::move(stone));
}