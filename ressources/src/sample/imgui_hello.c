// Dear Imgui demo
// see https://github.com/ocornut/imgui

#include <ctoy.h>
#include "../cimgui/cimgui.h"
#include "../util/imgui_ctoy.c"
#include "../util/imgui_gles2.c"

ImGuiContext *_ctx = NULL;

void ctoy_begin(void)
{
   printf("Hello Dear ImGui !\n");
   ctoy_window_title("Hello Dear ImGui !");

   _ctx = igCreateContext(NULL);
   ImGui_ImplCToy_Init();
}

void ctoy_end(void)
{
   ImGui_ImplOpenGLES2_DestroyDeviceObjects();
   igDestroyContext(_ctx);
}

void ctoy_main_loop(void)
{
   glClearColor(0, 0.5, 0, 1);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   ImGui_ImplOpenGLES2_NewFrame();
   ImGui_ImplCToy_NewFrame();
   igNewFrame();
   
   // Dear ImGui Hello World
   {
      static char buf[256] = "Quick brown fox";
      static float f = 0.6f;
      const ImVec2 vec2_identity = {0};

      igText("Hello, world %d", 123);
      if (igButton("Save", vec2_identity))
      {
         // do stuff
      }

      igInputText("string", buf, 256, ImGuiInputTextFlags_None, NULL, NULL);
      igSliderFloat("float", &f, 0.0f, 1.0f, "%.3f", 1.0f);
   }

   igRender();
   ImGui_ImplOpenGLES2_RenderDrawData(igGetDrawData());
   
   ctoy_swap_buffer(NULL);
}
