## Overview
In real world, light scatters in all possible directions with varying intensities so the indirectly lit parts of an object would have varying intensities. **Ambient Occlusion(AO)** is such an type of **indirect lighting approximation** which tries to approximate indirect lighting by darkening creases, holes, and surfaces that are close to each other. These areas are largely occluded by surrounding surfaces, hence the areas would appear darker. 

Ambient occlusion techniques are expensive as they take surrounding geometry into account

- shoot a large number of rays to determine its amount of occlusion
- it is computationally infeasible for real-time solutions

In 2007, Crytek published a technique called screen-space ambient occlusion (SSAO) 

- uses a scene's depth buffer in screen-space to estimate the amount of occlusion.
- it is incredibly fast compared to real ambient occlusion 
  - gives plausible results, making it the actual standard for approximating real-time ambient occlusion
- For each fragment 
  - First, calculate an **occlusion factor** based on the fragment's surrounding depth values. 
  - The **occlusion factor** is then used to adjust the fragment's ambient lighting component. 
  - The **occlusion factor** is obtained by following steps :
    - Take multiple depth samples in a sphere sample kernel surrounding current fragment position 
    - Compare each of the samples with the current fragment's depth value
    - The number of samples that have a higher depth value than the fragment's depth is occlusion factor
- How to select surrounding samples is trick. It determines the tradeoff between quality and speed

## SSAO Steps in this demo

![](.\doc\ssao-step.png)


## **Debug Output**

- It is enabled by using [glEnable](https://www.khronos.org/opengl/wiki/GLAPI/glEnable) with the GL_DEBUG_OUTPUT on [Debug Contexts](https://www.khronos.org/opengl/wiki/Debug_Context).

- Message format : 

  - message source
    - GL_DEBUG_SOURCE_API
    - GL_DEBUG_SOURCE_WINDOW_SYSTEM
    - GL_DEBUG_SOURCE_SHADER_COMPILER
  - message type
    - GL_DEBUG_TYPE_ERROR
    - GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR
    - GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR
    - GL_DEBUG_TYPE_PERFORMANCE
  - message severity 
    - GL_DEBUG_SEVERITY_HIGH
    - GL_DEBUG_SEVERITY_MEDIUM
    - GL_DEBUG_SEVERITY_LOW
  - ID
  - string describing the message

- API to generate messages

  - `glDebugMessageInsert(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message)`
  - Messages, once generated, can either be stored in a log or passed directly to the application via a callback function
    - If a callback is registered, the messages prefer callback
    - If no callback is registered, then messages are stored in a log. If log is full and more messages are generated, then the new messages will be discarded
  - `void glDebugMessageControl(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);`
    - The function is to be able to filter out certain messages
  - Each context maintains its own message log queue for commands executed in that context

- APIs to group a sequence of GL Commands to define an activity

  ```c
  
  void glPushDebugGroup(GLenum source, GLuint id, GLsizei length, const char * message);
  
  void glPopDebugGroup(void);
  ```


## SSAO Texture Visualization

- Problems : Default framebuffer format is RGBA. But ssao texture is a single channel texture.
- Solution : It is a workaround, duplicate ssao in RGB channels.

## Reference

- https://learnopengl.com/Advanced-Lighting/SSAO
- http://john-chapman-graphics.blogspot.nl/2013/01/ssao-tutorial.html
- https://www.khronos.org/opengl/wiki/Debug_Output#User_messages
- 

