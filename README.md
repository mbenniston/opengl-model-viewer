# OpenGL Model Viewer

A realtime graphics engine written in C++ to learn about physically based rendering. 

Implements various shading algorithms. This is not written for maximum performance as it just focuses on 
implementing the algorithms from scratch.

Features:

- Physically based rendering
	- Deferred shading
	- With image based environment lighting
	- Derived from [learnopengl](https://learnopengl.com/)'s PBR articles
- SSAO
- Physically based bloom
- glTF 2.0 model loading and rendering (using my [glTF-reader](https://github.com/mbenniston/glTF-reader) library) 
- Gooch shading (with outlines)
- Phong shading
- Flythrough and orbit camera modes

## Demo gifs

(Compressed for github, see demo video for better quality)

### Helmet PBR

![Helmet PBR](docs/gifs/helmet-pbr.gif)

### Helmet Gooch

![Helmet Gooch](docs/gifs/helmet-gooch.gif)

### Helmet Phong

![Helmet Phong](docs/gifs/helmet-phong.gif)

## Demo video

[Demo video](https://youtu.be/1_Y9VX40DXM)

## Screenshots

### Damaged helmet

![Helmet PBR](/docs/screenshots/helmet-pbr.jpg)
![Helmet Gooch](/docs/screenshots/helmet-gooch.jpg)
![Helmet Phong](/docs/screenshots/helmet-phong.jpg)

### Marble bust

![Bust PBR](/docs/screenshots/bust-pbr.jpg)
![Bust Gooch](/docs/screenshots/bust-gooch.jpg)
![Bust Phong](/docs/screenshots/bust-phong.jpg)

### Sponza

![Sponza PBR](/docs/screenshots/sponza-pbr.jpg)
![Sponza Gooch](/docs/screenshots/sponza-gooch.jpg)
![Sponza Phong](/docs/screenshots/sponza-phong.jpg)

## Sources

Learning sources:

- [learnopengl](https://learnopengl.com/)
- [gooch shading](https://en.wikipedia.org/wiki/Gooch_shading)
- [gltf specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html)
- [physically based rendering: from theory to implementation](https://www.pbr-book.org/)

Third party assets:

- [Polyhaven HDRIs](https://polyhaven.com/hdris)
- [Polyhaven Models](https://polyhaven.com/models)
- [glTF-Sample-Models](https://github.com/KhronosGroup/glTF-Sample-Models) 

Libraries used:

- [glfw](https://www.glfw.org/)
- [glad](https://glad.dav1d.de/)
- [glm](https://github.com/g-truc/glm)
- [stb_image](https://github.com/nothings/stb)
- [ImGui](https://github.com/ocornut/imgui)
- [nlohmann json](https://github.com/nlohmann/json)
- [cppcodec](https://github.com/tplgy/cppcodec)
- [portable file dialogs](https://github.com/samhocevar/portable-file-dialogs)

## Features to add

Potential features to add next:

- Different types of lights
- Transparency
- Normal mapping
- Animations
- Optimisation
- Outdoor environments with suns 
- Anti aliasing
