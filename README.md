# VulkanRenderer
This renderer was originally created in 2022–2023 as part of my studies. The aim of the project is not to provide a fully-fledged renderer for a game but to help me learn and understand the fundamentals of the Vulkan rendering API. To ensure I actually learned and understand what I was doing and did not simply mindlessly follow the amazing tutorial on by following [vulkan-tutorial.com](https://vulkan-tutorial.com/), I documented my learning process in several PDFs, which are available on my portfolio website or in the PDFs folder of this repository.

### 1. Rendering a Triangle
The first PDF documents my journey from creating a basic Vulkan instance to rendering my first triangle.
You can find the PDF [here](https://github.com/leymb/vRenderer/blob/main/PDFs/Rendering-a-triangle-using-Vulkan-MelvinRother.pdf)

### 2. Vertex Buffers
Since most of what I did to render a triangle in step one was setup, the actual data to render triangles etc. was mostly hard coded. The second PDF documents how I learned to use buffers for in-flight frame synchronization and to supply the GPU with vertex data. You can find the PDF [here](https://github.com/leymb/vRenderer/blob/main/PDFs/Vulkan-Vertex-Buffers-MelvinRother.pdf)

### 3. Texturing
The next step was learning how to texture the triangles. To texture triangles, I learned how to use Vulkan Image Objects and how to sample them. I also implemented Index and Uniform Buffers to avoid wasting memory when uploading data to the GPU and added proper object loading so that the renderer could now display nice objects. You can find the PDF [here](https://github.com/leymb/vRenderer/blob/main/PDFs/Vulkan-Vertex-Buffers-MelvinRother.pdf)


https://github.com/user-attachments/assets/2e15236d-d84d-48d3-b02f-1ccee149dcd0


### 4. Compute Shaders
I was interested in how to use compute shaders in Vulkan, so I learned how to use a separate Compute Pipeline (required since shading and compute pipelines are separate), SSBOs (Shader Storage Buffer Objects) and storage images (images that allow for read- and write operations).



# Acknowledgements
Thank you to Alexander Overvoorde who created [vulkan-tutorial.com](https://vulkan-tutorial.com/). Without his tutorials, I would have not been able to learn this much about Vulkan before giving up completely.
