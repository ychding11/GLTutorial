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


## Screen Shot





## Reference

- https://learnopengl.com/Advanced-Lighting/SSAO

