# MiniEngine
### A small PBR Game Engine 

This engine is still in active development.

<b>Current Features:</b>
- OpenGL backend, Deferred-only renderer
- PBR support based on Unreal Engine 4's study, including environment-based lighting
- Simple textual API
- Normal mapping
- Basic gltf (glb) loader
- Basic IMGUI support
- FXAA

<b>Planned Features:</b>
- Profiling stats per frame (full frame time available)
- ECS system
- Event Listening System
- Shadows
- Tonemapping via - https://github.com/h3r2tic/tony-mc-mapface
- TAA, ambient occlusion, bloom
- Point-based, area-based lights
- Default texture parameter support in Builders
- Global Illumination
- Vulkan support
<br></br>
---
<br></br>
![img](assets/readme/miniengine-1.png)
![img](assets/readme/miniengine-2.png)
<br></br>
---
<br></br>
## Building instructions

Currently only Windows (MSVC) is supported.

> run build.bat

Build files will be generated in a `build/` folder. This will build the sample `main.cpp` in the `examples/default/` folder to run an example similar to the screenshots posted above.



## API Usage
<i>Documentation is in progress. Will be updated at a later date.</i>

Refer to `examples/default/main.cpp` to get an idea of the basic API usage.
