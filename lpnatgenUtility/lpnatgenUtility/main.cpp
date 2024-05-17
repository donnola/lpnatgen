#define _CRT_SECURE_NO_WARNINGS
#include "lpng_tree.h"
#include "lpng_test.h"
#include "lpng_stone.h"
#include "lpng_bush.h"
#include "lpng_rand.h"
#include <iostream>
#include <stdlib.h>
#include <memory>
#include <ctime>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#undef RAYGUI_IMPLEMENTATION


static enum class InputBoxType
{
  NONE = -1,
  TEXT,
  VALUE_INT,
  VALUE_FLOAT
};

static struct InputBoxDesc
{
  Rectangle input_box_rect;
  Rectangle name_rect;
  std::string box_name = "";
  char text[256] = "";
  GuiState state = STATE_NORMAL;
  InputBoxType type = InputBoxType::NONE;
  int* value_int = nullptr;
  float* value_float = nullptr;
};

static enum ObjectType
{
  TEST = 0,
  STONE = 1,
  TREE = 2,
  BUSH = 3,
};

static lpng::TreeParams treeParams;
static lpng::TreeRebuildParams treeRebuildParams;
static lpng::BushParams bushParams;
static lpng::float3 objectSize(1, 2, 1);
static bool treeRebuildCheck = false;
static int stoneVertexCount = 40;
static bool goalModelSeedCheck = false;
static int goalModelSeed = 0;
static bool modelSmoothness = false;
static char* selectedInputBox = nullptr;

static Texture2D modelBaseTex;
static Texture2D modelWoodTex;
static Texture2D modelCrownTex;
static Texture2D modelStoneTex;

static Vector2 mousePoint = { 0.0f, 0.0f };

static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model);
static Mesh GenMesh(const lpng::Mesh& model);
static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static void InputBox(InputBoxDesc& inputBox);


int main(void)
{
  const int screenWidth = 1400;
  const int screenHeight = 800;

  InitWindow(screenWidth, screenHeight, "lpngUtility : object generation");
  SetTargetFPS(60);
  DisableCursor();
  fast_lpng_rand(int(std::time(0)));

  RenderTexture screen1 = LoadRenderTexture(screenWidth / 2, screenHeight);

  Camera camera = { 0 };
  camera.fovy = 45.0f;
  camera.up.y = 1.0f;
  camera.target.y = 0.0f;
  camera.position.x = 15.0f;
  camera.position.y = 15.0f;

  RenderTexture screen2 = LoadRenderTexture(screenWidth / 2, screenHeight);
  Rectangle splitScreenRect = { 0.0f, 0.0f, (float)screen1.texture.width, (float)-screen1.texture.height };
  Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };

  // BUTTONS

  Rectangle btnCopySeedBounds = { 320, 705, 140, 30 };
  std::string copySeedText = "Copy seed";

  Rectangle btnCreateBounds = { 12, 140, 140, 30 };
  std::string createModelText = "Create model";

  Rectangle btnSaveBounds = { 12, 220, 140, 30 };
  std::string saveModelText = "Save model";

  // INPUT BOXES

  Rectangle checkBoxSmoothnessRectangle = { 12, 60, 30, 30 };

  Rectangle checkBoxSeedRectangle = { 12, 100, 30, 30 };
  InputBoxDesc inputBoxSeed;
  inputBoxSeed.type = InputBoxType::VALUE_INT;
  inputBoxSeed.input_box_rect = Rectangle{ 120, 100, 180, 30 };
  inputBoxSeed.value_int = &goalModelSeed;

  InputBoxDesc inputBoxModelName;
  inputBoxModelName.type = InputBoxType::TEXT;
  inputBoxModelName.name_rect = Rectangle{ 12, 180, 100, 30 };
  inputBoxModelName.input_box_rect = Rectangle{ 120, 180, 180, 30 };
  inputBoxModelName.box_name = "Model name";

  InputBoxDesc inputSizeX;
  InputBoxDesc inputSizeY;
  InputBoxDesc inputSizeZ;
  snprintf(inputSizeX.text, sizeof inputSizeX.text, "%1.2f", objectSize.x);
  snprintf(inputSizeY.text, sizeof inputSizeY.text, "%1.2f", objectSize.y);
  snprintf(inputSizeZ.text, sizeof inputSizeZ.text, "%1.2f", objectSize.z);
  inputSizeX.type = InputBoxType::VALUE_FLOAT;
  inputSizeY.type = InputBoxType::VALUE_FLOAT;
  inputSizeZ.type = InputBoxType::VALUE_FLOAT;
  inputSizeX.name_rect = Rectangle{ 405, 20, 10, 30 };
  inputSizeY.name_rect = Rectangle{ 495, 20, 10, 30 };
  inputSizeZ.name_rect = Rectangle{ 585, 20, 10, 30 };
  inputSizeX.input_box_rect = Rectangle{ 420, 20, 70, 30 };
  inputSizeY.input_box_rect = Rectangle{ 510, 20, 70, 30 };
  inputSizeZ.input_box_rect = Rectangle{ 600, 20, 70, 30 };
  inputSizeX.box_name = "x";
  inputSizeY.box_name = "y";
  inputSizeZ.box_name = "z";
  inputSizeX.value_float = &objectSize.x;
  inputSizeY.value_float = &objectSize.y;
  inputSizeZ.value_float = &objectSize.z;

  // STONE

  InputBoxDesc inputStoneVertexCount;
  inputStoneVertexCount.type = InputBoxType::VALUE_INT;
  inputStoneVertexCount.name_rect = Rectangle{ 320, 60, 80, 30 };
  inputStoneVertexCount.input_box_rect = Rectangle{ 480, 60, 70, 30 };
  inputStoneVertexCount.box_name = "Stone vertex count";
  inputStoneVertexCount.value_int = &stoneVertexCount;

  // TREE

  InputBoxDesc inputTreeHeight;
  inputTreeHeight.type = InputBoxType::VALUE_FLOAT;
  inputTreeHeight.name_rect = Rectangle{ 320, 20, 70, 30 };
  inputTreeHeight.input_box_rect = Rectangle{ 480, 20, 70, 30 };
  inputTreeHeight.box_name = "Height";
  inputTreeHeight.value_float = &treeParams.height;
  snprintf(inputTreeHeight.text, sizeof inputTreeHeight.text, "%1.2f", treeParams.height);

  InputBoxDesc inputTreeFirstRad;
  inputTreeFirstRad.type = InputBoxType::VALUE_FLOAT;
  inputTreeFirstRad.name_rect = Rectangle{ 320, 60, 80, 30 };
  inputTreeFirstRad.input_box_rect = Rectangle{ 480, 60, 70, 30 };
  inputTreeFirstRad.box_name = "Base radius";
  inputTreeFirstRad.value_float = &treeParams.firstRad;
  snprintf(inputTreeFirstRad.text, sizeof inputTreeFirstRad.text, "%1.2f", treeParams.firstRad);

  InputBoxDesc inputTreeLastRad;
  inputTreeLastRad.type = InputBoxType::VALUE_FLOAT;
  inputTreeLastRad.name_rect = Rectangle{ 320, 100, 80, 30 };
  inputTreeLastRad.input_box_rect = Rectangle{ 480, 100, 70, 30 };
  inputTreeLastRad.box_name = "End radius";
  inputTreeLastRad.value_float = &treeParams.finalRad;
  snprintf(inputTreeLastRad.text, sizeof inputTreeLastRad.text, "%1.2f", treeParams.finalRad);

  InputBoxDesc inputTreeUpCoef;
  inputTreeUpCoef.type = InputBoxType::VALUE_FLOAT;
  inputTreeUpCoef.name_rect = Rectangle{ 320, 140, 80, 30 };
  inputTreeUpCoef.input_box_rect = Rectangle{ 480, 140, 70, 30 };
  inputTreeUpCoef.box_name = "Branch up K";
  inputTreeUpCoef.value_float = &treeParams.upCoef;
  snprintf(inputTreeUpCoef.text, sizeof inputTreeUpCoef.text, "%1.2f", treeParams.upCoef);

  InputBoxDesc inputTreeEdgeBase;
  inputTreeEdgeBase.type = InputBoxType::VALUE_INT;
  inputTreeEdgeBase.name_rect = Rectangle{ 320, 180, 80, 30 };
  inputTreeEdgeBase.input_box_rect = Rectangle{ 480, 180, 70, 30 };
  inputTreeEdgeBase.box_name = "Trunk edges";
  inputTreeEdgeBase.value_int = &treeParams.edgeBase;
  
  InputBoxDesc inputTreeBranchMinCount;
  InputBoxDesc inputTreeBranchMaxCount;
  inputTreeBranchMinCount.value_int = &treeParams.branchMinCount;
  inputTreeBranchMaxCount.value_int = &treeParams.branchMaxCount;
  inputTreeBranchMinCount.type = InputBoxType::VALUE_INT;
  inputTreeBranchMaxCount.type = InputBoxType::VALUE_INT;
  inputTreeBranchMinCount.name_rect = Rectangle{ 450, 220, 10, 30 };
  inputTreeBranchMaxCount.name_rect = Rectangle{ 560, 220, 10, 30 };
  inputTreeBranchMinCount.input_box_rect = Rectangle{ 480, 220, 70, 30 };
  inputTreeBranchMaxCount.input_box_rect = Rectangle{ 590, 220, 70, 30 };
  inputTreeBranchMinCount.box_name = "min";
  inputTreeBranchMaxCount.box_name = "max";

  InputBoxDesc inputTreeCrownBase;
  inputTreeCrownBase.type = InputBoxType::VALUE_INT;
  inputTreeCrownBase.name_rect = Rectangle{ 320, 260, 80, 30 };
  inputTreeCrownBase.input_box_rect = Rectangle{ 480, 260, 70, 30 };
  inputTreeCrownBase.box_name = "Crown vertex num";
  inputTreeCrownBase.value_int = &treeParams.crownVertexNum;

  Rectangle checkBoxTreeRebuildRectangle = { 320, 300, 30, 30 };
  InputBoxDesc inputTreeDMin;
  InputBoxDesc inputTreeDMax;
  snprintf(inputTreeDMin.text, sizeof inputTreeDMin.text, "%1.2f", treeRebuildParams.disp.x);
  snprintf(inputTreeDMax.text, sizeof inputTreeDMax.text, "%1.2f", treeRebuildParams.disp.y);
  inputTreeDMin.type = InputBoxType::VALUE_FLOAT;
  inputTreeDMax.type = InputBoxType::VALUE_FLOAT;
  inputTreeDMin.name_rect = Rectangle{ 450, 340, 10, 30 };
  inputTreeDMax.name_rect = Rectangle{ 560, 340, 10, 30 };
  inputTreeDMin.input_box_rect = Rectangle{ 480, 340, 70, 30 };
  inputTreeDMax.input_box_rect = Rectangle{ 590, 340, 70, 30 };
  inputTreeDMin.box_name = "min";
  inputTreeDMax.box_name = "max";
  inputTreeDMin.value_float = &treeRebuildParams.disp.x;
  inputTreeDMax.value_float = &treeRebuildParams.disp.y;

  InputBoxDesc inputTreeBalance;
  inputTreeBalance.type = InputBoxType::VALUE_FLOAT;
  inputTreeBalance.name_rect = Rectangle{ 320, 380, 80, 30 };
  inputTreeBalance.input_box_rect = Rectangle{ 480, 380, 70, 30 };
  inputTreeBalance.box_name = "Balance";
  inputTreeBalance.value_float = &treeRebuildParams.balance;
  snprintf(inputTreeBalance.text, sizeof inputTreeBalance.text, "%1.2f", treeRebuildParams.balance);

  InputBoxDesc inputTreeCentered;
  inputTreeCentered.type = InputBoxType::VALUE_FLOAT;
  inputTreeCentered.name_rect = Rectangle{ 320, 420, 80, 30 };
  inputTreeCentered.input_box_rect = Rectangle{ 480, 420, 70, 30 };
  inputTreeCentered.box_name = "Center accur";
  inputTreeCentered.value_float = &treeRebuildParams.centered;
  snprintf(inputTreeCentered.text, sizeof inputTreeCentered.text, "%1.2f", treeRebuildParams.centered);

  InputBoxDesc inputTreeRebuildNum;
  inputTreeRebuildNum.type = InputBoxType::VALUE_INT;
  inputTreeRebuildNum.name_rect = Rectangle{ 320, 460, 80, 30 };
  inputTreeRebuildNum.input_box_rect = Rectangle{ 480, 460, 70, 30 };
  inputTreeRebuildNum.box_name = "Rebuild num";
  inputTreeRebuildNum.value_int = &treeRebuildParams.rebuildNum;

  // BUSH

  InputBoxDesc inputBushHeight;
  inputBushHeight.type = InputBoxType::VALUE_FLOAT;
  inputBushHeight.name_rect = Rectangle{ 320, 20, 70, 30 };
  inputBushHeight.input_box_rect = Rectangle{ 480, 20, 70, 30 };
  inputBushHeight.box_name = "Height";
  inputBushHeight.value_float = &bushParams.height;
  snprintf(inputBushHeight.text, sizeof inputBushHeight.text, "%1.2f", bushParams.height);

  InputBoxDesc inputBushFirstRad;
  inputBushFirstRad.type = InputBoxType::VALUE_FLOAT;
  inputBushFirstRad.name_rect = Rectangle{ 320, 60, 80, 30 };
  inputBushFirstRad.input_box_rect = Rectangle{ 480, 60, 70, 30 };
  inputBushFirstRad.box_name = "Base radius";
  inputBushFirstRad.value_float = &bushParams.firstRad;
  snprintf(inputBushFirstRad.text, sizeof inputBushFirstRad.text, "%1.2f", bushParams.firstRad);

  InputBoxDesc inputBushLastRad;
  inputBushLastRad.type = InputBoxType::VALUE_FLOAT;
  inputBushLastRad.name_rect = Rectangle{ 320, 100, 80, 30 };
  inputBushLastRad.input_box_rect = Rectangle{ 480, 100, 70, 30 };
  inputBushLastRad.box_name = "End radius";
  inputBushLastRad.value_float = &bushParams.finalRad;
  snprintf(inputBushLastRad.text, sizeof inputBushLastRad.text, "%1.2f", bushParams.finalRad);

  InputBoxDesc inputBushUpCoef;
  inputBushUpCoef.type = InputBoxType::VALUE_FLOAT;
  inputBushUpCoef.name_rect = Rectangle{ 320, 140, 80, 30 };
  inputBushUpCoef.input_box_rect = Rectangle{ 480, 140, 70, 30 };
  inputBushUpCoef.box_name = "Branch up K";
  inputBushUpCoef.value_float = &bushParams.upCoef;
  snprintf(inputBushUpCoef.text, sizeof inputBushUpCoef.text, "%1.2f", bushParams.upCoef);

  InputBoxDesc inputBushEdgeBase;
  inputBushEdgeBase.type = InputBoxType::VALUE_INT;
  inputBushEdgeBase.name_rect = Rectangle{ 320, 180, 80, 30 };
  inputBushEdgeBase.input_box_rect = Rectangle{ 480, 180, 70, 30 };
  inputBushEdgeBase.box_name = "Trunk edges";
  inputBushEdgeBase.value_int = &bushParams.edgeBase;

  InputBoxDesc inputBushBranchMinCount;
  InputBoxDesc inputBushBranchMaxCount;
  inputBushBranchMinCount.value_int = &bushParams.baseBranchMinCount;
  inputBushBranchMaxCount.value_int = &bushParams.baseBranchMaxCount;
  inputBushBranchMinCount.type = InputBoxType::VALUE_INT;
  inputBushBranchMaxCount.type = InputBoxType::VALUE_INT;
  inputBushBranchMinCount.name_rect = Rectangle{ 450, 220, 10, 30 };
  inputBushBranchMaxCount.name_rect = Rectangle{ 560, 220, 10, 30 };
  inputBushBranchMinCount.input_box_rect = Rectangle{ 480, 220, 70, 30 };
  inputBushBranchMaxCount.input_box_rect = Rectangle{ 590, 220, 70, 30 };
  inputBushBranchMinCount.box_name = "min";
  inputBushBranchMaxCount.box_name = "max";

  InputBoxDesc inputBushChildrenMinCount;
  InputBoxDesc inputBushChildrenMaxCount;
  inputBushChildrenMinCount.value_int = &bushParams.childrenMinCount;
  inputBushChildrenMaxCount.value_int = &bushParams.childrenMaxCount;
  inputBushChildrenMinCount.type = InputBoxType::VALUE_INT;
  inputBushChildrenMaxCount.type = InputBoxType::VALUE_INT;
  inputBushChildrenMinCount.name_rect = Rectangle{ 450, 260, 10, 30 };
  inputBushChildrenMaxCount.name_rect = Rectangle{ 560, 260, 10, 30 };
  inputBushChildrenMinCount.input_box_rect = Rectangle{ 480, 260, 70, 30 };
  inputBushChildrenMaxCount.input_box_rect = Rectangle{ 590, 260, 70, 30 };
  inputBushChildrenMinCount.box_name = "min";
  inputBushChildrenMaxCount.box_name = "max";

  InputBoxDesc inputBushCrownBase;
  inputBushCrownBase.type = InputBoxType::VALUE_INT;
  inputBushCrownBase.name_rect = Rectangle{ 320, 300, 80, 30 };
  inputBushCrownBase.input_box_rect = Rectangle{ 480, 300, 70, 30 };
  inputBushCrownBase.box_name = "Crown vertex num";
  inputBushCrownBase.value_int = &bushParams.crownVertexNum;

  // LOAD MODEL
  int modelTypeActive = TEST;
  bool modelTypeEditMode = false;
  std::unique_ptr<lpng::GenerateObject> modelPtr{ std::make_unique<lpng::GenerateObjectTest>() };
  modelPtr->SetSize(objectSize);
  modelPtr->Generate();
  unsigned int modelSeed = modelPtr->GetModelSeed();
  std::vector<lpng::Mesh> generatedModel = modelPtr->GetModel();
  std::vector<Model> models;
  std::filesystem::path dir_path = std::filesystem::current_path();
  lpng::ModelMaterial::CreateModelTexture(dir_path/"resources");
  modelBaseTex = LoadTexture("resources/obj.tga");
  modelWoodTex = LoadTexture("resources/wood.tga");
  modelCrownTex = LoadTexture("resources/crown.tga");
  modelStoneTex = LoadTexture("resources/stone.tga");
  GenModels(models, generatedModel);
  
  while (!WindowShouldClose())
  {
    // CURSOR ACT
    {
      if (IsCursorHidden() && selectedInputBox == nullptr) UpdateCamera(&camera, CAMERA_THIRD_PERSON);
      if (IsKeyPressed(KEY_TAB))
      {
        if (IsCursorHidden()) EnableCursor();
        else DisableCursor();
      }
      mousePoint = GetMousePosition();
    }
    
    // DRAW LEFT WINDOW PART
    {
      BeginTextureMode(screen1);
      ClearBackground({ 230, 245, 255, 255 });

      if (modelTypeEditMode) GuiLock();
      GuiCheckBox(checkBoxSmoothnessRectangle, "Smoothness", &modelSmoothness);
      GuiCheckBox(checkBoxSeedRectangle, "Use seed", &goalModelSeedCheck);
      if (goalModelSeedCheck)
      {
        InputBox(inputBoxSeed);
      }
      if (GuiButton(btnCreateBounds, createModelText.c_str()))
      {
        bool success = GenerateObjectWithType(modelTypeActive, modelPtr);
        if (!success)
          break;
        modelPtr->Generate();
        modelSeed = modelPtr->GetModelSeed();
        generatedModel = modelPtr->GetModel();
        for (const Model& m : models)
        {
          UnloadModel(m);
        }
        models.clear();
        GenModels(models, generatedModel);
      }
      if (GuiButton(btnSaveBounds, saveModelText.c_str())) modelPtr->SaveModel(inputBoxModelName.text);
      InputBox(inputBoxModelName);
      if (GuiButton(btnCopySeedBounds, copySeedText.c_str())) 
      {
        char buffer[16];
        _itoa(modelSeed, buffer, 10);
        SetClipboardText(buffer);
      }

      if (modelTypeActive == TREE)
      {
        InputBox(inputTreeHeight);
        InputBox(inputTreeFirstRad);
        InputBox(inputTreeLastRad);
        InputBox(inputTreeUpCoef);
        InputBox(inputTreeEdgeBase);
        DrawText(TextFormat("Branches"), 320, 228, 16, BLACK);
        InputBox(inputTreeBranchMinCount);
        InputBox(inputTreeBranchMaxCount);
        InputBox(inputTreeCrownBase);
        GuiCheckBox(checkBoxTreeRebuildRectangle, "Use tree rebuild check", &treeRebuildCheck);
        if (treeRebuildCheck)
        {
          DrawText(TextFormat("Branches disp"), 320, 348, 16, BLACK);
          InputBox(inputTreeDMin);
          InputBox(inputTreeDMax);
          InputBox(inputTreeBalance);
          InputBox(inputTreeCentered);
          InputBox(inputTreeRebuildNum);
        }
      }
      else if (modelTypeActive == BUSH)
      {
        InputBox(inputBushHeight);
        InputBox(inputBushFirstRad);
        InputBox(inputBushLastRad);
        InputBox(inputBushUpCoef);
        InputBox(inputBushEdgeBase);
        DrawText(TextFormat("Base branches"), 320, 228, 16, BLACK);
        InputBox(inputBushBranchMinCount);
        InputBox(inputBushBranchMaxCount);
        DrawText(TextFormat("Children count"), 320, 268, 16, BLACK);
        InputBox(inputBushChildrenMinCount);
        InputBox(inputBushChildrenMaxCount);
        InputBox(inputBushCrownBase);
      }
      else 
      {
        DrawText(TextFormat("Model size"), 320, 28, 16, BLACK);
        InputBox(inputSizeX);
        InputBox(inputSizeY);
        InputBox(inputSizeZ);
        if (modelTypeActive == STONE)
        {
          InputBox(inputStoneVertexCount);
        }
      }

      GuiUnlock();

      DrawText(TextFormat("MODEL SEED : %u", modelSeed), 10, 710, 22, MAROON);
      DrawText("PRESS TAB TO ENABLE CURSOR", 10, 760, 22, MAROON);
      if (GuiDropdownBox({ 12, 20, 140, 30 }, "TEST;STONE;TREE;BUSH", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode;

      EndTextureMode();
    }

    // DRAW RIGHT WINDOW PART
    {
      BeginTextureMode(screen2);
      ClearBackground(RAYWHITE);

      // DRAW MODEL
      {
        BeginMode3D(camera);

        for (const Model& m : models)
        {
          DrawModel(m, modelPosition, 1.0f, WHITE);
          DrawModelWires(m, modelPosition, 1.0f, BLACK);
        }
        DrawGrid(10, 1.0f);
        EndMode3D();
      }
      EndTextureMode();
    } 

    // DRAW WINDOW 
    {
      BeginDrawing();
      DrawTextureRec(screen1.texture, splitScreenRect, { 0, 0 }, WHITE);
      DrawTextureRec(screen2.texture, splitScreenRect, { screenWidth / 2.0f, 0 }, WHITE);
      DrawRectangle(GetScreenWidth() / 2 - 2, 0, 4, GetScreenHeight(), LIGHTGRAY);
      EndDrawing();
    }
  }
  UnloadRenderTexture(screen1);
  UnloadRenderTexture(screen2);
  for (const Model& m : models)
  {
    UnloadModel(m);
  }
  modelPtr.reset();
  UnloadTexture(modelBaseTex);
  UnloadTexture(modelWoodTex);
  UnloadTexture(modelCrownTex);
  UnloadTexture(modelStoneTex);
  CloseWindow();

  return 0;
}


static Mesh GenMesh(const lpng::Mesh& model)
{
  int triangleCount = model.faces.size();
  Mesh mesh = { 0 };
  mesh.triangleCount = triangleCount;
  mesh.vertexCount = mesh.triangleCount * 3;
  mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
  mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

  int nextVertId = 0;
  for (const lpng::Face& f : model.faces)
  {
    for (int i = 0; i < 3; i++)
    {
      lpng::float3 vc = model.vertexCoords[f.vi[i] - 1];
      lpng::float3 vn = model.vertexNormals[f.vni[i] - 1];
      lpng::float2 vt = model.vertexTextCoords[f.vti[i] - 1];
      mesh.vertices[nextVertId * 3] = vc.x;
      mesh.vertices[nextVertId * 3 + 1] = vc.y;
      mesh.vertices[nextVertId * 3 + 2] = vc.z;
      mesh.normals[nextVertId * 3] = vn.x;
      mesh.normals[nextVertId * 3 + 1] = vn.y;
      mesh.normals[nextVertId * 3 + 2] = vn.z;
      mesh.texcoords[nextVertId * 2] = vt.x;
      mesh.texcoords[nextVertId * 2 + 1] = vt.y;
      ++nextVertId;
    }
  }
  UploadMesh(&mesh, false);
  return mesh;
}


static void GenModels(std::vector<Model>& models, const std::vector<lpng::Mesh>& generated_model)
{
  for (const lpng::Mesh& mesh : generated_model)
  {
    models.emplace_back(LoadModelFromMesh(GenMesh(mesh)));
    if (mesh.matType == lpng::MaterialTypes::NONE)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelBaseTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::WOOD)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelWoodTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::CROWN)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelCrownTex;
    }
    else if (mesh.matType == lpng::MaterialTypes::STONE)
    {
      models.back().materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = modelStoneTex;
    }
  }
}


static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  bool res = false;
  lpng::float3 model_size;

  switch (type)
  {
  case TEST:
  {
    lpng::GenerateObjectTest* test_ptr = new lpng::GenerateObjectTest();
    model_size = objectSize;
    model_ptr.reset(test_ptr);
    res = true;
    break;
  }
    
  case STONE:
  {
    lpng::GenerateObjectStone* stone_ptr = new lpng::GenerateObjectStone();
    model_size = objectSize;
    stone_ptr->SetVertexCount(stoneVertexCount);
    model_ptr.reset(stone_ptr);
    res = true;
    break;
  }

  case BUSH:
  {
    lpng::GenerateObjectBush* bush_ptr = new lpng::GenerateObjectBush();
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
    model_size.x = 2 * treeParams.firstRad;
    model_size.z = 2 * treeParams.firstRad;
    model_size.y = treeParams.height;
    tree_ptr->SetTreeParams(treeParams);
    if (treeRebuildCheck)
      tree_ptr->SetRebuildParams(treeRebuildParams);
    model_ptr.reset(tree_ptr);
    res = true;
    break;
  }
  default:
    break;
  }
  if (res)
  {
    model_ptr->SetSize(model_size);
    model_ptr->SetModelSmoothness(modelSmoothness);
    if (goalModelSeedCheck && goalModelSeed >= 0)
      model_ptr->SetModelSeed(goalModelSeed);
  }
  return res;
}


static void InputBox(InputBoxDesc& inputBox)
{
  if (inputBox.box_name.size() > 0)
    DrawText(inputBox.box_name.c_str(), (int)inputBox.name_rect.x, (int)inputBox.name_rect.y + 8, 16, BLACK);
  if (CheckCollisionPointRec(mousePoint, inputBox.input_box_rect))
  {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
      inputBox.state = STATE_PRESSED;
    }  
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && inputBox.state == STATE_PRESSED || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      selectedInputBox = inputBox.text;
      inputBox.state = STATE_FOCUSED;
    }
  }
  else
  {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && inputBox.state == STATE_PRESSED)
    {
      inputBox.state = STATE_PRESSED;
    }
    else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
      if (selectedInputBox == inputBox.text)
        selectedInputBox = nullptr;
      inputBox.state = STATE_NORMAL;
    }
  }
  switch (inputBox.type)
  {
  case InputBoxType::TEXT:
  {
    GuiTextBox(inputBox.input_box_rect, inputBox.text, 1024, selectedInputBox == inputBox.text);
    break;
  }
  case InputBoxType::VALUE_INT:
  {
    GuiValueBox(inputBox.input_box_rect, inputBox.text, inputBox.value_int, 0, INT_MAX, selectedInputBox == inputBox.text);
    break;
  }
  case InputBoxType::VALUE_FLOAT:
  {
    GuiValueBoxF(inputBox.input_box_rect, nullptr, inputBox.text, inputBox.value_float, selectedInputBox == inputBox.text);
    break;
  }
  default:
  {
    break;
  }
  }
  
}