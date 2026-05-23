#include "dv_glfw.h"

static bool KEEPGOING;

static const char* vertex_shader_src =
  "#version 330 core\n"
  "layout (location = 0) in vec2 aPos;\n"
  "layout (location = 1) in vec2 aTex;\n"
  "out vec2 texCoord;\n"
  "void main() {\n"
  "    texCoord = aTex;\n"
  "    gl_Position = vec4(aPos, 0.0, 1.0);\n"
  "}\n";

static const char* fragment_shader_src =
  "#version 330 core\n"
  "in vec2 texCoord;\n"
  "out vec4 FragColor;\n"
  "uniform sampler2D screenTex;\n"
  "void main() {\n"
  "    FragColor = texture(screenTex, texCoord);\n"
  "}\n";

static float QUAD[] = {  -1, -1,  0,  1,
                          1, -1,  1,  1,
                         -1,  1,  0,  0,
                          1,  1,  1,  0   };

GLuint compile_shader(GLenum type, const char* src) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &src, NULL);
  glCompileShader(shader);
  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char info[512];
    glGetShaderInfoLog(shader, 512, NULL, info);
    fprintf(stderr, "Shader compile error: %s\n", info);
    exit(1);
  }
  return shader;
}

GLuint create_shader_program(const char* vsrc, const char* fsrc) {
  GLuint vsh = compile_shader(GL_VERTEX_SHADER, vsrc);
  GLuint fsh = compile_shader(GL_FRAGMENT_SHADER, fsrc);
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vsh);
  glAttachShader(prog, fsh);
  glLinkProgram(prog);
  int success;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    char info[512];
    glGetProgramInfoLog(prog, 512, NULL, info);
    fprintf(stderr, "Program link error: %s\n", info);
    exit(1);
  }
  glDeleteShader(vsh);
  glDeleteShader(fsh);
  return prog;
}

static GLFWwindow *WINDOW;
static GLuint      SHADER;
static GLuint      SCRTEX;
static GLuint      VAO,VBO;

static dv_fb_t    *SCREEN;

dv_fb_t *dv_glfw_init(void){
  if (!glfwInit()) {
    fprintf(stderr, "Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  WINDOW = glfwCreateWindow(960, 640, "DV_GLFW_WINDOW", NULL, NULL);
  glfwMakeContextCurrent(WINDOW);

  if(!WINDOW){ 
    fprintf(stderr, "Failed to create GLFW window.\n");
    glfwTerminate();
    return NULL;
  }

  int glver = gladLoadGL(glfwGetProcAddress);
  if (glver == 0) {
    fprintf(stderr, "Failed to initialize OpenGL context\n");
    return NULL;
  }

  glfwSwapInterval(0);

  SHADER = create_shader_program(vertex_shader_src, fragment_shader_src);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), QUAD, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
  glEnableVertexAttribArray(1);

  SCREEN = dv_fb_create(VIRTUAL_SCREEN_W, VIRTUAL_SCREEN_H);

  glGenTextures(1, &SCRTEX);
  glBindTexture(GL_TEXTURE_2D, SCRTEX);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREEN->w, SCREEN->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, SCREEN->pixels);

  KEEPGOING = true;
  glfwSetTime(0.0);

  return SCREEN;
}

// TODO: There should be something maintaining aspect ratio.

void dv_glfw_draw_and_swap(){
  static int ww, wh;
  glfwGetFramebufferSize(WINDOW, &ww, &wh);
  glViewport(0, 0, ww, wh);

  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, SCRTEX);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN->w, SCREEN->h, GL_RGBA, GL_UNSIGNED_BYTE, SCREEN->pixels);

  glUseProgram(SHADER);
  glBindVertexArray(VAO);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, SCRTEX);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glfwSwapBuffers(WINDOW);
}

void dv_glfw_set_key_callback(GLFWkeyfun cb){
  glfwSetKeyCallback(WINDOW,cb);
}

void dv_glfw_set_char_callback(GLFWcharfun cb){
  glfwSetCharCallback(WINDOW,cb);
}

void dv_glfw_stop_game(){
  KEEPGOING = false;
}

bool dv_glfw_keep_going(void){
  return KEEPGOING && !glfwWindowShouldClose(WINDOW);
}

int32_t dv_glfw_get_ticks(void){
  return 1000 * glfwGetTime();
}

void dv_glfw_exit(void){
  glfwTerminate();
}

