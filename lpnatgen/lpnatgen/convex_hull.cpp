#include "convex_hull.h"

#include <set>
#include <stack>


std::vector<lpng::Face> lpng::ConvexHull::GetConvexHull() const
{
  return faces;
}


int lpng::ConvexHull::FindFirstPoint() const
{
  int min_id = 0;
  for (int i = 1; i < points.size(); ++i)
  {
    if (points[i].y < points[min_id].y ||
        (points[i].y == points[min_id].y && points[i].x < points[min_id].x) ||
        (points[i].y == points[min_id].y && points[i].x == points[min_id].x &&
         points[i].z < points[min_id].z))
    {
      min_id = i;
    }
  }
  return min_id;
}

int lpng::ConvexHull::FindMaxPoint() const
{
  int max_id = 0;
  for (int i = 1; i < points.size(); ++i)
  {
    if (points[i].y > points[max_id].y ||
      (points[i].y == points[max_id].y && points[i].x > points[max_id].x) ||
      (points[i].y == points[max_id].y && points[i].x == points[max_id].x &&
        points[i].z > points[max_id].z))
    {
      max_id = i;
    }
  }
  return max_id;
}


int lpng::ConvexHull::ReturnIsEdgeInHull(const Edge& e) const
{
  for (int i = 0; i < edges.size(); ++i)
  {
    if (edges[i].edge == e)
    {
      return i;
    }
  }
  return -1;
}


void lpng::ConvexHull::AddFace(const Face& f, const float3& n)
{
  faces.push_back(f);
  normals.push_back(n);
}


void lpng::ConvexHull::AddEdge(const Edge& e, int face_id)
{
  edges.emplace_back(e, face_id);
}


void lpng::ConvexHull::AddFirstFaceHull()
{
  int first_point, second_point, third_point;
  first_point = FindFirstPoint();
  int max_point = FindMaxPoint();
  float3 start = points[first_point];
  double min_angle = 7;
  int min_id = -1;
  for (int i = 0; i < points.size(); ++i)
  {
    if (first_point == i)
    {
      continue;
    }
    float3 next = points[i];
    double angle = Angle(next - start, float3(next.x, start.y, next.z) - start);
    if (min_angle > angle)
    {
      min_angle = angle;
      min_id = i;
    }
  }
  second_point = min_id;

  min_angle = 7;
  min_id = -1;
  for (int i = 0; i < points.size(); ++i)
  {
    if (first_point == i || second_point == i)
    {
      continue;
    }
    float3 normal = Cross(points[second_point] - points[first_point], points[i] - points[first_point]);
    double angle = Angle(float3(0, 1, 0), normal);
    if (min_angle > angle)
    {
      min_angle = angle;
      min_id = i;
    }
  }
  third_point = min_id;
  float3 normal = Cross(points[second_point] - points[first_point], points[third_point] - points[first_point]);
  float3 mean_point = (points[first_point] + points[second_point] + points[third_point]) / 3;
  if (Dot(points[max_point] - mean_point, normal) >= 0)
  {
    std::swap(second_point, third_point);
    normal *= -1;
  }
  AddFace(Face({ first_point + 1, second_point + 1, third_point + 1 }), Normalized(normal));
  AddEdge(Edge(first_point + 1, second_point + 1), 0);
  AddEdge(Edge(second_point + 1, third_point + 1), 0);
  AddEdge(Edge(third_point + 1, first_point + 1), 0);
}


void lpng::ConvexHull::GenerateMinConvexHull()
{
  AddFirstFaceHull();
  std::stack<int> stack;
  stack.push(0);
  stack.push(1);
  stack.push(2);
  while (!stack.empty())
  {
    float3 new_normal;
    CHEdge e = edges[stack.top()];
    stack.pop();
    if (e.is_close)
    {
      continue;
    }
    int min_id = -1;
    double min_angle = 7;

    for (int i = 0; i < points.size(); ++i)
    {
      int another = TakeThirdPointFromTriangle(faces[e.faceId], e.edge);
      another -= 1;
      if (i != another && e.edge.first != i && e.edge.second != i)
      {
        float3 current_normal = Cross(points[e.edge.first - 1] - points[e.edge.second - 1], points[i] - points[e.edge.second - 1]);
        Normalize(current_normal);
        double angle = Angle(current_normal, normals[e.faceId]);

        if (min_angle > angle)
        {
          min_angle = angle;
          min_id = i;
          new_normal = current_normal;
        }
      }
    }
    if (min_id != -1)
    {
      e.is_close = true;
      int count_flatness = faces.size();
      int first_edge_in_hull = ReturnIsEdgeInHull({ e.edge.first, min_id + 1 });
      int second_edge_in_hull = ReturnIsEdgeInHull({ e.edge.second, min_id + 1 });

      if (first_edge_in_hull == -1)
      {
        AddEdge({ e.edge.first, min_id + 1 }, count_flatness);
        stack.push(edges.size() - 1);
      }
      if (second_edge_in_hull == -1)
      {
        AddEdge({ min_id + 1, e.edge.second }, count_flatness);
        stack.push(edges.size() - 1);
      }
      if (first_edge_in_hull != -1)
      {
        edges[first_edge_in_hull].is_close = true;
      }
      if (second_edge_in_hull != -1)
      {
        edges[second_edge_in_hull].is_close = true;
      }

      AddFace({ e.edge.first, min_id + 1, e.edge.second }, new_normal);
    }
  }
}


void lpng::ConvexHull::GenerateFullConvexHull()
{
  GenerateMinConvexHull();
  std::set<int> vertexIds;
  for (const Face& f : faces)
  {
    for (const Vertex& v : f)
    {
      vertexIds.insert(v.vi - 1);
    }
  }
  for (int i = 0; i < points.size(); ++i)
  {
    if (vertexIds.find(i) == vertexIds.end())
    {
      double minDisToFace = FLT_MAX;
      int nearesFaceId = -1;
      for (int j = 0; j < faces.size(); ++j)
      {
        float3 a = points[faces[j][0].vi - 1];
        float3 b = points[faces[j][1].vi - 1];
        float3 c = points[faces[j][2].vi - 1];
        double dist = DistFromPointToFace(points[i], a, b, c);
        if (dist < minDisToFace)
        {
          float3 n = Cross(a - b, c - b);
          Normalize(n);
          float3 proj = points[i] - dist * n;
          bool is_point_in_triangle = IsPointInTriangle(proj, a, b, c);
          bool is_point_in_front = Dot(points[i] - proj, Cross(b - a, c - a)) >= 0;
          if (is_point_in_triangle && is_point_in_front)
          {
            minDisToFace = dist;
            nearesFaceId = j;
          }
        }
      }
      if (nearesFaceId >= 0)
        SplitFaceMithPoint(faces, nearesFaceId, i);
    }
  }
}