-- This function loads shaders into the ResourceManager so the renderer can find them by name later.
function load_shaders()
	print("[Lua] Loading runtime shaders...")
	ResourceManager.load_shader("default", "shaders/shader.vert", "shaders/shader.frag")
	ResourceManager.load_shader("canvas", "shaders/canvas.vert", "shaders/canvas.frag")
	ResourceManager.load_shader("canvas_alt", "shaders/canvas.vert", "shaders/canvas_alt.frag")
	-- You could load alternative shaders here as well for interactive switching
	-- ResourceManager.load_shader("cel_shader", "shaders/cel.vert", "shaders/cel.frag")
	ResourceManager.load_shader("box_test", "shaders/canvas.vert", "shaders/box_test.frag")
end

-- This function takes the C++ Config object and sets values on it.
function configure_renderer(config)
	print("[Lua] Configuring renderer settings...")
	-- Tell the application which shaders to use by name
	config.renderer_type = "scene"

	config.main_shader_name = "default"
	config.canvas_shader_name = "box_test"
end
