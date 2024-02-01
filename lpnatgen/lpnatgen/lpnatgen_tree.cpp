#define _USE_MATH_DEFINES

#include "lpnatgen_tree.h"


void lpng::GenerateObjectTree::GenerateMesh()
{
  Mesh branches;
  GenerateBranch(mainBranch, {0, 1, 0}, { 0, 0, 0 });
  //while (tree.size() < branchCount)
  //{

  //}
  for (int i = 0; i < tree.size(); ++i)
  {
    RelaxBranch(tree[i], i);
  }
}


void lpng::GenerateObjectTree::GenerateBranch(TreeBranch& branch, const float3& vecIn, const float3& pointStart)
{
  Mesh mesh;
  mesh.matType = MaterialTypes::STONE;
  float seg_angle = 2 * M_PI / segNum;
  float last_seg_len = branch.length / 6;
  float last_rad = branch.rad / 5;
  // create first ring
  TreeRing ring;
  ring.center = pointStart;
  ring.vecIn = vecIn;
  ring.vecOut = vecIn;
  ring.rad = branch.rad;
  ring.curLength = 0;

  for (size_t i = 0; i < segNum; ++i)
  {
    float3 vertex = ring.center + float3(sin(i * seg_angle) * ring.rad, 0, cos(i * seg_angle) * ring.rad);
    mesh.vertexCoords.push_back(vertex);
  }
  mesh.vertexCoords.push_back(ring.center);
  int rootId = mesh.vertexCoords.size();
  for (int i = 0; i < rootId - 1; ++i)
  {
    mesh.faces.push_back(Face({ i + 1, (i + 1) % segNum + 1, rootId }));
    ring.facesIds.push_back(mesh.faces.size()-1);
  }
  branch.rings.push_back(std::move(ring));

  while (branch.rings.back().curLength < branch.length)
  {
    TreeRing& p_ring = branch.rings.back();
    //float3 prev_vec_in = mRing.vecIn;

    float seg_len = std::max((branch.length - p_ring.curLength) / 5 * 2, last_seg_len);
    ring.curLength = p_ring.curLength + seg_len;
    ring.rad = std::max((treeRad - last_rad) * (1 - std::min(ring.curLength / branch.length, 1.f)), last_rad);
    float3 vec(rand() % 101 - 50, 0, rand() % 101 - 50);
    Normalize(vec);
    if (vec == float3(0, 0, 0))
      vec.y = 1;
    else
    {
      int angle = rand() % 10 + 15;
      vec.y = 1 / tan(DEG2RAD(angle));
    }
    Normalize(vec);
    float angle = Angle(float3(0, 1, 0), p_ring.vecIn);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(float3(0, 1, 0), p_ring.vecIn + float3(0, 1, 0)), angle);
      vec = QuatTransformVec(q, vec);
    }

    p_ring.vecOut = vec;
    ring.vecIn = vec;
    ring.vecOut = vec;
    ring.center = p_ring.center + vec * seg_len;
    
    if (branch.rings.size() == 1)
      ring.facesIds = ExtrudeWithCap(mesh, p_ring.facesIds, ring.vecIn * seg_len);
    else
      ring.facesIds = Extrude(mesh, p_ring.facesIds, vec);
    p_ring.facesIds.clear();
    ring.vertexesIds = GetVertexesIds(mesh, ring.facesIds);
    ScaleVertexes(mesh, float3(ring.rad / p_ring.rad, 1.f, ring.rad / p_ring.rad), ring.vertexesIds);
    branch.rings.push_back(std::move(ring));
  }

  tree.push_back(branch);
  model.push_back(std::move(mesh));
}

void lpng::GenerateObjectTree::RelaxBranch(TreeBranch& branch, size_t meshId)
{
  for (size_t i = 1; i < branch.rings.size(); ++i)
  {
    TreeRing& ring = branch.rings[i];
    float angle = Angle(float3(0, 1, 0), ring.vecIn);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(float3(0, 1, 0), ring.vecIn), angle);
      RotateVertexes(model[meshId], ring.vertexesIds, q, ring.center);
    }
    angle = Angle(ring.vecIn, ring.vecOut);
    if (angle > FLT_EPSILON)
    {
      Quat q(Cross(ring.vecIn, ring.vecOut), angle);
      RotateVertexes(model[meshId], ring.vertexesIds, q, ring.center);
    }
  }
}