#pragma once
#include <cmath>
#include <vector>


namespace lpng
{
#define CMP(x, y)  (fabs(x - y) <= FLT_EPSILON * fmax(1.0, fmax(fabs(x), fabs(y))))
#define ABSOLUTE_CMP(x, y) (fabs(x - y) <= FLT_EPSILON)
#define RELATIVE_CMP(x, y) (fabs(x - y) <= FLT_EPSILON * Max(fabs(x), fabs(y)))
#define RAD2DEG(x) ((x) * 57.295754f)
#define DEG2RAD(x) ((x) * 0.0174533f)

  struct float2
  {
    float x;
    float y;

    float2(float a, float b) : x(a), y(b) {}

    float2& operator+=(const float2& r);
    float2& operator-=(const float2& r);
    float2& operator*=(const float2& r);
    float2& operator/=(const float2& r);
    float2& operator*=(double r);
    float2& operator/=(double r);
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

  struct Quat
  {
    float x, y, z, w;
    // d - rad
    Quat(float a, float b, float c, float d);
    Quat(float3 vec, float d);
    Quat() 
    {
      x = 0; y = 0; z = 0; w = 0;
    }
  };

  void Invert(Quat& q);
  Quat Inverted(const Quat& q);
  Quat operator*(const Quat& l, const float3& r);
  Quat operator*(const Quat& l, const Quat& r);
  float3 QuatTransformVec(const Quat& l, const float3& r);

  double Magnitude(const float2& vec);
  double MagnitudeSq(const float2& vec);
  double Magnitude(const float3& vec);
  double MagnitudeSq(const float3& vec);
  void Normalize(float2& vec);
  float2 Normalized(const float2& vec);
  void Normalize(float3& vec);
  float3 Normalized(const float3& vec);
  float2 operator+(const float2& l, const float2& r);
  float2 operator-(const float2& l, const float2& r);
  float2 operator*(const float2& l, const float2& r);
  float2 operator/(const float2& l, const float2& r);
  float2 operator*(const float2& l, double r);
  float2 operator*(double l, const float2& r);
  float2 operator/(const float2& l, double r);
  bool operator==(const float2& l, const float2& r);
  bool operator!=(const float2& l, const float2& r);
  float3 operator+(const float3& l, const float3& r);
  float3 operator-(const float3& l, const float3& r);
  float3 operator*(const float3& l, const float3& r);
  float3 operator/(const float3& l, const float3& r);
  float3 operator*(const float3& l, double r);
  float3 operator*(double l, const float3& r);
  float3 operator/(const float3& l, double r);
  bool operator==(const float3& l, const float3& r);
  bool operator!=(const float3& l, const float3& r);
  inline double Dot(const float2& l, const float2& r);
  inline double Dot(const float3& l, const float3& r);
  float3 Cross(const float3& l, const float3& r);
  double Angle(const float2& l, const float2& r);
  double Angle(const float3& l, const float3& r);
  double Distance(const float2& l, const float2& r);
  double Distance(const float3& l, const float3& r);
  float3 Project(const float3& length, const float3& direction);
  float3 Perpendicular(const float3& len, const float3& dir);
  float3 Reflection(const float3& vec, const float3& normal);

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
  int TakeThirdPointFromTriangle(const Face& face, const Edge& edge);
  bool IsPointInTriangle(const float3& point, const float3& a, const float3& b, const float3& c);

  enum class MaterialTypes
  {
    NONE,
    WOOD,
    GRASS,
    STONE
  };

  struct Mesh
  {
    std::vector<Face> faces;
    std::vector<float3> vertexCoords;
    std::vector<float2> vertexTextCoords;
    std::vector<float3> vertexNormals;

    MaterialTypes matType = MaterialTypes::NONE;
    float3 pivot = float3(0, 0, 0);
  };

  std::vector<int> GetVertexesIds(const Mesh& mesh, const std::vector<int>& facesIds);
  void ScaleWorldCoord(Mesh& mesh, const float3& vec);
  void ScaleLocalCoord(Mesh& mesh, const float3& vec);
  void ScaleVertexes(Mesh& mesh, const float3& vec, const std::vector<int>& vertexesIds);
  void ScaleVertexes(Mesh& mesh, const float3& vec, const std::vector<int>& vertexesIds, const float3& O);
  void ScaleFaces(Mesh& mesh, const float3& vec, const std::vector<int>& facesIds);
  void ScaleFaces(Mesh& mesh, const float3& vec, const std::vector<int>& facesIds, const float3& O);
  void RotateFaces(Mesh& mesh, const std::vector<int>& facesIds, const Quat& quat, const float3& O);
  void RotateVertexes(Mesh& mesh, const std::vector<int>& vertexesIds, const Quat& quat, const float3& O);

  void MoveFaces(Mesh& mesh, const std::vector<int>& facesIds, const float3& vector);
  void MovePivot(Mesh& mesh, const float3& vec);
  void MoveObj(Mesh& mesh, const float3& vec);
  void DecomposeObj(Mesh& mesh);
  std::vector<float3> CalculateObjNormals(const Mesh& mesh);
  void SplitFaceMithPoint(std::vector<Face>& faces, const int faceId, const int pointId);
  double DistFromPointToFace(const float3& point, const float3& a, const float3& b, const float3& c);
  
  void SetPeripheryEdges(
    const Mesh& mesh, const std::vector<int>& facesIds, 
    std::vector<Edge>& peripheryEdges
  );
  std::vector<int> ExtrudeWithCap(Mesh& mesh, const std::vector<int>& facesIds, const float3& vec);
  std::vector<int> Extrude(Mesh& mesh, const std::vector<int>& facesIds, const float3& vec);

 
  std::vector<float3> GenerateEllipsoidUniformPoints(const float3& size, int pointsNum = 10);
  void FilterNearesPoints(std::vector<float3>& points, float d = 0.07);
  void FixMeshHole(Mesh& mesh);
  void FixMeshOrientation(Mesh& mesh);
}
