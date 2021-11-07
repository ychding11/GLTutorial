## Overview
It is to demonstrate msaa.

- `in int gl_SampleMaskIn[]`  read only. A sample bit set if and only if the sample is considered **covered**  for current fragment shader invocation.
  In most case, gl_SampleMaskIn[0] is enough, 32X MSAA is enough.

- `out int gl_SampleMask[]`  specifies the sample coverage mask for the current fragment.

- `in vec2 gl_SamplePosition` contains the location of the current sample within the current fragment. The x and y components of `gl_SamplePosition` contain the sub-pixel coordinate. The sub-pixel coordinates of the center of the pixel are always (0.5, 0.5).

- `in vec4 gl_FragCoord`  contain window-relative coordinates of the current fragment. The z component is the depth value that would be used for the fragment's depth if shader write to [gl_FragDepth](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragDepth.xhtml). It is affected by layout qualifier origin_upper_left ` or ` pixel_center_integer. see [gl_FragCoord](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/gl_FragCoord.xhtml) for

  details.

## Reference

- 

