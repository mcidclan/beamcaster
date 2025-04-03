#pragma once
#define OPT_POV_RANGE                           90

#define OPT_VOXEL_TYPE_MASK                     0b1111111111011111
#define OPT_FADE_BEAM_COLOR                     0
#define OPT_MAX_RAY_DEPTH                       64.0f


#define OPT_CAM_STEP                            1.5f
#define OPT_SHOW_FPS                            1

//
#define OPT_SPACE_SIZE                          256
#define OPT_SPACE_SIZE_X2                       512
#define OPT_SPACE_SLICE_SIZE                    65536
#define OPT_SPACE_BUFFER_SIZE                   16777216

#define OPT_HALF_FRAME_SIZE                     256
constexpr unsigned int OPT_FRAME_SIZE        =  OPT_HALF_FRAME_SIZE * 2; // <<
constexpr unsigned int OPT_FRAME_SIZE_X2     =  OPT_FRAME_SIZE * 2;
constexpr unsigned int OPT_FRAME_SIZE_X3     =  OPT_FRAME_SIZE * 3;
constexpr unsigned int OPT_FRAME_SIZE_X4     =  OPT_FRAME_SIZE * 4;
constexpr unsigned int OPT_FRAME_SIZE_X5     =  OPT_FRAME_SIZE * 5;
constexpr unsigned int OPT_FRAME_SIZE_X6     =  OPT_FRAME_SIZE * 6;
constexpr unsigned int OPT_FRAME_BUFFER_SIZE =  OPT_FRAME_SIZE * OPT_FRAME_SIZE;

//
#define OPT_USE_FOG                             1
#define OPT_FOG_RED_INTENSITY                   24.94f /*19.952f*/
#define OPT_FOG_GREEN_INTENSITY                 43.0f /*34.4f*/
#define OPT_FOG_BLUE_INTENSITY                  42.0f /*40.0f*/  /*32.0f*/ /*38.7f*/

// Ray behavior
#define OPT_USE_NEAR_ADJUSTER                   1
#define OPT_NEAR_ADJUSTER                       0.006f
#define OPT_NEAR                                32.0f
#define OPT_SCALE_ORIGIN                        1.0f
#define OPT_SCALE_X_ORIGIN                      1.0f
#define OPT_SCALE_Y_ORIGIN                      1.0f
#define OPT_RAY_LENGTH_INFLUENCE                0.008f
#define OPT_RAY_PROJECTION_BASE                 0.0f
#define OPT_RAY_BASE                            0.5f


#define OPT_USE_COLLIDE                         1

#define OPT_BEAM_MASK                           /*0b01010101101010100101010110101010*/ 0xFFFFFFFF /*0xFFFFFFFFFFFFFFFF*/ /*64*/
#define OPT_COLOR_CHECKER_ENCODING(x)           ((u32)x) /*((u64)x)*/
#define OPT_NUMBER_OF_RAY_PER_BEAM              32 /*64*/
#define OPT_NUMBER_OF_ORIGIN_TO_FIND            /*6*/ 4

// #define OPT_ORIGIN_MASK 0b1000000100000000001001000000000000000000001001000000000010000001 /*8*/
// #define OPT_ORIGIN_MASK 0b1000000100000000001000000000000000000000000001000000000010000001 /*6*/
// #define OPT_ORIGIN_MASK 0b0000100000000000000000000000000110000000000000000000000000010000 /*4 64 bits*/
#define OPT_ORIGIN_MASK 0b00001000000000011000000000010000 /*4 32 bits*/

//
#define OPT_BOOST_FOUND                         6
#define OPT_MIN_LINEAR_LEVEL                    6 /*/!\*/
#define OPT_RAY_COLOR_DEPTH_FACTOR              4.0f
// build world
#define OPT_VOXEL_FADE                          0.05f
#define OPT_USE_VOXEL_FADE_DEPTH_ADJUSTER       1
#define OPT_VOXEL_FADE_DEPTH_ADJUSTER_COLOR     0x0
//
#define OPT_GRID_WIDTH                          8
#define OPT_GRID_HEIGHT                         8
#define OPT_H_SCISSOR                           18

#ifdef _WIN32
#define OPT_V_SCISSOR_START                     0 /*16*/
#define OPT_V_SCISSOR_END                       0 /*16*/
#define OPT_V_DISPLACEMENT                      0
#else
#define OPT_V_SCISSOR_START                     4 /*16*/
#define OPT_V_SCISSOR_END                       4 /*16*/
#define OPT_V_DISPLACEMENT                      -32
#endif
