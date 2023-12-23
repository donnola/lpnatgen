#pragma once
#include "lpmath.h"

namespace lpng
{

  class ConvexHull
  {
  public:
    ConvexHull(std::vector<float3> p) : points(std::move(p)) {};
    void GenerateMinConvexHull();
    void GenerateFullConvexHull();
    std::vector<Face> GetConvexHull() const;
  private:
    struct CHEdge
    {
      Edge edge;
      int faceId;
      bool is_close = false;
      CHEdge(const Edge& e, int face_id = -1) : edge(e), faceId(face_id) {}
    };
    void AddFirstFaceHull();
    int FindFirstPoint() const;
    int FindMaxPoint() const;
    int ReturnIsEdgeInHull(const Edge& e) const;
    void AddFace(const Face& f, const float3& n);
    void AddEdge(const Edge& e, int face_id);

    std::vector<float3> points;
    std::vector<Face> faces;
    std::vector<float3> normals;
    std::vector<CHEdge> edges;

  };
}
