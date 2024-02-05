// zprecision.h : Test z precision

#pragma once

#include <k3.h>

#pragma pack(push, 1)
struct vertex_t {
	float position[4];
};
#pragma pack(pop)

union flint_t {
	float f;
	uint32_t i;
};

class App
{
private:
	static const uint32_t CB_VERSIONS = 2;
	k3win win;
	k3gfx gfx;
	k3cmdBuf cmd_buf;
	k3gfxState depth_state;
	k3gfxState render_state;
	k3buffer ztri_vb;
	k3buffer fullscreen_vb;
	k3surf depth_surf;

	static const uint32_t win_width = 1024;
	static const uint32_t win_height = 128;

public:
	App();
	~App();

	void Setup();
	void Keyboard(k3key k, char c, k3keyState state);
	void Display();
	static void K3CALLBACK KeyboardCallback(void* data, k3key k, char c, k3keyState state);
	static void K3CALLBACK DisplayCallback(void* data);
	static void K3CALLBACK IdleCallback(void* data);
};

// TODO: Reference additional headers your program requires here.
