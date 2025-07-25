## Overview  
This experimental project implements a voxel raycasting technique using a beam-based acceleration. Instead of casting a ray for every pixel, it processes the scene in 8x8 pixel blocks and dynamically adjusts its traversal speed after the first hit.

## How It Works  
- The Beam Caster groups rays into beams to improve efficiency
- Witness rays are used within each block (defined with a binary mask) to detect voxels, minimizing check counts
- If no voxels are found, the algorithm skips the full 8x8 block ahead to accelerate traversal
- Once a voxel is detected, it switches to high-precision mode, scanning unit-sized voxels step by step

## Voxel Editor
To create voxel regions for this project, you can use the following editor: https://github.com/mcidclan/voxelander-voxel-editor

## File Format
You can export multiple voxel files to be loaded by the renderer. Make sure to name them sequentially:
```
object_0.bin
object_1.bin
object_2.bin
etc.
```

*m-c/d*
