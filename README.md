# Lighthouse OpenGL Scene
[![](https://skillicons.dev/icons?i=c,cpp,visualstudio,blender)](https://skillicons.dev)

### Description
This is an OpenGL scene depicting ships passing by the lighthouse and the coast.

### Features
- the user can move a single ship using the W, S, A, and D keys
- the scene contains two light sources that implement a three-component lighting model (ambient, diffuse, and specular)
- objects naturally respond to the lighting
- all objects are textured
- a cube map is used

### Technologies
The project was created using:
- C 17
- C++ 20
- OpenGL 3.3
- GLSL 3.30.6

### Setup
If you have MSBuild on your system, you can run the project with this command:
```
msbuild.exe grk-cw.sln
