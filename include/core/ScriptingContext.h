#pragma once

// Forward declarations of the systems we want to expose to Lua
class Scene;
class Renderer;

struct ScriptingContext {
  Scene *scene = nullptr;
  Renderer *renderer = nullptr;
  // AudioEngine* audioEngine = nullptr;
};
