## Overview
It is to demonstrate MSAA.

Render scene into a MSAA capable render target. First, create a multiple sample capable render texture or render buffer object.

1. Create multiple sample 2D texture.

    ```c++
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex); 
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGB, width, height, GL_TRUE); 
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0); 
    ```

2. Attach it to frame buffer object's attach point

   ```c++
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tex, 0); 
   ```
   
3. draw() as usual

4. resolve MSAA render target : set source fbo, set dst fbo, blit
   
    ```c++
    glBindFramebuffer(GL_READ_FRAMEBUFFER, multisampledFBO); // read from msaa render target
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // default frame buffer as dst frame buffer 
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST); // only color attachment resolved
    ```

### GLSL built in variables 

- `in int gl_SampleMaskIn[]`  read only. A sample bit set if and only if the sample is considered **covered**  for current fragment shader invocation.
  In most case, gl_SampleMaskIn[0] is enough, 32X MSAA is enough.

- `out int gl_SampleMask[]`  specifies the sample coverage mask for the current fragment.

- `in vec2 gl_SamplePosition` contains the location of the current sample within the current fragment. The x and y components of `gl_SamplePosition` contain the sub-pixel coordinate. The sub-pixel coordinates of the center of the pixel are always (0.5, 0.5).

- `in vec4 gl_FragCoord`  contain window-relative coordinates of the current fragment. The z component is the depth value that would be used for the fragment's depth if shader write to [gl_FragDepth](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragDepth.xhtml). It is affected by layout qualifier origin_upper_left ` or ` pixel_center_integer. see [gl_FragCoord](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml) for

  details.

## Reference

- [Renderbuffer Object - OpenGL Wiki (khronos.org)](https://www.khronos.org/opengl/wiki/Renderbuffer_Object)
- [glBlitFramebuffer - OpenGL 4 Reference Pages (khronos.org)](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBlitFramebuffer.xhtml)

