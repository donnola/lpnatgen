#define _USE_MATH_DEFINES

#include "lpnatgen_tree.h"


void lpng::GenerateObjectTree::GenerateMesh()
{
  Mesh stone;

  stone.matType = MaterialTypes::STONE;
  model.push_back(std::move(stone));
}