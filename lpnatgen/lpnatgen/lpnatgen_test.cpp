#define _USE_MATH_DEFINES
#include "lpnatgen.h"
#include "lpmath.h"

void lpng::GenerateObjectTest::GenerateMesh()
{
  Object cylinder;

  float3 root(0, 0, 0);
  float3 dir(0, 1, 0);
  float height = 2;
  float radius = 0.5;
  int segments_num = 6;
  float segment_angle = 2 * M_PI / segments_num;
  std::vector<Face> culinderBaseFaces;
  
  for (int i = 0; i < segments_num; ++i)
  {
    float3 vertex = root + float3(sin(-i * segment_angle), 0, cos(-i * segment_angle));
    cylinder.vertexCoords.push_back(vertex);
  }
  cylinder.vertexCoords.push_back(root);
  int rootId = cylinder.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    cylinder.faces.push_back(Face({ Vertex(i + 1), Vertex((i + 1) % 6 + 1), Vertex(rootId) }));
  }

  // here must be extrude, but it's not realized
  int culinderBaseFacesNum = cylinder.faces.size();
  for (int i = 0; i < rootId; ++i)
  {
    cylinder.vertexCoords.push_back(cylinder.vertexCoords[i] + dir * height);
  }
  for (int i = 0; i < culinderBaseFacesNum; ++i)
  {
    Face f_t = cylinder.faces[i];
    std::reverse(f_t.begin(), f_t.end());
    for (Vertex& v : f_t)
    {
      v.vi += rootId;
    }
    cylinder.faces.push_back(f_t);
    Face f_b = cylinder.faces[i];
    cylinder.faces.push_back(Face({ f_b[0], f_t[2], f_t[1],  f_b[1] }));
  }

  AddObject(cylinder);
}