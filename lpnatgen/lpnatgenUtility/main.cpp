#define _CRT_SECURE_NO_WARNINGS
#include <lpnatgen.h>
#include <iostream>
#include <memory>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define NUM_MODELS 1 

static Mesh GenMesh(const std::vector<lpng::Mesh> &model);
static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);


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

  int modelTypeActive = TEST;
  bool modelTypeEditMode = false;

  int btnCreateState = 0;
  bool btnCreateAction = false;
  Rectangle btnCreateBounds = { 12, 120, 140, 28 };

  Vector2 mousePoint = { 0.0f, 0.0f };

  std::unique_ptr<lpng::GenerateObject> model_ptr{ std::make_unique<lpng::GenerateObjectTest>() };
  model_ptr->Generate();
  std::vector<lpng::Mesh> generatedModel = model_ptr->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));

  while (!WindowShouldClose())
  {

    if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    if (IsKeyPressed(KEY_F))
    {
      if (IsCursorHidden()) EnableCursor();
      else DisableCursor();
    }

    mousePoint = GetMousePosition();
    btnCreateAction = false;

    if (CheckCollisionPointRec(mousePoint, btnCreateBounds))
    {
      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) btnCreateState = 2;
      else btnCreateState = 1;

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) btnCreateAction = true;
    }
    else btnCreateState = 0;

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
    GuiDrawRectangle(btnCreateBounds, GuiGetStyle(LABEL, BORDER_WIDTH), 
      GRAY,
      btnCreateState == 2 ? WHITE : LIGHTGRAY);
    DrawText("Create model", 
      (int)(btnCreateBounds.x + btnCreateBounds.width / 2 - MeasureText("Create model", 16)/2), 
      (int)(btnCreateBounds.y + btnCreateBounds.height / 2 - 8),
      16, DARKGRAY);
    GuiUnlock();
    if (GuiDropdownBox({ 12, 40, 140, 28 }, "TEST;STONE", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode; //;BUSH;TREE
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