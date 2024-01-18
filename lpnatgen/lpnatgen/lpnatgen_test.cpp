#define _USE_MATH_DEFINES
#include "lpnatgen.h"
#include "lpmath.h"


void lpng::GenerateObjectTest::GenerateMesh()
{
  Mesh cylinder;

  float3 root(0, 0, 0);
  float3 dir(0, 1, 0);
  float height = objectSize.y;
  float radius_x = objectSize.x * 0.5;
  float radius_z = objectSize.z * 0.5;
  int segments_num = 6;
  float segment_angle = 2 * M_PI / segments_num;
  std::vector<Face> culinderBaseFaces;
  
  for (int i = 0; i < segments_num; ++i)
  {
    float3 vertex = root + float3(sin(i * segment_angle) * radius_x, 0, cos(i * segment_angle) * radius_z);
    cylinder.vertexCoords.push_back(vertex);
  }
  cylinder.vertexCoords.push_back(root);
  int rootId = cylinder.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    cylinder.faces.push_back(Face({ i + 1, (i + 1) % 6 + 1, rootId }));
  }

  ExtrudeWithCap(cylinder, { 0, 1, 2, 3, 4, 5 }, dir * height);

  model.push_back(std::move(cylinder));
}