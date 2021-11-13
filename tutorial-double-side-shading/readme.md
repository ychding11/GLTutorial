## Overview
It is to demonstrate double side shading. In order to see internal of a cube, cut the cube by a custom clip plane.

- disable back face culling by OpenGL API to keep back facing triangle.
- distinguish the front face from back face in pixel shader, shade them in different way.
- set a custom clip plane in vertex shader.
- enable GL_CLIP_DISTANCE0 by OpenGL call

## Screen Shot

![](..\screen\double-side-shading.png)

## Reference

- 

