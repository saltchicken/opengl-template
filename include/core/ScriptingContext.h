#pragma once

// Forward declarations of the systems we want to expose to Lua
class Scene;
class IRenderer;

struct ScriptingContext {
  Scene *scene = nullptr;
  IRenderer *renderer = nullptr;
  // AudioEngine* audioEngine = nullptr;
};
