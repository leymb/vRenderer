mkdir ..\vRenderer\assets\shaders\compiled

CALL "glslc.exe" ../vRenderer/assets/shaders/fragment_shader.frag -o ../vRenderer/assets/shaders/compiled/fragment_shader.spv
CALL "glslc.exe" ../vRenderer/assets/shaders/vertex_shader.vert -o ../vRenderer/assets/shaders/compiled/vertex_shader.spv

pause