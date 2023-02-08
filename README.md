# Lighthouse OpenGL Scene
[![](https://skills.thijs.gg/icons?i=c,cpp,visualstudio,blender)](https://skills.thijs.gg)

### Description
This is an OpenGL project depicting a scene in which ships pass by the coast and a lighthouse.

### Features
- user have ability to move one ship with the keys W,S,A,D
- scene contains two light sources implementing a three-component light model (ambient, diffuse and specular)
- objects naturally respond to light
- all objects have textures
- cube map is used

### Technologies
The project was created with:
- C 17
- C++ 20
- OpenGL 3.3
- GLSL 3.30.6

### Setup
If you have MSBuild on your system, you can run the project with this command:
```
msbuild.exe grk-cw.sln
