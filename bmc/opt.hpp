// Configuration file for a frame size of 512
#pragma once

#define OPT_SHOW_FPS                            1
#define OPT_USE_COLLIDE                         1

// Camera parameters
#define OPT_POV_RANGE                           90
#define OPT_CAM_STEP                            1.0f

// Space size parameters
#define OPT_SPACE_SIZE                          256
#define OPT_SPACE_SIZE_X2                       512
#define OPT_SPACE_SLICE_SIZE                    65536
#define OPT_SPACE_BUFFER_SIZE                   16777216

// Frame size parameters
#define OPT_HALF_FRAME_SIZE                     256
constexpr unsigned int OPT_FRAME_SIZE        =  OPT_HALF_FRAME_SIZE * 2; // <<
constexpr unsigned int OPT_FRAME_SIZE_X2     =  OPT_FRAME_SIZE * 2;
constexpr unsigned int OPT_FRAME_SIZE_X3     =  OPT_FRAME_SIZE * 3;
constexpr unsigned int OPT_FRAME_SIZE_X4     =  OPT_FRAME_SIZE * 4;
constexpr unsigned int OPT_FRAME_SIZE_X5     =  OPT_FRAME_SIZE * 5;
constexpr unsigned int OPT_FRAME_SIZE_X6     =  OPT_FRAME_SIZE * 6;
constexpr unsigned int OPT_FRAME_BUFFER_SIZE =  OPT_FRAME_SIZE * OPT_FRAME_SIZE;

// Fog
#define OPT_USE_FOG                             1
#define OPT_FOG_RED_INTENSITY                   24.94f
#define OPT_FOG_GREEN_INTENSITY                 43.0f
#define OPT_FOG_BLUE_INTENSITY                  42.0f

// Ray behavior
#define OPT_MAX_RAY_DEPTH                       104.0f
#define OPT_USE_NEAR_ADJUSTER                   1
#define OPT_NEAR_ADJUSTER                       0.0033f
#define OPT_NEAR                                96.0f
#define OPT_RAY_LENGTH_INFLUENCE                0.0056f
#define OPT_RAY_PROJECTION_BASE                 0.025f
#define OPT_RAY_COLOR_DEPTH_FACTOR              4.0f

// Beam parameters 64 pixels
// #define OPT_BEAM_MASK                           0xFFFFFFFFFFFFFFFF
// #define OPT_COLOR_CHECKER_ENCODING(x)           ((u64)x)
// #define OPT_NUMBER_OF_RAY_PER_BEAM              64
// #define OPT_NUMBER_OF_ORIGIN_TO_FIND            4
// #define OPT_ORIGIN_MASK 0b0000100000000000000000000000000110000000000000000000000000010000

// Beam parameters 32 pixels
#define OPT_BEAM_MASK                           0xFFFFFFFF
#define OPT_COLOR_CHECKER_ENCODING(x)           ((u32)x)
#define OPT_NUMBER_OF_RAY_PER_BEAM              32
#define OPT_NUMBER_OF_ORIGIN_TO_FIND            4
#define OPT_ORIGIN_MASK                         0b00001000000000011000000000010000

#define OPT_BOOST_LEVEL                         7

// 2d rendering
#define OPT_GRID_WIDTH                          8
#define OPT_GRID_HEIGHT                         8
#define OPT_H_SCISSOR                           16
#define OPT_V_SCISSOR_START                     0 // 4
#define OPT_V_SCISSOR_END                       0 // 4
#define OPT_V_DISPLACEMENT                      0 // -32
