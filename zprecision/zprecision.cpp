// zprecision.cpp : Tests z precision

#include "zprecision.h"

App::App()
{
	Setup();
}

App::~App()
{ }

void App::Setup()
{
	win = k3winObj::CreateWindowed("Z Precision", 100, 100, win_width, win_height, 128, 32);
	win->SetKeyboardFunc(KeyboardCallback);
	win->SetDisplayFunc(DisplayCallback);
	win->SetIdleFunc(IdleCallback);
	win->SetVisible(true);
	win->SetCursorVisible(true);
	win->SetVsyncInterval(0);
	win->SetDataPtr(this);

	gfx = win->GetGfx();

	k3resourceDesc rdesc = { 0 };
	rdesc.width = win_width;
	rdesc.height = win_height;
	rdesc.depth = 1;
	rdesc.mip_levels = 1;
	rdesc.num_samples = 1;
	rdesc.format = k3fmt::D32_FLOAT;
	k3viewDesc vdesc = { 0 };
	depth_surf = gfx->CreateSurface(&rdesc, &vdesc, NULL, NULL);

	cmd_buf = gfx->CreateCmdBuf();

	k3shader vs, ps;
	vs = gfx->CreateShaderFromCompiledFile("simple_vs.cso");
	ps = gfx->CreateShaderFromCompiledFile("simple_ps.cso");

	//k3bindingParam bind_params[1];
	//bind_params[0].type = k3bindingType::VIEW_SET;
	//bind_params[0].view_set_desc.type = k3shaderBindType::CBV;
	//bind_params[0].view_set_desc.num_views = 1;
	//bind_params[0].view_set_desc.reg = 0;
	//bind_params[0].view_set_desc.space = 0;
	//bind_params[0].view_set_desc.offset = 0;
	k3shaderBinding main_binding = gfx->CreateShaderBinding(0, NULL, 0, NULL);

	k3inputElement elem[4];
	elem[0].name = "POSITION";
	elem[0].index = 0;
	elem[0].format = k3fmt::RGBA32_FLOAT;
	elem[0].slot = 0;
	elem[0].offset = 0;
	elem[0].in_type = k3inputType::VERTEX;
	elem[0].instance_step = 0;

	k3gfxStateDesc state_desc = { 0 };
	state_desc.num_input_elements = 1;
	state_desc.input_elements = elem;
	state_desc.shader_binding = main_binding;
	state_desc.vertex_shader = vs;
	state_desc.pixel_shader = ps;
	state_desc.sample_mask = ~0;
	state_desc.cut_index = k3stripCut::CUT_FFFF_FFFF;
	state_desc.rast_state.fill_mode = k3fill::SOLID;
	state_desc.rast_state.cull_mode = k3cull::BACK;
	state_desc.rast_state.front_counter_clockwise = true;
	state_desc.depth_state.depth_enable = true;
	state_desc.depth_state.depth_write_enable = true;
	state_desc.depth_state.depth_test = k3testFunc::ALWAYS;
	state_desc.dsv_format = k3fmt::D32_FLOAT;
	state_desc.blend_state.alpha_to_mask = false;
	state_desc.blend_state.blend_op[0].rt_write_mask = 0x0;
	state_desc.prim_type = k3primType::TRIANGLE;
	state_desc.num_render_targets = 1;
	state_desc.rtv_format[0] = k3fmt::RGBA8_UNORM;
	state_desc.msaa_samples = 1;
	depth_state = gfx->CreateGfxState(&state_desc);

	state_desc.depth_state.depth_test = k3testFunc::EQUAL;
	state_desc.blend_state.blend_op[0].rt_write_mask = 0xf;
	render_state = gfx->CreateGfxState(&state_desc);

	flint_t FL_0;
	flint_t FL_1;
	flint_t FL_8;
	flint_t FL_NEG;
	flint_t FL_NEG_1;
	flint_t FL_NEG_8;
	//flint_t FL_INTERCEPT;
	flint_t FL_LSB;

	FL_0.f = 0.0f;;
	FL_1.f = 1.0f;
	FL_8.f = 8.0f;
	FL_NEG_1.f = -1.0f;;
	FL_NEG_8.f = -8.0f;
	FL_LSB.i = 0x00000001;// 0x382a0000;
	//FL_INTERCEPT.f = FL_LSB.f;// FL_1.f - (1.5f / win_width);
	//FL_INTERCEPT.i += FL_LSB.i;

	flint_t zstart, zend, zintercept;

	zstart.i = 0x00800000;// 0x05800000;
	zend.i = FL_0.i;
	zintercept.f = zstart.f - 1.5f * ((zstart.f - zend.f) / win_width);

	printf("Adapter: %s\n", gfx->AdapterName());

	k3uploadBuffer ztri_upbuf = gfx->CreateUploadBuffer();
	uint32_t* vert_data = (uint32_t*)ztri_upbuf->MapForWrite(4 * 4 * sizeof(float));
	vert_data[0] = FL_NEG_1.i; vert_data[1] = FL_1.i; vert_data[2] = zstart.i; vert_data[3] = FL_1.i;
	vert_data[4] = FL_NEG_1.i; vert_data[5] = FL_NEG_1.i; vert_data[6] = zstart.i; vert_data[7] = FL_1.i;
	vert_data[8] = FL_1.i; vert_data[9] = FL_1.i; vert_data[10] = zend.i; vert_data[11] = FL_1.i;
	vert_data[12] = FL_1.i; vert_data[13] = FL_NEG_1.i; vert_data[14] = zend.i; vert_data[15] = FL_1.i;
	printf("zrange: 0x%x 0x%x\n", zstart, zend);
	ztri_upbuf->Unmap();
	k3bufferDesc bdesc = { 0 };
	bdesc.size = 4 * 4 * sizeof(float);
	bdesc.stride = 4 * sizeof(float);
	ztri_vb = gfx->CreateBuffer(&bdesc);

	k3uploadBuffer fullscreen_upbuf = gfx->CreateUploadBuffer();
	vert_data = (uint32_t*)fullscreen_upbuf->MapForWrite(3 * 4 * sizeof(float));
	vert_data[0] = FL_NEG_1.i; vert_data[1] = FL_1.i; vert_data[2] = zintercept.i; vert_data[3] = FL_1.i;
	vert_data[4] = FL_NEG_1.i; vert_data[5] = FL_NEG_8.i; vert_data[6] = zintercept.i; vert_data[7] = FL_1.i;
	vert_data[8] = FL_8.i; vert_data[9] = FL_1.i; vert_data[10] = zintercept.i; vert_data[11] = FL_1.i;
	printf("z-intercept: 0x%x\n", zintercept);
	fullscreen_upbuf->Unmap();
	fullscreen_vb = gfx->CreateBuffer(&bdesc);

	cmd_buf->Reset();
	k3resource resc = ztri_vb->GetResource();
	cmd_buf->TransitionResource(resc, k3resourceState::COPY_DEST);
	cmd_buf->UploadBuffer(ztri_upbuf, resc);
	resc = fullscreen_vb->GetResource();
	cmd_buf->TransitionResource(resc, k3resourceState::COPY_DEST);
	cmd_buf->UploadBuffer(fullscreen_upbuf, resc);
	cmd_buf->TransitionResource(depth_surf->GetResource(), k3resourceState::RENDER_TARGET);
	cmd_buf->Close();
	gfx->SubmitCmdBuf(cmd_buf);

	gfx->WaitGpuIdle();
}

void App::Keyboard(k3key k, char c, k3keyState state)
{
	if (state == k3keyState::PRESSED) {
		switch (k) {
		case k3key::ESCAPE:
			k3winObj::ExitLoop();
			break;
		}
	}
}

void App::Display()
{
	k3surf back_buf = win->GetBackBuffer();
	k3resource back_buf_resc = back_buf->GetResource();
	float clear_color[] = { 0.5f, 0.0f, 0.5f, 1.0f };
	k3renderTargets rt = { NULL };
	rt.render_targets[0] = back_buf;
	rt.depth_target = depth_surf;

	cmd_buf->Reset();
	cmd_buf->TransitionResource(back_buf_resc, k3resourceState::RENDER_TARGET);
	cmd_buf->ClearDepthTarget(depth_surf, k3depthSelect::DEPTH, 0.0f, 0, NULL);
	cmd_buf->ClearRenderTarget(back_buf, clear_color, NULL);
	cmd_buf->SetRenderTargets(&rt);
	cmd_buf->SetViewToSurface(back_buf_resc);
	cmd_buf->SetDrawPrim(k3drawPrimType::TRIANGLESTRIP);

	cmd_buf->SetGfxState(depth_state);
	cmd_buf->SetVertexBuffer(0, ztri_vb);
	cmd_buf->Draw(4);

	cmd_buf->SetGfxState(render_state);
	cmd_buf->SetVertexBuffer(0, fullscreen_vb);
	cmd_buf->Draw(3);

	cmd_buf->TransitionResource(back_buf_resc, k3resourceState::COMMON);
	cmd_buf->Close();
	gfx->SubmitCmdBuf(cmd_buf);
	win->SwapBuffer();
}

void K3CALLBACK App::KeyboardCallback(void* data, k3key k, char c, k3keyState state)
{
	App* a = (App*)data;
	a->Keyboard(k, c, state);
}

void K3CALLBACK App::DisplayCallback(void* data)
{
	App* a = (App*)data;
	a->Display();
}

void K3CALLBACK App::IdleCallback(void* data)
{
	App* a = (App*)data;
	a->Display();
}

int main()
{
	App a;
	k3winObj::WindowLoop();
}
