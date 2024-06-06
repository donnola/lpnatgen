#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_bush.h"
#include "lpng_fir.h"
#include "lpng_rand.h"

static enum ObjectType
{
  TEST = 0,
  STONE = 1,
  TREE = 2,
  BUSH = 3,
  FIR = 4,
  MAX_TYPE = 5
};

static lpng::TreeParams treeParams;
static lpng::FirParams firParams;
static lpng::TreeRebuildParams treeRebuildParams;
static lpng::BushParams bushParams;
static int stoneVertexCount = 40;

static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);

int main(void)
{
  fast_lpng_rand(int(std::time(nullptr)));
  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  int model_type = TREE;
  for (int i = 0; i < 10000000; ++i)
  {
    SetModelParams(model_type, modelPtr);
    modelPtr->Generate();
  } 
}

static bool SetModelParams(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  bool res = false;
  lpng::float3 model_size;

  switch (type)
  {

  case STONE:
  {
    lpng::GenerateObjectStone* stone_ptr = new lpng::GenerateObjectStone();
    float x_k = float(fast_lpng_rand(1700, 2800)) / 1000.f;
    float y = float(fast_lpng_rand(200, 600)) / 1000.f;
    model_size = { y * x_k, y, y * x_k };
    stone_ptr->SetVertexCount(stoneVertexCount);
    model_ptr.reset(stone_ptr);
    res = true;
    break;
  }

  case BUSH:
  {
    lpng::GenerateObjectBush* bush_ptr = new lpng::GenerateObjectBush();
    bushParams.height = float(fast_lpng_rand(500, 1500)) / 1000.f;
    model_size.x = 2 * bushParams.firstRad;
    model_size.z = 2 * bushParams.firstRad;
    model_size.y = bushParams.height;
    bush_ptr->SetBushParams(bushParams);
    model_ptr.reset(bush_ptr);
    res = true;
    break;
  }

  case TREE:
  {
    lpng::GenerateObjectTree* tree_ptr = new lpng::GenerateObjectTree();
    treeParams.height = float(fast_lpng_rand(6000, 10000)) / 1000.f;
    treeParams.firstRad = float(fast_lpng_rand(250, 400)) / 1000.f;
    model_size.x = 2 * treeParams.firstRad;
    model_size.z = 2 * treeParams.firstRad;
    model_size.y = treeParams.height;
    tree_ptr->SetTreeParams(treeParams);
    model_ptr.reset(tree_ptr);
    res = true;
    break;
  }
  case FIR:
  {
    lpng::GenerateObjectFir* fir_ptr = new lpng::GenerateObjectFir();
    firParams.height = float(fast_lpng_rand(6000, 10000)) / 1000.f;
    firParams.firstRad = float(fast_lpng_rand(200, 500)) / 1000.f;
    firParams.baseCrownRadMinCoefStart = 5.f;
    firParams.baseCrownRadMaxCoefStart = 7.f;
    model_size.x = 3 * firParams.firstRad;
    model_size.z = 3 * firParams.firstRad;
    model_size.y = firParams.height * 1.5;
    fir_ptr->SetFirParams(firParams);
    model_ptr.reset(fir_ptr);
    res = true;
    break;
  }
  default:
    break;
  }
  if (res)
  {
    model_ptr->SetSize(model_size);
    model_ptr->SetModelSmoothness(false);
  }
  return res;
}