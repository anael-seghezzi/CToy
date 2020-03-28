// OpenGL-ES2 hello triangle

#include <ctoy.h>
#include "../util/img_util.c"
#include "../util/gl_util.c"

GLuint vert_shader = 0;
GLuint frag_shader = 0;
GLuint prog_object = 0;

char vert_src[] =
"attribute vec4 aPosition;"
"void main()"
"{"
" gl_Position = aPosition;"
"}";                             

char frag_src[] =
"#ifdef GL_ES\n"
"precision mediump float;\n"
"#endif\n"
"void main()"
"{"
" gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);"
"}";

void draw(void)
{
   float vertices[] = {
       0.0,  0.5, 0.0,
      -0.5, -0.5, 0.0,
       0.5, -0.5, 0.0
   };

   glUseProgram(prog_object);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vertices);
   glEnableVertexAttribArray(0);
   glDrawArrays(GL_TRIANGLES, 0, 3);
}

void ctoy_begin(void)
{
   const char * version = (const char *)glGetString(GL_VERSION);

   printf("<sample triangle_hello>\n");
   printf("%s\n", version);

   vert_shader = gu_shader_from_string(GL_VERTEX_SHADER, vert_src);
   frag_shader = gu_shader_from_string(GL_FRAGMENT_SHADER, frag_src);
   prog_object = glCreateProgram();

   glAttachShader(prog_object, vert_shader);
   glAttachShader(prog_object, frag_shader);
   glBindAttribLocation(prog_object, 0, "aPosition");
   glLinkProgram(prog_object);

   glDisable(GL_CULL_FACE);
   glDisable(GL_TEXTURE_2D);
   glDisable(GL_DEPTH_TEST);
}

void ctoy_end(void)
{
   glDeleteProgram(prog_object);
   glDeleteShader(vert_shader);
   glDeleteShader(frag_shader);
}

void ctoy_main_loop()
{
   glViewport(0, 0, ctoy_frame_buffer_width(), ctoy_frame_buffer_height());

   glClearColor(0, 0.2, 0, 0);
   glClear(GL_COLOR_BUFFER_BIT);

   draw();
   ctoy_swap_buffer(NULL);
}
