## Overview
It is to demonstrate how to draw wireframe on shaded surface in one pass. The most important step is to tell whether a pixel is on edge of triangle or inside of a triangle. In order to achieve this goal, it can take advantage of vertex attribute interpolation in GPU. The detailed steps is as follow.

- in **geometry shader** stage, add a **none perspective attribute** which represents vertex distance to each edge in screen space. 
- for pixel in edge, there is one height  is 0. 

The above is the main idea. The advantage of this method is :

- It is robust , view independent
- It is able to draw wireframe on shaded surface in a single pass

It requires the GPU is capable of Geometry Shader.

### Height to edge

- picture to demo the height to edges in window space
  ![](.\pic.png)
  
- picture to demo how to do it in geometry shader
  ![](.\height-to-edge.png)

## Screen Shot

![solid-wireframe](..\screen\solid-wireframe.png)



## Reference

- [solid wireframe]( https://developer.download.nvidia.com/whitepapers/2007/SDK10/SolidWireframe.pdf)

