cmake_minimum_required(VERSION 3.15)

add_executable(model-viewer)

target_sources(model-viewer
PRIVATE
	"main.cc"
	"context/window.cc"
	"control/orbit_camera.cc"
	"control/fly_through_camera.cc"
	"wrapper/framebuffer.cc"
	"wrapper/render_buffer.cc"
	"wrapper/mesh.cc"
	"wrapper/primitive.cc"
	"wrapper/quad.cc"
	"wrapper/shader.cc"
	"wrapper/vertex_array.cc"
	"wrapper/vertex_buffer.cc"
	"wrapper/texture.cc"
	"wrapper/sampler.cc"
	"wrapper/gltf_file_resource.cc"
	"wrapper/wrapper_utils.cc"
	"wrapper/cube.cc"
	"renderers/pbr/deferred_pass.cc"
	"renderers/pbr/pbr_pass.cc"
	"renderers/pbr/blur_pass.cc"
	"renderers/pbr/bloom_pass.cc"
	"renderers/pbr/ssao_pass.cc"
	"renderers/pbr/deferred_debug_pass.cc"
	"renderers/pbr/ibl_baking.cc" 
	"renderers/pbr/pbr_renderer.cc" 
	"renderers/gooch/gooch_pass.cc"
	"renderers/gooch/gooch_renderer.cc"
	"panels/ssao_panel.cc" 
	"panels/bloom_panel.cc" 
	"panels/ibl_panel.cc" 
	"panels/scene_panel.cc"
	"panels/gooch_panel.cc"
	"panels/phong_panel.cc"
	"renderers/phong/phong_pass.cc"
	"renderers/phong/phong_renderer.cc"
)

target_compile_features(model-viewer PUBLIC cxx_std_17)

target_link_libraries(model-viewer
PRIVATE
	project_options
	glm glfw glad stb_image cppcodec imgui glTF-reader portable_file_dialogs)