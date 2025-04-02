### **Overview**  
This project implements a voxel raycasting technique using beam-based acceleration. Instead of casting a ray for every pixel, it processes the scene in 8x8 pixel blocks and dynamically adjusts its traversal speed.

### **How It Works**  
- The Beam Caster groups rays into a beam to improve efficiency.  
- Witness rays are used within each block to detect voxels while minimizing unnecessary checks.  
- If no voxels are found, the algorithm skips ahead to accelerate traversal.  
- Once a voxel is detected, it switches to high-precision mode, scanning unit-sized voxels step by step.
