## Overview
It is to demonstrate how to draw wireframe on shaded surface in one pass. The key is to tell edge in pixel shader.

- in geometry shader stage, add a none perspective attribute to represent distance to opposite edge in screen space. 
- for edge, there is one channel (one height in this case) near 0. 

## Reference

- [solid wireframe]( https://developer.download.nvidia.com/whitepapers/2007/SDK10/SolidWireframe.pdf)

