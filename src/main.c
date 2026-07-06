#include <unistd.h>
#include <string.h>

#include "dv_glfw.h"
#include "dv_font.h"

uint32_t TICK = 0, SKIP = 0;

#define MAX_INPUT_LEN 256
char INPUT_BUFFER[MAX_INPUT_LEN];
uint32_t INPUT_POS = 0;

typedef struct{
  char string[MAX_INPUT_LEN];
  uint32_t tick;
} input_record_t;

#define INPUT_HISTORY_LEN 256
input_record_t INPUT_HISTORY[INPUT_HISTORY_LEN];
int HISTORY_POS = 0;

void execute_input_buffer(void){
  static char command[20];

  memset(command, 0, 20);

  for( int i=0; i<16; i++ ){
    command[i] = INPUT_BUFFER[i];
    if(command[i] == ' ' ){ command[i] = '\0'; }
    if(command[i] == '\0'){ break; }
  }

  int ret = strcmp(command, "exit");
  if( ret == 0 ){ dv_glfw_stop_game(); }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  (void)mods;
  (void)window;
  (void)scancode;

  if(action == GLFW_PRESS){
    if (key == GLFW_KEY_ESCAPE){
      dv_glfw_stop_game();
    }else if(key == GLFW_KEY_BACKSPACE){
      if(INPUT_POS > 0){ INPUT_POS -= 1; INPUT_BUFFER[INPUT_POS] = '\0'; }
    }else if(key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER){
      execute_input_buffer();
      INPUT_HISTORY[HISTORY_POS].tick = TICK;
      sprintf(INPUT_HISTORY[HISTORY_POS].string, "%s", INPUT_BUFFER);
      HISTORY_POS += 1;
      INPUT_POS = 0; INPUT_BUFFER[INPUT_POS] = '\0';
    }
  }
}

void char_callback(GLFWwindow* window, unsigned int codepoint){
  (void)window;
  char input_char;

  if (codepoint >= 32 && codepoint <= 126) { // printable ASCII range (32-126)
    input_char = (char)codepoint; // cast directly to a standard 8-bit char
  } else {
    input_char = '?'; // non-ASCII or non-printable replaced by ?
  }

  INPUT_BUFFER[INPUT_POS] = input_char;
  INPUT_POS += 1;
  INPUT_BUFFER[INPUT_POS] = 0;
}

int main(void){
  dv_fb_t *screen_data = dv_glfw_init();
  dv_fbr_t screen = dv_fbr_verify(screen_data, 0, 0, screen_data->w, screen_data->h);

  dv_glfw_set_key_callback(&key_callback);
  dv_glfw_set_char_callback(&char_callback);

  dv_fb_t *back_image = dv_get_image("bg-frame-single-4-3.png");
  dv_fbr_t screen_clear = dv_fbr_verify(back_image, 0, 0, back_image->w, back_image->h);

  font_t *title_font = font_create("font-title.png", RGBA(0x99,0xDD,0xFF,0xFF), RGBA(0x00,0x00,0x00,0x66));
  font_t *prose_font = font_create("font-prose.png", RGBA(0x99,0xDD,0xFF,0xFF), RGBA(0x00,0x00,0x00,0x66));
  font_t *small_font = font_create("font-small.png", RGBA(0x99,0xDD,0xFF,0x99), RGBA(0x00,0x00,0x00,0x66));

  int32_t cms = 0, pms = 0, msd = 0, msa = 0, mspf = 10 ;
  while (dv_glfw_keep_going()) {
    glfwPollEvents();

    pms = cms; cms = dv_glfw_get_ticks(); msd = cms - pms;
    if(msd < 0){ msd = mspf; } /* Time went backwards somehow. Act normal. */
    msa += msd;

    if(msa > mspf){ TICK += 1; msa -= mspf;
      dv_fbr_blit(screen_clear, screen);
      font_draw_string(title_font, "Welcome to This Game", 24, 24, screen.fb);
      font_draw_string(prose_font, INPUT_BUFFER, 32, 270,  screen.fb);
      for( int i = 1; i < 10; i++ ){
        static char text_format_buffer[1024];
        if( HISTORY_POS - i < 0 ){ break; }
        sprintf(text_format_buffer, "%i %s",INPUT_HISTORY[HISTORY_POS-i].tick,INPUT_HISTORY[HISTORY_POS-i].string);
        font_draw_string(prose_font, text_format_buffer, 32, 270-(i*10), screen.fb);
      }
      while(msa > mspf){ SKIP += 1; msa -= mspf; }

      font_draw_string(small_font, "Terminal version 0.0.1", 24, 290, screen.fb);
    }

    dv_glfw_draw_and_swap();
    sleep(0);
  }

  dv_glfw_exit();
  return 0;
}

