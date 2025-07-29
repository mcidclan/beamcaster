## BeamCaster Overview  
This experimental project implements a CPU-Side voxel raycasting technique using a beam-based acceleration method. Instead of casting a ray for every pixel, it processes the scene in 8x8 pixel blocks and dynamically adjusts traversal speed and precision after each hit.

A `Beam` here consists of two parts: a coherent group of rays (the "witness rays") that drives the global advancement within the 8×8×LOD block, and a second part in which the full set of rays for the 8×8 block is advanced to scan the space, starting from the point where the witness beam stopped.

## Beam Traversal: Step-by-Step  

- The Beam Caster groups rays into beams to improve efficiency.
- Witness rays are used within each block (defined with a binary mask) to detect voxels, minimizing check counts.

The algorithm operates in three main phases:

1. **Fast traversal phase**  
   Witness rays advance at maximum speed, 8x8 voxels per step (coarse LOD).  
   If no witness ray detects a voxel, the entire 8x8 block is skipped, and fast traversal continues.  
   This repeats until the first hit is detected.

2. **LOD switching phase after the first hit**  
   When a witness ray detects a voxel, the algorithm switches to a higher level of detail.  
   It rolls back slightly and resumes the beam's progression at the new LOD.  
   (Implementing DDA on coarser levels may further improve performance)

3. **Scanning precision at the finest level**  
   When a unit-sized voxel is hit, the algorithm switches to unit precision mode.  
   It resumes from where the accelerated beam previously stopped and scans voxel-by-voxel, processing the full 8x8 block with all rays.

## Loading Voxel Regions  

### Voxel Editor  
To create voxel regions for this project, you can use the following editor:  
[https://github.com/mcidclan/voxelander-voxel-editor](https://github.com/mcidclan/voxelander-voxel-editor)

### Blender Script  
Alternatively, use the Blender 3.x.x Python script available in the `tool` folder to export voxel regions from a voxelized mesh.

## File Format  
You can export multiple voxel files to be loaded by the renderer. Name them sequentially:

```
object\_0.bin
object\_1.bin
object\_2.bin
etc.
```

## Configuration

You can adjust and experiment with the Beamcaster configuration using the `opt.h` file located in the `/bmc` folder.

This file centralizes key parameters such as:

- **Rendering options**: toggle FPS display, fog effects, and visual distortions (barrel, spherical, lens).
- **Camera settings**: field of view, step size, near plane distance.
- **Space and frame size**: dimensions, buffer sizes, and slice configurations.
- **Raycasting behavior**: ray depth, base scale, color depth factor, and FOV factor.
- **Beam parameters**: beam size, bitmask, origin detection, and boost level.
- **2D rendering layout**: grid size, scissor bounds, and vertical displacement.

You can change these constants to explore different visual or performance profiles.


*m-c/d*
