## Overview
It is to demonstrate off screen rendering and post processing in pixel shader. It has the following steps:

- Render scene into a frame buffer with color attachment.
- Copy the color attachment in previous step into "display texture" which is the default frame buffer provided by glfw.
- The copy is implemented as a full screen draw. 
- The post processing can happen in fragment shader in copy.

## Screen Shot

![](..\screen\post-processing.png)



## Reference

- [full screen triangle in OpenGL](https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/)

