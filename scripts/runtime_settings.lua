-- This function loads shaders into the ResourceManager so the renderer can find them by name later.
function load_shaders()
	print("[Lua] Loading runtime shaders...")
	ResourceManager.load_shader("default", ShaderType.Graphics, { "shaders/shader.vert", "shaders/shader.frag" })
	ResourceManager.load_shader("canvas", ShaderType.Graphics, { "shaders/canvas.vert", "shaders/canvas.frag" })
	ResourceManager.load_shader("canvas_alt", ShaderType.Graphics, { "shaders/canvas.vert", "shaders/canvas_alt.frag" })
	ResourceManager.load_shader("box_test", ShaderType.Graphics, { "shaders/canvas.vert", "shaders/box_test.frag" })
	ResourceManager.load_shader("compute_test", ShaderType.Compute, { "shaders/texture_compute.comp" })
	ResourceManager.load_shader("draw_texture", ShaderType.Graphics, { "shaders/canvas.vert", "shaders/shader.frag" })
end

-- This function takes the C++ Config object and sets values on it.
function configure_renderer(config)
	print("[Lua] Configuring renderer settings...")
	-- Tell the application which shaders to use by name
	config.renderer_type = "compute"

	config.main_shader_name = "compute_test"
	config.canvas_shader_name = "box_test"
end
