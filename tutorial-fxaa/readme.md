## Overview
It is to demonstrate how fxaa work. FXAA is a single-pass, screen-space, anti-aliasing technique. It is easy to integration.

Main steps from [FXAA_WhitePaper.pdf](https://developer.download.nvidia.cn/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf) :

- local contrast to avoid non-edge
- Pixels passing the local contrast test are then classified as horizontal or  vertical
- Select pixel
- Search end-of-edge
- Detect sub-pixel aliasing

## Screen Shot





## Reference

- [What Is FXAA, And Why Has It Made Anti-Aliasing As We Know It Obsolete? (nvidia.com)](http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf)
- [OpenGL/fxaa.frag.glsl at master · McNopper/OpenGL (github.com)](https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl)
- [textureOffset - OpenGL 4 Reference Pages (khronos.org)](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/textureOffset.xhtml)
- [App Note Template (nvidia.cn)](https://developer.download.nvidia.cn/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf)

