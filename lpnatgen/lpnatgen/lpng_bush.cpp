#include "lpng_bush.h"
#include "lpng_primitive.h"


void lpng::GenerateObjectBush::GenerateMesh()
{
  int base_branch_count = fast_lpng_rand(bushParams.baseBranchMinCount, bushParams.baseBranchMaxCount + 1);
  std::vector<float2> mainDirections = GetNDirections(base_branch_count);
  while (mainDirections.size() > 0)
  {
    Branch branch;
    branch.weight = 10000;
    branch.baseRad = bushParams.firstRad;
    branch.lastRad = bushParams.finalRad;
    branch.freeDirections = GetNDirections(bushParams.edgeBase);
    branch.edgeBase = bushParams.edgeBase;
    float3 vec_in = GenOutVec(mainDirections, float3(0, 1, 0), baseBranchAngleMin, baseBranchAngleMax);
    branch.length = bushParams.height * (fast_lpng_rand(850, 1100) / (vec_in.y * 1000.f));
    float3 point_start = branch.length * vec_in / 3;
    point_start.x *= fast_lpng_rand(300, 1100) / 1000.f;
    point_start.z *= fast_lpng_rand(300, 1100) / 1000.f;
    point_start.y = 0.f;
    GenerateBranch(branch, point_start, { 0, 1, 0 }, vec_in);
  }
  int children_count = fast_lpng_rand(bushParams.childrenMinCount, bushParams.childrenMaxCount + 1);
  for (int i = 0; i < children_count; ++i)
  {
    int parent_id = SelectWeightedBranch();
    if (parent_id < 0)
      break;
    Branch branch;
    float3 point_start;
    float3 vec_in;
    InitBranch(parent_id, branch, point_start, vec_in);
    GenerateBranch(branch, point_start, vec_in, vec_in);
    branches[parent_id].childsIds.push_back(branches.size() - 1);
  }
  for (size_t i = 0; i < branches.size(); ++i)
  {
    TreeRoot(branches[i], i);
    RelaxBranch(branches[i], i);
  }
  GenerateCrown();
}


void lpng::GenerateObjectBush::GenerateCrown()
{
  float3 crown_center;
  crown_center.y = bushParams.height * (fast_lpng_rand(800, 900) / 1000.f);
  float min_r_sq = 0;
  float min_h = 0;
  for (const Branch& b : branches)
  {
    const float3& branch_p = b.rings.back().center;
    float r_sq = MagnitudeSq(float2(branch_p.x, branch_p.z) - float2(crown_center.x, crown_center.z));
    if (r_sq > min_r_sq)
      min_r_sq = r_sq;
    if (branch_p.y < crown_center.y)
    {
      float h = crown_center.y - branch_p.y;
      if (h > min_h)
        min_h = h;
    } 
  }
  float crown_rad = sqrt(min_r_sq);
  float3 crown_size;
  crown_size.x = crown_rad * (fast_lpng_rand(950, 1050) / 1000.f);
  crown_size.z = crown_rad * (fast_lpng_rand(950, 1050) / 1000.f);
  crown_size.y = std::max(min_h, (crown_size.x + crown_size.z) / 7.f);

  Sphere* sphere = Sphere::GetInstance();
  std::unordered_set<size_t> points;
  Mesh crown = sphere->GetSphere();
  while (points.size() < bushParams.crownVertexNum)
  {
    points.insert(fast_lpng_rand(0, sphere->GetVertexCount()));
  }
  FilterMeshWithPoints(crown, points);
  ScaleObj(crown, crown_size / sphere->GetSizeCoef());
  MoveObj(crown, crown_center);
  crown.matType = MaterialTypes::CROWN;
  ModifyCrown(crown, crown_center);
  model.push_back(std::move(crown));
}


void lpng::GenerateObjectBush::ModifyCrown(Mesh& crown, const float3& c)
{
  std::vector<float3> vertexes = crown.vertexCoords;
  std::sort(vertexes.begin(), vertexes.end(), [](const float3& a, const float3& b) { return a.y < b.y; });
  int quant_id = vertexes.size() * 0.6;
  float3 quantile = vertexes[quant_id];
  for (float3& v : crown.vertexCoords)
  {
    float k = fast_lpng_rand(1050, 1200) / 1000.f;;
    float n = v.y - quantile.y;
    if (n > 0)
    {
      float n_k = fast_lpng_rand(4500, 5500) / 1000.f;
      v.y = quantile.y + n * n_k;
    }
    else
    {
      float n_k = fast_lpng_rand(1200, 2200) / 1000.f;
      v.y = quantile.y + n * n_k;
    }
    v.x = c.x + (v.x - c.x) * k;
    v.z = c.z + (v.z - c.z) * k;
  }
}