#ifdef __TINYC__
#define GLFWwindow void
void glfwSetClipboardString(GLFWwindow* handle, const char* string);
const char* glfwGetClipboardString(GLFWwindow* handle);
GLFWwindow * ctoy__get_glfw_window(void);
#endif

static const char* ImGui_ImplGlfw_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString((GLFWwindow*)user_data);
}

static void ImGui_ImplGlfw_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString((GLFWwindow*)user_data, text);
}

void ImGui_ImplCToy_Init()
{
	ImGuiIO* io = igGetIO();
	io->BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io->BackendPlatformName = "CToy";

	io->KeyMap[ImGuiKey_Tab] = CTOY_KEY_TAB;
	io->KeyMap[ImGuiKey_LeftArrow] = CTOY_KEY_LEFT;
	io->KeyMap[ImGuiKey_RightArrow] = CTOY_KEY_RIGHT;
	io->KeyMap[ImGuiKey_UpArrow] = CTOY_KEY_UP;
	io->KeyMap[ImGuiKey_DownArrow] = CTOY_KEY_DOWN;
	io->KeyMap[ImGuiKey_PageUp] = CTOY_KEY_PAGE_UP;
	io->KeyMap[ImGuiKey_PageDown] = CTOY_KEY_PAGE_DOWN;
	io->KeyMap[ImGuiKey_Home] = CTOY_KEY_HOME;
	io->KeyMap[ImGuiKey_End] = CTOY_KEY_END;
	io->KeyMap[ImGuiKey_Insert] = CTOY_KEY_INSERT;
	io->KeyMap[ImGuiKey_Delete] = CTOY_KEY_DELETE;
	io->KeyMap[ImGuiKey_Backspace] = CTOY_KEY_BACKSPACE;
	io->KeyMap[ImGuiKey_Space] = CTOY_KEY_SPACE;
	io->KeyMap[ImGuiKey_Enter] = CTOY_KEY_ENTER;
	io->KeyMap[ImGuiKey_Escape] = CTOY_KEY_ESCAPE;
	io->KeyMap[ImGuiKey_A] = CTOY_KEY_A;
	io->KeyMap[ImGuiKey_C] = CTOY_KEY_C;
	io->KeyMap[ImGuiKey_V] = CTOY_KEY_V;
	io->KeyMap[ImGuiKey_X] = CTOY_KEY_X;
	io->KeyMap[ImGuiKey_Y] = CTOY_KEY_Y;
	io->KeyMap[ImGuiKey_Z] = CTOY_KEY_Z;
   
	io->SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
	io->GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
	io->ClipboardUserData = ctoy__get_glfw_window();   
}

void ImGui_ImplCToy_NewFrame()
{
	static double time = 0.0;

	unsigned int chars[CTOY_CHAR_MAX];
    ImGuiIO* io = igGetIO();
	double current_time = ctoy_get_time();
	int i, nchars;

	// system
	io->DisplaySize.x = ctoy_window_width();
    io->DisplaySize.y = ctoy_window_height();
    io->DisplayFramebufferScale.x = (float)ctoy_frame_buffer_width() / ctoy_window_width();
    io->DisplayFramebufferScale.y = (float)ctoy_frame_buffer_height() / ctoy_window_height();
    io->DeltaTime = time > 0.0 ? (float)(current_time - time) : (float)(1.0f/60.0f);
    time = current_time;

	// mouse
	io->MousePos.x = ctoy_mouse_x() * io->DisplaySize.x;
	io->MousePos.y = ctoy_mouse_y() * io->DisplaySize.y;

	for (i = 0; i < 5; i++)
		io->MouseDown[i] = ctoy_mouse_button_press(i) || ctoy_mouse_button_pressed(i);

	io->MouseWheelH += ctoy_scroll_x() * io->DisplaySize.x;
	io->MouseWheel  += ctoy_scroll_y() * io->DisplaySize.y;
    
	// keys
	for (i = 0; i < 512; i++)
        io->KeysDown[i] = ctoy_key_pressed(i);

    io->KeyCtrl = io->KeysDown[CTOY_KEY_LEFT_CONTROL] || io->KeysDown[CTOY_KEY_RIGHT_CONTROL];
    io->KeyShift = io->KeysDown[CTOY_KEY_LEFT_SHIFT] || io->KeysDown[CTOY_KEY_RIGHT_SHIFT];
    io->KeyAlt = io->KeysDown[CTOY_KEY_LEFT_ALT] || io->KeysDown[CTOY_KEY_RIGHT_ALT];
    io->KeySuper = io->KeysDown[CTOY_KEY_LEFT_SUPER] || io->KeysDown[CTOY_KEY_RIGHT_SUPER];

	// characters
	nchars = ctoy_get_chars(chars);
	for (i = 0; i < nchars; i++) {
		int c = chars[i];
		if (c > 0 && c < 0x10000)
			ImGuiIO_AddInputCharacter(io, (unsigned short)c);
	}
}
