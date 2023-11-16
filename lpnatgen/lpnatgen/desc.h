#pragma once

#include "lpmath.h"

#include <vector>
#include <ostream>

namespace lpng
{
  struct Vertex
  {
    size_t vi;
    size_t vti;
    size_t vni;

    Vertex(size_t a, size_t b, size_t c) : vi(a), vti(b), vni(c) {}
    Vertex(size_t a, size_t b) : vi(a), vti(b), vni(-1) {}
    Vertex(size_t a) : vi(a), vti(-1), vni(-1) {}
  };

  using Face = std::vector<Vertex>;

  struct Object
  {
    std::vector<Face> faces;
    std::vector<float3> vertexCoords;
    std::vector<float2> vertexTextCoords;
    std::vector<float3> vertexNormals;
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
