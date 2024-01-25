#define _CRT_SECURE_NO_WARNINGS
#include <lpnatgen.h>
#include <iostream>
#include <memory>
#include "raylib.h"
#include "raymath.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


struct InputBoxDesc
{
  Rectangle box;
  std::string box_name;
  std::string text;
};

enum ObjectTypes
{
  TEST = 0,
  STONE = 1,
  //BUSH = 2,
  //TREE = 3,
};


static Mesh GenMesh(const std::vector<lpng::Mesh> &model);
static bool GenerateObjectWithType(int type, std::unique_ptr<lpng::GenerateObject>& model_ptr);
static void BtnHandler(const Vector2& mousePoint, const Rectangle& btnBounds, bool& btnAction, int& btnState);
static void BtnDraw(const Rectangle& btnBounds, const std::string& text, const int btnState);
static void InputBoxHandler(const Vector2& mousePoint, const std::vector<InputBoxDesc>& inputBoxes, bool& inputAction, int& inputState, int& activeInputBoxId, size_t& framesCounter);
static void InputHandler(std::vector<InputBoxDesc>& inputBoxes, const bool inputAction, const int activeInputBoxId);
static void InputBoxDraw(const std::vector<InputBoxDesc>& inputBoxes, const int inputState, const int activeInputBoxId, const Font& font, const size_t framesCounter);


int main(void)
{
  const int screenWidth = 1000;
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

  std::vector<InputBoxDesc> inputBoxes{ 
    {Rectangle{ 120, 140, 140, 30 }, "Model name", ""},
    {Rectangle{ 120, 180, 140, 30 }, "Save path", ""}
  };

  int inputState = 0;
  bool inputAction = false;
  bool isInputActive = false;
  int activeInputBoxId = -1;

  int btnSaveState = 0;
  bool btnSaveAction = false;
  Rectangle btnSaveBounds = { 12, 220, 140, 30 };

  Vector2 mousePoint = { 0.0f, 0.0f };

  std::unique_ptr<lpng::GenerateObject> model_ptr{ std::make_unique<lpng::GenerateObjectTest>() };
  model_ptr->Generate();
  std::vector<lpng::Mesh> generatedModel = model_ptr->GetModel();
  Model model = LoadModelFromMesh(GenMesh(generatedModel));


  Font font = LoadFont("resources/fonts/romulus.png");
  size_t framesCounter = 0;

  while (!WindowShouldClose())
  {

    if (IsCursorHidden()) UpdateCamera(&camera, CAMERA_THIRD_PERSON);

    if (IsKeyPressed(KEY_TAB))
    {
      if (IsCursorHidden()) EnableCursor();
      else DisableCursor();
    }

    mousePoint = GetMousePosition();
    BtnHandler(mousePoint, btnCreateBounds, btnCreateAction, btnCreateState);
    BtnHandler(mousePoint, btnSaveBounds, btnSaveAction, btnSaveState);
    InputBoxHandler(mousePoint, inputBoxes, inputAction, inputState, activeInputBoxId, framesCounter);
    InputHandler(inputBoxes, inputAction, activeInputBoxId);

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
    InputBoxDraw(inputBoxes, inputState, activeInputBoxId, font, framesCounter);

    GuiUnlock();

    DrawText("PRESS TAB TO ENABLE CURSOR", 10, 420, 16, MAROON);
    DrawText("PRESS DELETE TO CLEAN INPUTBOX", 10, 400, 16, MAROON);
    if (GuiDropdownBox({ 12, 20, 140, 30 }, "TEST;STONE", &modelTypeActive, modelTypeEditMode)) modelTypeEditMode = !modelTypeEditMode; //;BUSH;TREE
    
    EndDrawing();
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


static void BtnDraw(const Rectangle& btnBounds, const std::string& text, const int btnState)
{
  GuiDrawRectangle(btnBounds, GuiGetStyle(LABEL, BORDER_WIDTH),
    GRAY,
    btnState == 2 ? WHITE : LIGHTGRAY);
  DrawText(text.c_str(),
    (int)(btnBounds.x + btnBounds.width / 2 - MeasureText(text.c_str(), 16) / 2),
    (int)(btnBounds.y + btnBounds.height / 2 - 8),
    16, DARKGRAY);
}


static void InputBoxHandler(const Vector2& mousePoint, const std::vector<InputBoxDesc>& inputBoxes, bool& inputAction, int& inputState, int& activeInputBoxId, size_t& framesCounter)
{
  bool mouseInTextBox = false;
  int boxCollId = -1;

  for (int i = 0; i < inputBoxes.size(); ++i)
  {
    if (CheckCollisionPointRec(mousePoint, inputBoxes[i].box))
    {
      mouseInTextBox |= true;
      boxCollId = i;
    }
  }

  if (mouseInTextBox) SetMouseCursor(MOUSE_CURSOR_IBEAM);
  else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

  if (mouseInTextBox && boxCollId >= 0)
  {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) inputState = 2;
    else inputState = 1;

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
      inputAction = true;
      activeInputBoxId = boxCollId;
    }
  }
  else inputState = 0;

  if (inputState == 0 && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
  {
    inputAction = false;
    activeInputBoxId = -1;
  }

  if (inputAction) ++framesCounter;
  else framesCounter = 0;
}


static void InputHandler(std::vector<InputBoxDesc>& inputBoxes, const bool inputAction, const int activeInputBoxId)
{
  if (inputAction && activeInputBoxId >= 0)
  {
    std::string& text = inputBoxes[activeInputBoxId].text;
    int key = GetCharPressed();

    while (key > 0)
    {
      if ((key >= 32) && (key <= 125))
      {
        text += (char)key;
      }

      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && text.size() > 0)
    {
      text.pop_back();
    }

    if (IsKeyPressed(KEY_DELETE))
    {
      text.clear();
    }

    if (activeInputBoxId >= 0 && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C))
    {
      SetClipboardText(text.c_str());
    }
    if (activeInputBoxId >= 0 && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V))
    {
      text += GetClipboardText();
    }
  }
}


static void InputBoxDraw(const std::vector<InputBoxDesc>& inputBoxes, const int inputState, const int activeInputBoxId, const Font& font, const size_t framesCounter)
{
  for (int i = 0; i < inputBoxes.size(); ++i)
  {
    const std::string& text = inputBoxes[i].text;
    const std::string& box_name = inputBoxes[i].box_name;
    const Rectangle& box = inputBoxes[i].box;
    GuiDrawRectangle(box, GuiGetStyle(LABEL, BORDER_WIDTH),
      GRAY,
      activeInputBoxId == i ? WHITE : LIGHTGRAY);
    
    if (((framesCounter / 30) % 2) == 0 && activeInputBoxId == i)
    {
      DrawTextEx(font, "|", { box.x + 3 + MeasureTextEx(font, text.c_str(), 16, 2).x, box.y + 8 }, 20, 2, BLACK);
    }
    DrawTextEx(font, text.c_str(), { box.x + 3, box.y + 8 }, 16, 2, BLACK);
    DrawText(box_name.c_str(), 12, (int)box.y + 8, 16, BLACK);
  }
}