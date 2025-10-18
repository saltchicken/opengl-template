#pragma once

#include <memory>
#include <sol/sol.hpp>

class Scene;

class ScriptingManager {
public:
  ScriptingManager() = delete;

  // Initializes the Lua state and binds all C++ functions/types
  static void init();

  // Loads and executes a Lua script that builds a scene
  static bool load_scene_script(Scene &scene, const std::string &filepath);

private:
  // Exposes core types like glm::vec3
  static void bind_core_types();
  // Exposes scene-related classes like Scene, SceneObject, etc.
  static void bind_scene_types();
  // Exposes components like TransformComponent, PropertyAnimatorComponent
  static void bind_component_types();
  // Exposes utility classes like ResourceManager
  static void bind_utility_types();

  // The global Lua state
  static std::unique_ptr<sol::state> s_lua_state;
};
