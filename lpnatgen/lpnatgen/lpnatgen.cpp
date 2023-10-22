#include "lpnatgen.h"

void lpng::GenerateObject::GenerateNormals()
{

}

void lpng::GenerateObject::Write() const
{

}

void lpng::GenerateObject::AddObject(Object& obj)
{
  model.push_back(std::move(obj));
}

void lpng::GenerateObjectTree::GenerateMesh()
{

}
