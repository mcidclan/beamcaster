#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <cstdlib>

#include "../bmc/bmc.hpp"

struct InputState {
  bool up, down, left, right;
  bool triangle, cross, circle, square;
} inputState;

GLFWwindow* window;
static float dstep = OPT_CAM_STEP;
static ucb* frameBuffer = nullptr;

static u8 canMove(const v3* const p) {
  if (p->x < 0 || p->x >= OPT_SPACE_SIZE ||
      p->y < 0 || p->y >= OPT_SPACE_SIZE ||
      p->z < 0 || p->z >= OPT_SPACE_SIZE) {
    return 1;
  }
  static u32 offset;
  static ucb color;
  offset = ((i16)(p->x) >> bmc::_.space->rlevel) |
           (((i16)(p->y) >> bmc::_.space->rlevel) << bmc::_.space->yBitOffset) |
           (((i16)(p->z) >> bmc::_.space->rlevel) << bmc::_.space->zBitOffset);
  color = bmc::_.space->region[offset];
  return (color & 1 << 5) || color == 0;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);

  if (key == GLFW_KEY_ESCAPE && pressed)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (key == GLFW_KEY_UP) inputState.up = pressed;
  if (key == GLFW_KEY_DOWN) inputState.down = pressed;
  if (key == GLFW_KEY_LEFT) inputState.left = pressed;
  if (key == GLFW_KEY_RIGHT) inputState.right = pressed;

  if (key == GLFW_KEY_W) inputState.triangle = pressed;
  if (key == GLFW_KEY_S) inputState.cross = pressed;

  if (key == GLFW_KEY_D) inputState.circle = pressed;
  if (key == GLFW_KEY_A) inputState.square = pressed;
}

static inline void getView(ucb* const frame) {
  static u8 useTool = 0;
  static float ax = OPT_POV_RANGE / 20.0f;
  static float ay = OPT_POV_RANGE / 2.0f;
  static v3 position = {131.0f, 191.0f, 181.0f};
  static v3 _position = position;
  static v3 _iposition;

  SpacePov pov = {0.0f, ax, ay, position};

  useTool = 0;
  if (inputState.circle) useTool = 0b001;
  if (inputState.square) useTool = 0b010;

  if (inputState.triangle) {
    _iposition.x = position.x;
    _position.x = position.x + bmc::_.rayStep.x * dstep;
    _iposition.y = position.y;
    _position.y = position.y + bmc::_.rayStep.y * dstep;
    _iposition.z = position.z;
    _position.z = position.z + bmc::_.rayStep.z * dstep;
  }

  if (inputState.cross) {
    _iposition.x = position.x;
    _position.x = position.x - bmc::_.rayStep.x * dstep;
    _iposition.y = position.y;
    _position.y = position.y - bmc::_.rayStep.y * dstep;
    _iposition.z = position.z;
    _position.z = position.z - bmc::_.rayStep.z * dstep;
  }

  bmc::_.collide = 0;
  if (canMove(&_iposition) && canMove(&_position)) {
    position = _position;
  } else {
    bmc::_.collide = 1;
  }

  if (inputState.left) ay = (ay - 1) < 0 ? OPT_POV_RANGE - 1 : ay - 1;
  if (inputState.right) ay = (ay + 1) >= OPT_POV_RANGE ? 0 : ay + 1;
  if (inputState.down) ax = (ax - 1) < 0 ? OPT_POV_RANGE - 1 : ax - 1;
  if (inputState.up) ax = (ax + 1) >= OPT_POV_RANGE ? 0 : ax + 1;

  bmc::_.cameraUsed = inputState.cross || inputState.triangle || inputState.left ||
                      inputState.right || inputState.up || inputState.down;

  bmc::getRendering(frame, &pov, useTool);
}

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec2 iPosition;
layout(location = 1) in vec2 iCoordinates;
out vec2 coordinates;
void main() {
  gl_Position = vec4(iPosition, 0.0, 1.0);
  coordinates = iCoordinates;
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
in vec2 coordinates;
out vec4 fragment;
uniform sampler2D sampler;
void main() {
  vec3 color = texture(sampler, coordinates).rgb;
  fragment = vec4(color.bgr, 1.0);
}
)";

void checkShaderCompile(GLuint shader, const char* type) {
  GLint success;
  char infoLog[512];
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    fprintf(stderr, "Shader %s compile error:\n%s\n", type, infoLog);
  }
}

void checkProgramLink(GLuint program) {
  GLint success;
  char infoLog[512];
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program, 512, NULL, infoLog);
    fprintf(stderr, "Program link error:\n%s\n", infoLog);
  }
}

int main() {
  fprintf(stderr, "Starting...\n");

  if (!glfwInit()) {
    fprintf(stderr, "GLFW initialization failed\n");
    return -1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, 0);

  window = glfwCreateWindow(OPT_FRAME_SIZE, OPT_FRAME_SIZE, "Beamcaster GLFW", NULL, NULL);
  if (!window) {
    fprintf(stderr, "Failed to create window\n");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);
  glfwSetKeyCallback(window, key_callback);

  glewExperimental = GL_TRUE;
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "GLEW initialization failed\n");
    glfwTerminate();
    return -1;
  }

  frameBuffer = (ucb*)memalign(16, OPT_FRAME_SIZE * OPT_FRAME_SIZE * 2);
  if (!frameBuffer) {
    fprintf(stderr, "Failed to allocate framebuffer\n");
    glfwTerminate();
    return -1;
  }

  bmc::init();
  pov::init();

  glViewport(0, 0, OPT_FRAME_SIZE, OPT_FRAME_SIZE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  checkShaderCompile(vertexShader, "vertex");

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  checkShaderCompile(fragmentShader, "fragment");

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  checkProgramLink(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLuint textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_2D, textureID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 2);

  float vertices[] = {
    -1.0f, -1.0f, 0.0f, 1.0f,
     1.0f, -1.0f, 1.0f, 1.0f,
     1.0f,  1.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f
  };
  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  auto lastTime = std::chrono::high_resolution_clock::now();
  int frameCount = 0, fps = 0;

  while (!glfwWindowShouldClose(window)) {
    frameCount++;
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

    if (elapsedTime >= 1000) {
      fps = frameCount;
      frameCount = 0;
      lastTime = currentTime;
      char title[50];
      sprintf(title, "Beamcaster GLFW - FPS: %d", fps);
      glfwSetWindowTitle(window, title);
    }

    getView(frameBuffer);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, OPT_FRAME_SIZE, OPT_FRAME_SIZE, 0,
                 GL_RGB, GL_UNSIGNED_SHORT_5_6_5, frameBuffer);

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
  glDeleteTextures(1, &textureID);
  free(frameBuffer);
  glfwDestroyWindow(window);
  glfwTerminate();

  fprintf(stderr, "Exited cleanly\n");
  return 0;
}
