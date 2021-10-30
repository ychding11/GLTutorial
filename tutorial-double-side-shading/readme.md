## Overview
It is to demonstrate double side shading. In order to see internal of a cube, cut the cube by a custom clip plane.

- disable back face culling
- tell the front face from back face in pixel shader
- set a custom clip plane.
- enable GL_CLIP_DISTANCE0 by OpenGL call
