## Overview
It is to demonstrate instance draw which is used to draw a group of similar objects.

- an independent vertex stream for instance. 
- set instance frequency for that stream.
- vertex shader would have a special input value: **gl_InstanceID**

The API is :

```c
void glDrawArraysInstanced( GLenum mode, GLint first,
   GLsizei count, GLsizei instancecount );
```

```c
void glDrawElementsInstanced( GLenum mode, GLsizei count, 
   GLenum type, const void *indices, GLsizei instancecount );
```

It behaviors like :

```c
 if ( mode or count is invalid )
        generate appropriate error
    else {
        for (int i = 0; i < instancecount ; i++) 
        {
            instanceID = i;
            glDrawArrays(mode, first, count);
        }
        instanceID = 0;
    }
```

From OpenGL 4.2, **baseinstance** is supported. **gl_InstanceID** can begin from **baseinstance**.

```c
void glDrawArraysInstancedBaseInstance( GLenum mode, GLint first,
   GLsizei count, GLsizei instancecount, GLuint baseinstance );

 void glDrawElementsInstancedBaseInstance( GLenum mode, GLsizei count, 
   GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance );
```

## Reference

- [OpenGL wiki](https://www.khronos.org/opengl/wiki/Vertex_Rendering#Instancing)

