#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "glad/gl.h"
#include <GLFW/glfw3.h>
#include "dv_fb.h"

#define VIRTUAL_SCREEN_W 480
#define VIRTUAL_SCREEN_H 320

dv_fb_t *dv_glfw_init(void);
void     dv_glfw_exit(void);

void dv_glfw_draw_and_swap();
bool dv_glfw_keep_going();
int32_t dv_glfw_get_ticks(void);

void dv_glfw_stop_game();

void dv_glfw_set_key_callback(GLFWkeyfun cb);
void dv_glfw_set_char_callback(GLFWcharfun cb);

GLFWwindow *dv_glfw_get_window(void);
