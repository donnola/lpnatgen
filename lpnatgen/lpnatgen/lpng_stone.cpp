#define _USE_MATH_DEFINES
#include "lpng_stone.h"
#include "lpng_primitive.h"
#include <unordered_set>
#include <algorithm>


void lpng::GenerateObjectStone::ModifyStone(Mesh& stone)
{
  std::vector<float3> vertexes = stone.vertexCoords;
  std::sort(vertexes.begin(), vertexes.end(), [](const float3& a, const float3& b) { return a.y < b.y; });
  int quant_id = vertexes.size() * 0.4 - 1;
  float3 quantile = vertexes[quant_id];
  while (quant_id > 1)
  {
    for (float3& v : stone.vertexCoords)
    {
      float n = quantile.y - v.y;
      if (n > 0)
        v.y = quantile.y - n * 0.5;
    }
    quant_id /= 2;
  }
  float3 max_vertex;
  float3 min_vertex;
  for (const float3& v : stone.vertexCoords)
  {
    if (v.y < min_vertex.y)
      min_vertex = v;
    if (v.y > max_vertex.y)
      max_vertex = v;
  }
  MoveObj(stone, float3(0, -min_vertex.y * 0.9, 0));
  ScaleObj(stone, float3(1, objectSize.y / (max_vertex.y - min_vertex.y), 1));
}


void lpng::GenerateObjectStone::GenerateMesh()
{
  std::unordered_set<size_t> points;
  Sphere* sphere = Sphere::GetInstance();
  if (sphere->GetVertexCount() < pointsCount * 3)
  {
    sphere->RaiseToMinSubdiv(pointsCount * 3);
  }
  while (points.size() < pointsCount)
  {
    points.insert(fast_lpng_rand(0, sphere->GetVertexCount()));
  }
  Mesh stone = GenerateMeshFromSphere(points);
  
  ScaleObj(stone, objectSize / (sphere->GetSizeCoef() * 2));
  
  stone.matType = MaterialTypes::STONE;
  ModifyStone(stone);
  model.push_back(std::move(stone));
}