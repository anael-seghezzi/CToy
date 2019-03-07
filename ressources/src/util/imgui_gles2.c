// imgui_impl_openGLES2.cpp

// OpenGL Data
static GLuint       g_FontTexture = 0;
static GLuint       g_ShaderHandle = 0, g_VertHandle = 0, g_FragHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static int          g_AttribLocationPosition = 0, g_AttribLocationUV = 0, g_AttribLocationColor = 0;
static unsigned int g_VboHandle = 0, g_ElementsHandle = 0;

void ImGui_ImplOpenGLES2_DestroyFontsTexture()
{
	if (g_FontTexture)
	{
		ImGuiIO* io = igGetIO();
		glDeleteTextures(1, &g_FontTexture);
		io->Fonts->TexID = 0;
		g_FontTexture = 0;
	}
}

void ImGui_ImplOpenGLES2_DestroyDeviceObjects()
{
    if (g_VboHandle) glDeleteBuffers(1, &g_VboHandle);
    if (g_ElementsHandle) glDeleteBuffers(1, &g_ElementsHandle);
    g_VboHandle = g_ElementsHandle = 0;

    if (g_ShaderHandle && g_VertHandle) glDetachShader(g_ShaderHandle, g_VertHandle);
    if (g_VertHandle) glDeleteShader(g_VertHandle);
    g_VertHandle = 0;

    if (g_ShaderHandle && g_FragHandle) glDetachShader(g_ShaderHandle, g_FragHandle);
    if (g_FragHandle) glDeleteShader(g_FragHandle);
    g_FragHandle = 0;

    if (g_ShaderHandle) glDeleteProgram(g_ShaderHandle);
    g_ShaderHandle = 0;

    ImGui_ImplOpenGLES2_DestroyFontsTexture();
}

int ImGui_ImplOpenGLES2_CreateFontsTexture()
{
	GLint last_texture;

    // Build texture atlas
	ImGuiIO* io = igGetIO();
    unsigned char* pixels;
    int width, height, bpp;
    ImFontAtlas_GetTexDataAsRGBA32(io->Fonts, &pixels, &width, &height, &bpp);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // Upload texture to graphics system
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io->Fonts->TexID = (ImTextureID)(intptr_t)g_FontTexture;

    // Restore state
    glBindTexture(GL_TEXTURE_2D, last_texture);
	
    return 1;
}

int ImGui_ImplOpenGLES2_CreateDeviceObjects()
{
	GLint last_texture, last_array_buffer, last_vertex_array;

	const GLchar* vertex_shader =
        "uniform mat4 ProjMtx;\n"
        "attribute vec2 Position;\n"
        "attribute vec2 UV;\n"
        "attribute vec4 Color;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#ifdef GL_ES\n"
        "    precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D Texture;\n"
        "varying vec2 Frag_UV;\n"
        "varying vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV.st);\n"
        "}\n";

    // Backup GL state
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);

    // Create shaders
    g_VertHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(g_VertHandle, 1, &vertex_shader, NULL);
    glCompileShader(g_VertHandle);

    g_FragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(g_FragHandle, 1, &fragment_shader, NULL);
    glCompileShader(g_FragHandle);

    g_ShaderHandle = glCreateProgram();
    glAttachShader(g_ShaderHandle, g_VertHandle);
    glAttachShader(g_ShaderHandle, g_FragHandle);
    glLinkProgram(g_ShaderHandle);

    g_AttribLocationTex = glGetUniformLocation(g_ShaderHandle, "Texture");
    g_AttribLocationProjMtx = glGetUniformLocation(g_ShaderHandle, "ProjMtx");
    g_AttribLocationPosition = glGetAttribLocation(g_ShaderHandle, "Position");
    g_AttribLocationUV = glGetAttribLocation(g_ShaderHandle, "UV");
    g_AttribLocationColor = glGetAttribLocation(g_ShaderHandle, "Color");

    // Create buffers
    glGenBuffers(1, &g_VboHandle);
    glGenBuffers(1, &g_ElementsHandle);

    ImGui_ImplOpenGLES2_CreateFontsTexture();

    // Restore modified GL state
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);

    return 1;
}

void ImGui_ImplOpenGLES2_NewFrame()
{
    if (!g_FontTexture)
        ImGui_ImplOpenGLES2_CreateDeviceObjects();
}

void ImGui_ImplOpenGLES2_RenderDrawData(ImDrawData* draw_data)
{
	GLenum last_blend_src_rgb; 
	GLenum last_blend_dst_rgb;
	GLint last_scissor_box[4];
	GLint last_viewport[4];
	GLint last_polygon_mode[2];
	GLenum last_active_texture;
	GLint last_program;
	GLint last_texture;
	GLint last_sampler;
	GLint last_array_buffer;
	GLenum last_blend_src_alpha; 
	GLenum last_blend_dst_alpha; 
	GLenum last_blend_equation_rgb; 
	GLenum last_blend_equation_alpha; 
	GLboolean last_enable_blend;
	GLboolean last_enable_cull_face;
	GLboolean last_enable_depth_test;
	GLboolean last_enable_scissor_test;
	GLenum last_clip_origin = 0;
	
	GLuint vao_handle = 0;
	ImVec2 clip_off, clip_scale;
	int clip_origin_lower_left = 1;
	int n;

    // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    // Backup GL state
    glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
    glActiveTexture(GL_TEXTURE0);
    glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
    glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
#ifdef GL_POLYGON_MODE
    glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
    glGetIntegerv(GL_VIEWPORT, last_viewport);
    glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
    glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
    glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
    glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
    glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
    last_enable_blend = glIsEnabled(GL_BLEND);
    last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
#ifdef GL_CLIP_ORIGIN
    glGetIntegerv(GL_CLIP_ORIGIN, (GLint*)&last_clip_origin); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
    if (last_clip_origin == GL_UPPER_LEFT)
        clip_origin_lower_left = 0;
#endif

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayMin is typically (0,0) for single viewport apps.
    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
	{
		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
		const float ortho_projection[4][4] =
		{
			{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
			{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
			{ 0.0f,         0.0f,        -1.0f,   0.0f },
			{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
		};
		glUseProgram(g_ShaderHandle);
		glUniform1i(g_AttribLocationTex, 0);
		glUniformMatrix4fv(g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	}
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif
    // Recreate the VAO every time
    // (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
    
    glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
    glEnableVertexAttribArray(g_AttribLocationPosition);
    glEnableVertexAttribArray(g_AttribLocationUV);
    glEnableVertexAttribArray(g_AttribLocationColor);
    glVertexAttribPointer(g_AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)0);
    glVertexAttribPointer(g_AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)8);
    glVertexAttribPointer(g_AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)16);

    // Will project scissor/clipping rectangles into framebuffer space
    clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t idx_buffer_offset = 0;
		int cmd_i;

        glBindBuffer(GL_ARRAY_BUFFER, g_VboHandle);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);
	
        for (cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer.Data[cmd_i];
            if (pcmd->UserCallback)
            {
                // User callback (registered via ImDrawList::AddCallback)
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    if (clip_origin_lower_left)
                        glScissor((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
                    else
                        glScissor((int)clip_rect.x, (int)clip_rect.y, (int)clip_rect.z, (int)clip_rect.w); // Support for GL 4.5's glClipControl(GL_UPPER_LEFT)

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)idx_buffer_offset);
                }
            }
            idx_buffer_offset += pcmd->ElemCount * sizeof(ImDrawIdx);
        }
    }

    // Restore modified GL state
    glUseProgram(last_program);
    glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
    glBindSampler(0, last_sampler);
#endif
    glActiveTexture(last_active_texture);

    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
