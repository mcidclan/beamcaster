import bpy
import math
import struct
import numpy as np

obj = bpy.context.active_object.data
vertices = obj.vertices

OPT_MODEL_RED_LEVEL = 0.9
OPT_MODEL_GREEN_LEVEL = 1.0
OPT_MODEL_BLUE_LEVEL = 1.5
OPT_MODEL_BRITNESS = 0;

SHELL_DIRECTION = -1
SHELL_SIZE = 2
SHELL_FACTOR = 0.5
SIZE = 256
HALF = SIZE / 2
SHELL_START = 0
i = 00
idx = set()
coords = {}
test = [1.0, 2.0]
a = test[0]
for poly in obj.polygons:
    for id in poly.loop_indices:
        loop = obj.loops[id]
        v = loop.vertex_index
        
        if v not in idx:
            idx.add(v)
            
            shell = SHELL_START
            while shell <= SHELL_SIZE:
                nx = vertices[v].normal.x * shell * SHELL_FACTOR
                ny = vertices[v].normal.y * shell * SHELL_FACTOR
                nz = vertices[v].normal.z * shell * SHELL_FACTOR
                
                x = math.floor(vertices[v].co.x + nx * SHELL_DIRECTION)
                z = math.floor(vertices[v].co.y + ny * SHELL_DIRECTION)
                y = math.floor(-(vertices[v].co.z + nz * SHELL_DIRECTION))
                
                coord = int((x + HALF) + (y + HALF) * SIZE + (z + SIZE) * SIZE * SIZE)
                if (x >= -HALF) and (x < HALF) and (y >= -HALF) and (y < HALF) and (z >= -HALF) and (z < HALF):
                    c = obj.vertex_colors.active.data[i].color
                    
                    r = OPT_MODEL_BRITNESS + math.floor(c[0] * 0x1F * OPT_MODEL_RED_LEVEL)
                    g = OPT_MODEL_BRITNESS + math.floor(c[1] * 0x3F * OPT_MODEL_GREEN_LEVEL)
                    b = OPT_MODEL_BRITNESS + math.floor(c[2] * 0x1F * OPT_MODEL_BLUE_LEVEL)
                    
                    if (r >= 0x1F):
                        r = 0x1F
                    if (g >= 0x3F):
                        g = 0x3F
                    if (b >= 0x1F):
                        b = 0x1F
                    
                    if coord not in coords:
                        coords[coord] = [r, g, b, x, y, z, 1.0]
                    else:
                        _c = coords[coord]
                        _c[0] += r
                        _c[1] += g
                        _c[2] += b
                        _c[6] += 1.0
                        
                shell += 1
        i += 1


f = open("./object_.bin", "wb");
for key in coords:
    coord = coords[key]
    coord[0] /= coord[6]
    coord[1] /= coord[6]
    coord[2] /= coord[6]
    color = int(coord[0]) | (int(coord[1]) << 5) | (int(coord[2]) << 11)
    f.write(struct.pack("<H", color))
    f.write(struct.pack("<b", int(coord[3])))
    f.write(struct.pack("<b", int(coord[4])))
    f.write(struct.pack("<b", int(coord[5])))
    f.write(struct.pack("<b", 1))