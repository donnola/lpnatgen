#pragma once
#include "lpng_math.h"
#include <set>


namespace lpng
{
  class Sphere
  {
  private:
    static Sphere* p_instance;
    Sphere();
    Sphere(const Sphere&) = default;
    Sphere& operator=(Sphere&) = default;
    Mesh sphere;
    size_t vertex_num = 12;
    const float size_coef = 10;
    size_t smooth_level = 0;
  public:
    ~Sphere() { p_instance = nullptr; }

    static Sphere* GetInstance()
    {
      if (!p_instance)
        p_instance = new Sphere();
      return p_instance;
    }

    static bool IsInstanced()
    {
      return !!p_instance;
    }

    static void Destroy()
    {
      if (!p_instance)
        return;
      delete p_instance;
    }

    const Mesh& GetSphere() const
    {
      return sphere;
    }

    size_t GetVertexCount() const
    {
      return vertex_num;
    }

    float GetSizeCoef() const
    {
      return size_coef;
    }

    void RaiseToMinSmoothness(int min_point_count);
    void RaiseSmoothness();
  };

  Mesh GenerateMeshFromSphere(const std::set<size_t>& points_ids);
}