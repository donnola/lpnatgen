#include "lpng_bush.h"

void lpng::GenerateObjectBush::GenerateMesh()
{
  int base_branch_count = fast_lpng_rand(bushParams.baseBranchMinCount, bushParams.baseBranchMaxCount + 1);
  std::vector<float2> mainDirections = GetNDirections(base_branch_count);
  while (mainDirections.size() > 0)
  {
    Branch branch;
    branch.weight = 10000;
    branch.baseRad = bushParams.firstRad;
    branch.lastRad = std::min(branch.baseRad / 2, bushParams.finalRad);
    branch.freeDirections = GetNDirections(bushParams.edgeBase);
    branch.edgeBase = bushParams.edgeBase;
    float3 vec_in = GenOutVec(mainDirections, float3(0, 1, 0), 30, 50);
    branch.length = bushParams.height * fast_lpng_rand(850, 1100) / (vec_in.y * 1000.f);
    float3 point_start = -branch.length * vec_in / 3;
    point_start.x *= fast_lpng_rand(-1100, 1100) / 1000.f;
    point_start.z *= fast_lpng_rand(-1100, 1100) / 1000.f;
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
    RelaxBranch(branches[i], i);
  }
  GenerateCrown();
}


void lpng::GenerateObjectBush::GenerateCrown()
{
  Mesh crown;
  float3 center;
  ModifyCrown(crown, center);
  model.push_back(std::move(crown));
}


void lpng::GenerateObjectBush::ModifyCrown(Mesh& crown, const float3& c)
{

}