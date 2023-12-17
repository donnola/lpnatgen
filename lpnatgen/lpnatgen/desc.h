#pragma once

#include <vector>
#include <ostream>


namespace lpng
{
  struct float2
  {
    float x;
    float y;

    float2(float a, float b) : x(a), y(b) {}
  };

  struct float3
  {
    float x;
    float y;
    float z;

    float3() : x(0), y(0), z(0) {}
    float3(float a, float b, float c) : x(a), y(b), z(c) {}

    float3& operator+=(const float3& r);
    float3& operator-=(const float3& r);
    float3& operator*=(const float3& r);
    float3& operator/=(const float3& r);
    float3& operator*=(double r);
    float3& operator/=(double r);
  };

  double Magnitude(const float3& vec);

  double MagnitudeSq(const float3& vec);

  void Normalize(float3& vec);

  float3 Normalized(const float3& vec);

  float3 operator+(const float3& l, const float3& r);

  float3 operator-(const float3& l, const float3& r);

  float3 operator*(const float3& l, const float3& r);

  float3 operator/(const float3& l, const float3& r);

  float3 operator*(const float3& l, double r);

  float3 operator*(double l, const float3& r);

  float3 operator/(const float3& l, double r);

  bool operator==(const float3& l, const float3& r);

  bool operator!=(const float3& l, const float3& r);

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
  using Edge = std::pair<int, int>;

  bool operator==(const Face& l, const Face& r);
  bool operator==(const Edge& l, const Edge& r);
  bool IsEdgeInFace(const Edge& edge, const Face& face);

  enum class ObjectTypes
  {
    NONE = -1,
    TREE = 0,
    BUSH = 1,
    STONE = 2
  };

  enum class MaterialTypes
  {
    NONE,
    WOOD,
    GRASS,
    STONE
  };

  struct Object
  {
    std::vector<Face> faces;
    std::vector<float3> vertexCoords;
    std::vector<float2> vertexTextCoords;
    std::vector<float3> vertexNormals;

    MaterialTypes matType = MaterialTypes::NONE;
    float3 pivot = float3(0,0,0);
  };

  std::ostream& operator<<(std::ostream& out, const float3& v);

  std::ostream& operator<<(std::ostream& out, const float2& v);

  std::ostream& operator<<(std::ostream& out, const Vertex& v);

  std::ostream& operator<<(std::ostream& out, const Face& v);
}
