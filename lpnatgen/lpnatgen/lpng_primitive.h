#pragma once
#include "lpng_math.h"
#include <unordered_set>
#include <mutex>


namespace lpng
{
  class Sphere
  {
  private:
    static Sphere* p_instance;
    Sphere();
    Sphere(const Sphere&) = default;
    ~Sphere() {}
    Sphere& operator=(Sphere&) = default;
    Mesh sphere;
    size_t vertex_num = 12;
    size_t smooth_level = 0;
    static std::mutex mutex_;
    void Subdiv();
  public:
    static void Destroy()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!p_instance)
      {
        delete p_instance;
        p_instance = nullptr;
      }
    }

    static Mesh GetSphere()
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!p_instance)
        p_instance = new Sphere();
      return p_instance->sphere;
    }

    static const float size_coef;

    static Mesh GetMeshWithMinSubdiv(size_t min_point_count)
    {
      std::lock_guard<std::mutex> lock(mutex_);
      if (!p_instance)
        p_instance = new Sphere();
      while (p_instance->vertex_num < min_point_count)
      {
        p_instance->Subdiv();
      }
      return p_instance->sphere;
    }
  };
}