// OpenGL-ES2 utils

void gu_texture_from_file(const char *filename)
{
   struct m_image ubi = M_IMAGE_IDENTITY();
   if (m_image_load(&ubi, filename)) {
      if (ubi.comp == 1)
         glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, ubi.width, ubi.height, 0, GL_RGB, GL_UNSIGNED_BYTE, ubi.data);
      else if (ubi.comp == 3)
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ubi.width, ubi.height, 0, GL_RGB, GL_UNSIGNED_BYTE, ubi.data);
      else if (ubi.comp == 4)
         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ubi.width, ubi.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ubi.data);
   }
   m_image_destroy(&ubi);
}

GLuint gu_shader_from_string(GLenum type, const char *src)
{
   GLuint shader;
   GLint compiled;

   shader = glCreateShader(type);
   if (shader == 0)
      return 0;

   glShaderSource(shader, 1, &src, NULL);
   glCompileShader(shader);

   glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
   if (!compiled) {

      GLint ilen = 0;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &ilen);
      
      if (ilen > 0) {
         char *infolog = malloc(sizeof(char) * (ilen + 1));
         glGetShaderInfoLog(shader, ilen, NULL, infolog);
         printf("Error compiling shader:\n%s\n", infolog);
         free(infolog);
      }

      glDeleteShader(shader);
      return 0;
   }

   return shader;
}
