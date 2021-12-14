## Overview
## 

### Stencil Test

- Each [Fragment](https://www.khronos.org/opengl/wiki/Fragment) has a stencil value(an unsigned integer)
- Every fragment from every primitive for every object in a [draw call](https://www.khronos.org/opengl/wiki/Vertex_Rendering) gets the same fragment stencil value 
- All stencil tests and operation functions have two sets of data: 
  - one for the front side
  - one for the back side
  - `void glStencilFuncSeparate(GLenum face, GLenum func, GLint ref, GLuint mask);`
  - `void glStencilOpSeparate(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);`
    - stencil test is before depth test
    - The function specify the stencil buffer update operation under 3 possible cases:
      - stencil fail
      - stencil pass, depth fail
      - stencil pass, depth pass
- The **stencil test** will test current fragment's stencil value against the one in stencil buffer
  - `glEnable(GL_STENCIL_TEST);`  
  - When rendering to a framebuffer without a stencil buffer, stenciling will always behave as test passed
  - `glStencilMask(0xFF);`  stencil buffer write mask if GL_REPLACE is set as stencil operation


## Screen Shot





## Reference

- [Stencil Test - OpenGL Wiki (khronos.org)](https://www.khronos.org/opengl/wiki/Stencil_Test)
- https://www.khronos.org/opengl/wiki/GLAPI/glStencilFuncSeparate
- https://www.khronos.org/opengl/wiki/GLAPI/glStencilOpSeparate
- [glStencilMask (khronos.org)](https://www.khronos.org/registry/OpenGL-Refpages/es2.0/xhtml/glStencilMask.xml)

