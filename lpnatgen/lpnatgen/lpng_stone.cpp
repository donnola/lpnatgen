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
        v.y = quantile.y - n * 0.2;
    }
    quant_id /= 2;
    quantile = vertexes[quant_id];
  }
  float max_vertex = stone.vertexCoords[0].y;
  float min_vertex = stone.vertexCoords[0].y;
  for (const float3& v : stone.vertexCoords)
  {
    if (v.y < min_vertex)
      min_vertex = v.y;
    if (v.y > max_vertex)
      max_vertex = v.y;
  }
  MoveObj(stone, float3(0, -min_vertex * 0.6, 0));
  max_vertex -= min_vertex * 0.6;
  ScaleObj(stone, float3(1, objectSize.y / max_vertex, 1));
}


void lpng::GenerateObjectStone::GenerateMesh()
{
  if (pointsCount < 4)
  {
    return;
  }
  std::unordered_set<size_t> points;
  Mesh stone = Sphere::GetMeshWithMinSubdiv(pointsCount * 3);
  while (points.size() < pointsCount)
  {
    points.insert(fast_lpng_rand(0, stone.vertexCoords.size()));
  }
  
  FilterMeshWithPoints(stone, points);
  
  ScaleObj(stone, objectSize / (Sphere::size_coef * 2));
  
  stone.matType = MaterialTypes::STONE;
  ModifyStone(stone);
  model.push_back(std::move(stone));
}