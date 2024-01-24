#define _CRT_SECURE_NO_WARNINGS
#include <lpnatgen.h>
#include <iostream>
#include <memory>
#include <map>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define NUM_MODELS 1 


static Mesh GenMesh(const std::vector<lpng::Mesh> &model);
static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static void BtnHandler(const Vector2& mousePoint, const Rectangle& btnBounds, bool& btnAction, int& btnState);
static void BtnDraw(const Rectangle& btnBounds, const std::string& text, const int& btnState);


enum ObjectTypes
{
  TEST = 0,
  STONE = 1,
  //BUSH = 2,
  //TREE = 3,
};


int main(void)
{
  const int screenWidth = 800;
  const int screenHeight = 450;

  InitWindow(screenWidth, screenHeight, "lpnatgenUtility : object generation");

  // Define the camera to look into our 3d world
  Camera camera = { 0 };
  camera.position = { 10.0f, 10.0f, 10.0f };
  camera.target = { 0.0f, 0.0f, 0.0f };
  camera.up = { 0.0f, 1.0f, 0.0f };
  camera.fovy = 45.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Vector3 modelPosition = { 0.0f, 0.0f, 0.0f };
  Vector2 modelScreenPosition = { 0.0f, 0.0f };

  DisableCursor();

  SetTargetFPS(60);

  Font fonts = LoadFont("resources/fonts/alpha_beta.png");

  int modelTypeActive = TEST;
  bool modelTypeEditMode = false;

  int btnCreateState = 0;
  bool btnCreateAction = false;
  Rectangle btnCreateBounds = { 12, 100, 140, 30 };
  std::string createModelText = "Create model";

  std::string saveModelText = "Save model";

  std::string modelNameText = "Model name";
  std::string modelPathText = "Save path";
  //std::map<Rectangle, std::string> inputBoxes {{{ 40, 140, 140, 30 }, ""}, {{ 40, 180, 140, 30 }, ""}};

  int btnInputState = 0;
  bool btnInputAction = false;
  bool isInputActive = false;
  int* activeStingPtr = nullptr;

  int btnSaveState = 0;
  bool btnSaveAction = false;
  Rectangle btnSaveBounds = { 12, 210, 140, 30 };


  Vector2 mousePoint = { 0.0f, 0.0f };

  std::unique_ptr<lpng::GenerateObject> model_ptr{ std::make_unique<lpng::GenerateObjectTest>() };
  model_ptr->Generate();
  std::vector<lpng::Mesh> generatedModel = model_ptr->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));

  while (!WindowShouldClose())
  {

    if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    if (IsKeyPressed(KEY_SPACE))
    {
      if (IsCursorHidden()) EnableCursor();
      else DisableCursor();
    }

    mousePoint = GetMousePosition();
    BtnHandler(mousePoint, btnCreateBounds, btnCreateAction, btnCreateState);
    BtnHandler(mousePoint, btnSaveBounds, btnSaveAction, btnSaveState);

    if (btnCreateAction)
    {
      bool isNewModel = GenerateObjectWithType(modelTypeActive, model_ptr);
      if (!isNewModel)
        break;
      model_ptr->Generate();
      generatedModel = model_ptr->GetModel();
      UnloadModel(model);
      model = LoadModelFromMesh(GenMesh(generatedModel));
    }

    if (btnSaveAction)
    {
      model_ptr->SaveModel();
    }

    modelScreenPosition = GetWorldToScreen({ modelPosition.x, modelPosition.y, modelPosition.z }, camera);

    BeginDrawing();

    ClearBackground(RAYWHITE);

    BeginMode3D(camera);

    for (const lpng::Mesh& m : generatedModel)
    {
      for (const lpng::float3& v : m.vertexCoords)
      {
        DrawSphere({v.x, v.y, v.z}, 0.05f, DARKPURPLE);
      }
    }
    
    DrawModel(model, modelPosition, 1.0f, RED);
    DrawModelWires(model, modelPosition, 1.0f, DARKPURPLE);

    DrawGrid(10, 1.0f);
    EndMode3D();

    if (modelTypeEditMode) GuiLock();
    BtnDraw(btnCreateBounds, createModelText, btnCreateState);
    BtnDraw(btnSaveBounds, saveModelText, btnSaveState);

    GuiUnlock();
    DrawText("PRESS SPACE TO ENABLE CURSOR", 10, 400, 16, MAROON);
    if (GuiDropdownBox({ 12, 20, 140, 30 }, "TEST;STONE", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode; //;BUSH;TREE
    EndDrawing();

    if (IsKeyPressed(KEY_ONE)) modelTypeActive = 0;
    else if (IsKeyPressed(KEY_TWO)) modelTypeActive = 1;
    //else if (IsKeyPressed(KEY_THREE)) modelTypeActive = 2;
    //else if (IsKeyPressed(KEY_FOUR)) modelTypeActive = 3;
  }

  UnloadModel(model);
  model_ptr.reset();

  CloseWindow();

  return 0;
}

static Mesh GenMesh(const std::vector<lpng::Mesh>& model)
{
  int triangleCount = 0;
  for (const lpng::Mesh& m : model)
  {
    triangleCount += m.faces.size();
  }
  Mesh mesh = { 0 };
  mesh.triangleCount = triangleCount;
  mesh.vertexCount = mesh.triangleCount * 3;
  mesh.vertices = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));
  mesh.texcoords = (float*)MemAlloc(mesh.vertexCount * 2 * sizeof(float));
  mesh.normals = (float*)MemAlloc(mesh.vertexCount * 3 * sizeof(float));

  int nextVertId = 0;
  for (const lpng::Mesh& obj : model)
  {
    for (const lpng::Face& f : obj.faces)
    {
      for (int i = 0; i < 3; i++)
      {
        lpng::Vertex v = f[i];
        lpng::float3 vc = obj.vertexCoords[v.vi - 1];
        lpng::float3 vn = obj.vertexNormals[v.vni - 1];
        lpng::float2 vt = obj.vertexTextCoords[v.vti - 1];
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
  }
  UploadMesh(&mesh, false);

  return mesh;
}


static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr)
{
  switch (type)
  {
  case TEST:
  {
    model_ptr.reset(new lpng::GenerateObjectTest());
    return true;
  }
    
  case STONE:
  {
    model_ptr.reset(new lpng::GenerateObjectStone());
    return true;
  }
    
  //case BUSH:
  //  return new lpng::GenerateObjectBush();
  //case TREE:
  //  return new lpng::GenerateObjectTree();
  default:
    return false;
  }
  return false;
}

static void BtnHandler(const Vector2& mousePoint, const Rectangle& btnBounds, bool& btnAction, int& btnState)
{
  btnAction = false;

  if (CheckCollisionPointRec(mousePoint, btnBounds))
  {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnState = 2;
    else btnState = 1;

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnAction = true;
  }
  else btnState = 0;
}

static void BtnDraw(const Rectangle& btnBounds, const std::string& text, const int& btnState)
{
  GuiDrawRectangle(btnBounds, GuiGetStyle(LABEL, BORDER_WIDTH),
    GRAY,
    btnState == 2 ? WHITE : LIGHTGRAY);
  DrawText(text.c_str(),
    (int)(btnBounds.x + btnBounds.width / 2 - MeasureText(text.c_str(), 16) / 2),
    (int)(btnBounds.y + btnBounds.height / 2 - 8),
    16, DARKGRAY);
}