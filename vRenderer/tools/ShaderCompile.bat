mkdir ..\assets\shaders\compiled

CALL "glslc.exe" ../assets/shaders/fragment_shader.frag -o ../assets/shaders/compiled/fragment_shader.spv
CALL "glslc.exe" ../assets/shaders/vertex_shader.vert -o ../assets/shaders/compiled/vertex_shader.spv
CALL "glslc.exe" ../assets/shaders/compute_shader.comp -o ../assets/shaders/compiled/compute_shader.spv
CALL "glslc.exe" ../assets/shaders/compute_shader.vert -o ../assets/shaders/compiled/compute_shader_vert.spv
CALL "glslc.exe" ../assets/shaders/compute_shader.frag -o ../assets/shaders/compiled/compute_shader_frag.spv

pause