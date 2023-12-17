#define _USE_MATH_DEFINES
#include "lpnatgen.h"
#include "lpmath.h"


void lpng::GenerateObjectStone::GenerateMesh()
{
  Object stone;
  stone.matType = MaterialTypes::STONE;
  std::vector<float3> points = GenerateEllipsoidUniformPoints(objectSize, 13);
  // add nois
  float3 eps = objectSize * 0.05;
  for (float3& v : points)
  {
    float x = -eps.x + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (eps.x * 2)));
    float y = -eps.y + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (eps.y * 2)));
    float z = -eps.z + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (eps.z * 2)));
    v += float3(x, y, z);
  }
  std::vector<Face> faces = GenerateConvexHullFull(points);
  stone.vertexCoords = points;
  stone.faces = faces;
  model.push_back(std::move(stone));
}