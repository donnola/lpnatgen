#define _USE_MATH_DEFINES
#include "lpnatgen.h"
#include "lpmath.h"
#include <stack>
#include <set>


void lpng::GenerateObjectStone::GenerateMesh()
{
  Object stone;
  stone.matType = MaterialTypes::STONE;
  GenerateRandomConvexHull(stone);
  model.push_back(std::move(stone));
}

void lpng::GenerateObjectStone::GenerateUniformPoints(Object& obj, int pointsNum)
{
  float from = -objectSize * 0.5;
  float to = objectSize * 0.5;
  double radiusSq = to * to;
  double halfRadiusSq = to * to * 0.25;
  float eps = objectSize * 0.1;
  while (obj.vertexCoords.size() < pointsNum)
  {
    float x = from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (objectSize)));
    float y = from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (objectSize)));
    float z = from + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (objectSize)));
    float3 p(x, y, z);
    double vecLengthSq = MagnitudeSq(p);
    if (vecLengthSq <= radiusSq && vecLengthSq >= halfRadiusSq)
    {
      obj.vertexCoords.push_back(std::move(p));
    }
  }
  for (float3& v : obj.vertexCoords)
  {
    float len = (from - eps) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (objectSize + eps * 2)));
    Normalize(v);
    v *= len;
  }
}

void lpng::GenerateObjectStone::GenerateRandomConvexHull(Object& obj, const float3& size)
{
  GenerateUniformPoints(obj);
  std::stack<Edge> freeEdges;



  AddFreeVertexesToModel(obj);
  Scale(obj, size);
}

void lpng::GenerateObjectStone::AddFreeVertexesToModel(Object& obj)
{
  std::set<int> vertexIds;
  for (const Face& f : obj.faces)
  {
    for (const Vertex& v : f)
    {
      vertexIds.insert(v.vi - 1);
    }
  }
  for (int i = 0; i < obj.vertexCoords.size(); ++i)
  {
    if (vertexIds.find(i) == vertexIds.end())
    {
      double minDisToFace = FLT_MAX;
      int nearesFaceId = -1;
      for (int j = 0; j < obj.faces.size(); ++j)
      {
        double dist = DistFromPointToFace(obj, obj.faces[j], obj.vertexCoords[i]);
        if (dist > 0 && dist < minDisToFace)
        {
          minDisToFace = dist;
          nearesFaceId = j;
        }
      }
      SplitFaceMithPoint(obj, nearesFaceId, i);
    }
  }
}