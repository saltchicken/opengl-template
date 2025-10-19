#pragma once

#include <memory>
#include <sol/sol.hpp>

class ScriptingContext;
class Scene;
class Settings;
class IRenderer;

class ScriptingManager {
public:
  ScriptingManager() = delete;

  // Initializes the Lua state and binds all C++ functions/types
  static void init();
  static void set_context(ScriptingContext &context);

  // Loads and executes a Lua script that builds a scene
  static bool load_scene_script(Scene &scene, const std::string &filepath);
  static bool load_runtime_settings(Settings &settings,
                                    const std::string &filepath);
  static void run_command(const std::string &command);

private:
  // Exposes core types like glm::vec3
  static void bind_core_types();
  // Exposes Settings
  static void bind_settings_types();
  // Exposes ScriptingContext
  static void bind_context_types();
  // Exposes Renderer
  static void bind_renderer_types();
  // Exposes scene-related classes like Scene, SceneObject, etc.
  static void bind_scene_types();
  // Exposes components like TransformComponent, PropertyAnimatorComponent
  static void bind_component_types();
  // Exposes utility classes like ResourceManager
  static void bind_utility_types();

  // The global Lua state
  static std::unique_ptr<sol::state> s_lua_state;
};
