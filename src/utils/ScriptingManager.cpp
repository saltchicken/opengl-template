#include "utils/ScriptingManager.h"
#include "core/Settings.h"
#include "scene/CameraComponent.h"
#include "scene/PropertyAnimatorComponent.h"
#include "scene/Scene.h"
#include "scene/SceneObject.h"
#include "utils/Log.h"
#include "utils/ResourceManager.h"
#include <glm/glm.hpp>

// Instantiate the static lua state
std::unique_ptr<sol::state> ScriptingManager::s_lua_state = nullptr;

void ScriptingManager::init() {
  if (s_lua_state) {
    Log::warn("ScriptingManager already initialized.");
    return;
  }

  s_lua_state = std::make_unique<sol::state>();
  // Open standard libraries (io, string, math, etc.)
  s_lua_state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math,
                              sol::lib::string, sol::lib::table);

  // Bind all our C++ types and functions
  bind_core_types();
  bind_settings_types();
  bind_utility_types();
  bind_component_types();
  bind_scene_types();

  Log::info("ScriptingManager initialized.");
}

bool ScriptingManager::load_runtime_settings(Settings &settings,
                                             const std::string &filepath) {
  if (!s_lua_state) {
    Log::error("ScriptingManager not initialized. Cannot load runtime script.");
    return false;
  }
  try {
    // Load and run the script file
    s_lua_state->script_file(filepath);

    // Call the 'load_shaders' function from the script, if it exists
    sol::function load_shaders_func = (*s_lua_state)["load_shaders"];
    if (load_shaders_func.valid()) {
      auto result = load_shaders_func();
      if (!result.valid()) {
        sol::error err = result;
        Log::error("Error executing load_shaders function: " +
                   std::string(err.what()));
        // Continue anyway, it might not be a fatal error
      }
    } else {
      Log::warn("Script '" + filepath +
                "' does not contain a 'load_shaders' function.");
    }

    // Call the 'configure_renderer' function, passing the C++ config object
    sol::function configure_func = (*s_lua_state)["configure_renderer"];
    if (configure_func.valid()) {
      auto result = configure_func(settings.get_mutable_config());
      if (!result.valid()) {
        sol::error err = result;
        Log::error("Error executing configure_renderer function: " +
                   std::string(err.what()));
        return false; // This is more critical
      }
    } else {
      Log::error("Script '" + filepath +
                 "' does not contain a 'configure_renderer' function.");
      return false;
    }

  } catch (const sol::error &e) {
    Log::error("Failed to load or execute runtime settings script '" +
               filepath + "': " + e.what());
    return false;
  }
  Log::info("Runtime settings loaded from " + filepath);
  return true;
}

bool ScriptingManager::load_scene_script(Scene &scene,
                                         const std::string &filepath) {
  if (!s_lua_state) {
    Log::error("ScriptingManager not initialized. Cannot load scene script.");
    return false;
  }

  try {
    // Load and run the script file
    s_lua_state->script_file(filepath);

    // Get the 'build_scene' function from the script
    sol::function build_func = (*s_lua_state)["build_scene"];
    if (build_func.valid()) {
      // Call the Lua function, passing the C++ scene object to it
      auto result = build_func(scene);
      if (!result.valid()) {
        sol::error err = result;
        Log::error("Error executing build_scene function: " +
                   std::string(err.what()));
        return false;
      }
    } else {
      Log::error("Script '" + filepath +
                 "' does not contain a 'build_scene' function.");
      return false;
    }

  } catch (const sol::error &e) {
    Log::error("Failed to load or execute scene script '" + filepath +
               "': " + e.what());
    return false;
  }
  return true;
}

void ScriptingManager::bind_core_types() {
  // glm::vec3
  s_lua_state->new_usertype<glm::vec3>(
      "vec3",
      sol::constructors<glm::vec3(), glm::vec3(float),
                        glm::vec3(float, float, float)>(),
      "x", &glm::vec3::x, "y", &glm::vec3::y, "z", &glm::vec3::z,
      // Allow vec3 * float
      sol::meta_function::multiplication,
      [](const glm::vec3 &v, float s) { return v * s; },
      // Allow vec3 + vec3
      sol::meta_function::addition,
      [](const glm::vec3 &a, const glm::vec3 &b) { return a + b; });
}

void ScriptingManager::bind_settings_types() {
  s_lua_state->new_usertype<Config>(
      "Config", "main_shader_name", &Config::main_shader_name,
      "background_shader_name", &Config::background_shader_name
      // Other Config fields can be added here if they need to be scriptable
  );
}

void ScriptingManager::bind_utility_types() {
  // ResourceManager
  auto resource_manager_type =
      s_lua_state->new_usertype<ResourceManager>("ResourceManager");
  resource_manager_type["get_primitive"] = &ResourceManager::get_primitive;
  resource_manager_type["load_texture"] = &ResourceManager::load_texture;
  resource_manager_type["load_shader"] =
      &ResourceManager::load_shader; // ‼️ Expose load_shader
}

void ScriptingManager::bind_component_types() {
  // TransformComponent
  s_lua_state->new_usertype<TransformComponent>(
      "TransformComponent", "position", &TransformComponent::position,
      "rotation", &TransformComponent::rotation, "scale",
      &TransformComponent::scale);

  // CameraComponent
  s_lua_state->new_usertype<CameraComponent>(
      "CameraComponent",
      sol::constructors<CameraComponent(float, float, float)>());

  // PropertyAnimatorComponent
  s_lua_state->new_usertype<PropertyAnimatorComponent>(
      "PropertyAnimatorComponent");
  // Expose the TargetProperty enum to Lua
  s_lua_state->new_enum(
      "TargetProperty", "ROTATION",
      PropertyAnimatorComponent::TargetProperty::ROTATION, "POSITION",
      PropertyAnimatorComponent::TargetProperty::POSITION, "SCALE",
      PropertyAnimatorComponent::TargetProperty::SCALE);
}

void ScriptingManager::bind_scene_types() {
  // Add this new usertype binding for Mesh
  s_lua_state->new_usertype<Mesh>(
      "Mesh", "add_texture",
      [](Mesh &self, const std::shared_ptr<Texture> &texture) {
        if (texture) {
          self.textures.push_back(texture);
        }
      });

  // SceneObject
  s_lua_state->new_usertype<SceneObject>(
      "SceneObject",
      sol::factories([]() { return std::make_shared<SceneObject>(); },
                     [](const std::shared_ptr<Mesh> &mesh) {
                       return std::make_shared<SceneObject>(mesh);
                     }),
      "transform", &SceneObject::transform, "mesh", &SceneObject::mesh,
      "add_camera_component",
      &SceneObject::add_component<CameraComponent, float, float, float>,
      "add_rotation_animator",
      [](SceneObject &self, const glm::vec3 &axis, float degrees_per_second) {
        self.add_component<PropertyAnimatorComponent>(
            PropertyAnimatorComponent::TargetProperty::ROTATION,
            PropertyAnimatorComponent::RotationParams{glm::normalize(axis),
                                                      degrees_per_second});
      },
      "add_bobbing_animator",
      [](SceneObject &self, const glm::vec3 &direction, float speed,
         float distance) {
        self.add_component<PropertyAnimatorComponent>(
            PropertyAnimatorComponent::TargetProperty::POSITION,
            PropertyAnimatorComponent::PositionParams{direction, speed,
                                                      distance});
      });

  // Scene
  s_lua_state->new_usertype<Scene>("Scene", "add_object", &Scene::add_object,
                                   "set_active_camera",
                                   &Scene::set_active_camera);
}

void ScriptingManager::execute_string(Scene &scene,
                                      const std::string &command) {
  if (!s_lua_state) {
    Log::error("ScriptingManager not initialized.");
    return;
  }
  try {
    // Set a global variable 'scene' to the current scene object
    (*s_lua_state)["scene"] = &scene;

    auto result = s_lua_state->script(command);

    if (!result.valid()) {
      sol::error err = result;
      Log::error("Lua error: " + std::string(err.what()));
    }
  } catch (const sol::error &e) {
    Log::error("Lua error: " + std::string(e.what()));
  }
}
