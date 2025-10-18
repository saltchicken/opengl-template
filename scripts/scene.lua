-- This function will be called by C++ to build the scene.
-- It receives the C++ Scene object as an argument.
function build_scene(scene)
	print("[Lua] Starting scene construction.")

	-- Create and set up the camera
	local camera_object = SceneObject.new() -- No mesh for the camera
	camera_object.transform.position = vec3.new(0.0, 0.0, 5.0)
	camera_object:add_camera_component(45.0, 0.1, 100.0)
	scene:add_object(camera_object)
	scene:set_active_camera(camera_object)

	-- Load resources
	local cube_mesh = ResourceManager.get_primitive("cube")
	local sphere_mesh = ResourceManager.get_primitive("sphere")
	local test_texture = ResourceManager.load_texture("test", "assets/textures/test.png")

	if test_texture then
		-- Use our new C++ helper function
		cube_mesh:add_texture(test_texture)
		sphere_mesh:add_texture(test_texture)
	end

	-- Object 1: Rotating Cube
	local rotating_object = SceneObject.new(cube_mesh)
	rotating_object.transform.position = vec3.new(-1.5, 0.0, 0.0)
	rotating_object:add_rotation_animator(vec3.new(0.5, 1.0, 0.0), 50.0)
	scene:add_object(rotating_object)

	-- Object 2: Bobbing Cube
	local bobbing_object = SceneObject.new(cube_mesh)
	bobbing_object.transform.position = vec3.new(1.5, 0.0, 0.0)
	bobbing_object:add_bobbing_animator(vec3.new(0.0, 1.0, 0.0), 2.0, 0.5)
	scene:add_object(bobbing_object)

	-- Object 3: Multi-animation Cube
	local multi_anim_object = SceneObject.new(cube_mesh)
	multi_anim_object.transform.position = vec3.new(0.0, -1.5, 0.0)
	multi_anim_object:add_rotation_animator(vec3.new(0.0, 1.0, 0.0), 80.0)
	multi_anim_object:add_bobbing_animator(vec3.new(0.0, 1.0, 0.0), 1.5, 0.7)
	scene:add_object(multi_anim_object)

	-- Object 4: Sphere
	local sphere_object = SceneObject.new(sphere_mesh)
	sphere_object.transform.position = vec3.new(0.0, 1.5, 0.0)
	scene:add_object(sphere_object)

	print("[Lua] Scene construction complete.")
end
