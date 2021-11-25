## Overview
It is to demonstrate how FXAA work. FXAA is a single-pass, screen-space, anti-aliasing technique. It is easy to integration.

Main steps from [FXAA_WhitePaper.pdf](https://developer.download.nvidia.cn/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf) :

- local contrast to avoid non-edge

  ```glsl
  // Luminance Conversion
  float FxaaLuma(float3 rgb) 
  {
  	return rgb.y * (0.587/0.299) + rgb.x; 
  }
  ```

  

- Pixels passing the local contrast test are then classified as horizontal or  vertical

  ```glsl
  // Contrast test
  /*
      FXAA_EDGE_THRESHOLD The minimum amount of local contrast required to apply algorithm.
          1/3 – too little
          1/4 – low quality
          1/8 – high quality
          1/16 – overkill
      FXAA_EDGE_THRESHOLD_MIN Trims the algorithm from processing darks.
          1/32 – visible limit
          1/16 – high quality
          1/12 – upper limit (start of visible unfiltered edges)
  
  */
  float3 rgbN = FxaaTextureOffset(tex, pos.xy, FxaaInt2( 0,-1)).xyz;
  float3 rgbW = FxaaTextureOffset(tex, pos.xy, FxaaInt2(-1, 0)).xyz;
  float3 rgbM = FxaaTextureOffset(tex, pos.xy, FxaaInt2( 0, 0)).xyz;
  float3 rgbE = FxaaTextureOffset(tex, pos.xy, FxaaInt2( 1, 0)).xyz;
  float3 rgbS = FxaaTextureOffset(tex, pos.xy, FxaaInt2( 0, 1)).xyz;
  float lumaN = FxaaLuma(rgbN);
  float lumaW = FxaaLuma(rgbW);
  float lumaM = FxaaLuma(rgbM);
  float lumaE = FxaaLuma(rgbE);
  float lumaS = FxaaLuma(rgbS);
  float rangeMin = min(lumaM, min(min(lumaN, lumaW), min(lumaS, lumaE)));
  float rangeMax = max(lumaM, max(max(lumaN, lumaW), max(lumaS, lumaE)));
  float range = rangeMax - rangeMin;
  if(range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * XAA_EDGE_THRESHOLD)) 
  { /*no need AA, early exit*/ }
  
  ```

  

- Sub-pixel

- Select pixel Vertical / Horizon

  ```glsl
  //< 3x3 kernel, like edge detection
  float edgeVert = 
   abs((0.25 * lumaNW) + (-0.5 * lumaN) + (0.25 * lumaNE)) +
   abs((0.50 * lumaW ) + (-1.0 * lumaM) + (0.50 * lumaE )) +
   abs((0.25 * lumaSW) + (-0.5 * lumaS) + (0.25 * lumaSE));
  float edgeHorz = 
   abs((0.25 * lumaNW) + (-0.5 * lumaW) + (0.25 * lumaSW)) +
   abs((0.50 * lumaN ) + (-1.0 * lumaM) + (0.50 * lumaS )) +
   abs((0.25 * lumaNE) + (-0.5 * lumaE) + (0.25 * lumaSE));
  bool horzSpan = edgeHorz >= edgeVert;
  ```

  

- Search end-of-edge

- Detect sub-pixel aliasing

## Screen Shot





## Reference

- [What Is FXAA, And Why Has It Made Anti-Aliasing As We Know It Obsolete? (nvidia.com)](http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf)
- [OpenGL/fxaa.frag.glsl at master · McNopper/OpenGL (github.com)](https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl)
- [textureOffset - OpenGL 4 Reference Pages (khronos.org)](https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/textureOffset.xhtml)
- [App Note Template (nvidia.cn)](https://developer.download.nvidia.cn/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf)

