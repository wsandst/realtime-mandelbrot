# Realtime Mandelbrot using OpenGL Compute Shaders in C++ with SDL2 
This is a realtime low-precision Mandelbrot navigator, utilizing the GPU through OpenGL Compute Shaders to deliver >30 FPS. It supports a few coloring methods and has a few presets built in, which can be cycled through with the hotkeys M and N. You cannot zoom very far due to precision issues with floats. Increased precision is being worked on.  

## Images
![Red Mandelbrot](https://i.ibb.co/WG7XyfV/20200820-114612.png)  
![Purple Mandelbrot](https://i.ibb.co/4jKw77w/20200820-114716.png)  

## Dependencies
**SDL2** - for window and input managment  
**GLEW** (static library) - for OpenGL context  
**GLM** - for matrix/vector math  
**FreeType** - for font bitmap creation   
Included in the `external/` folder using git submodules:  
**stb_image, stb_write, tinyobjloader**
  
## Build instructions  
1. Install the required dependencies: [SDL2](https://www.libsdl.org/download-2.0.php), [GLEW](http://glew.sourceforge.net/), [GLM](https://glm.g-truc.net/0.9.9/index.html), [FreeType](https://www.freetype.org/download.html) (`apt-get install libsdl2-dev, libglm-dev, libglew-dev` installs all except FreeType. Visit FreeType website for download)
2. Clone the repo and the submodules (`git submodule update --init --recursive`)
3. Run cmake (`cmake -DCMAKE_BUILD_TYPE=Release ..` in the folder `opengl-starter/build/`)
4. You can now compile/modify the project 

## Controls
**WSAD** - Movement  
**Scroll** Zoom in/out  
**M** - Go forward in color presets  
**N** - Go backward in color presets  
**E** - Increase iterations  
**Q** - Decrease iterations  
**Escape** - Unfocus window  
**F1** - Toggle text rendering  
**F2** - Take screenshot  
**F3** - Toggle debug menu, contains Mandelbrot info  
**F11** - Toggle fullscreen  
**R** - Hot reload shaders  
**C** - Quit the program  
**Z/X** - Increase/Decrease max speed  