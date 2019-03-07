//#include <mud/frame.h>
#include <frame/Api.h>
#include <gfx-pbr/Api.h>

#include <xx_three/xx_three.h>

using namespace mud;

#define IMMEDIATE 0

Texture& generateTexture(GfxSystem& gfx)
{
	Texture& texture = gfx.textures().create("half");

	uint16_t width = 2U;
	uint16_t height = 2U;

	const bgfx::Memory* memory = bgfx::alloc(width * height * 4);

	uint32_t* data = (uint32_t*)memory->data;
	for(uint16_t x = 0; x < width; ++x)
		for(uint16_t y = 0; y < height; ++y)
		{
			if(y == 0) data[x + y * width] = 0xffffffff;
			else data[x + y * width] = 0x00000000;
		}

	load_texture_rgba(texture, 2U, 2U, *memory);
	return texture;
}

void xx_shadow_point(Shell& app, Widget& parent, Dockbar& dockbar)
{
	UNUSED(dockbar);
	constexpr uint32_t colors[] = { 0x0088ff, 0xff8888 };
	constexpr float intensity = 1.5f;
	constexpr float range = 20.0f;

	SceneViewer& viewer = ui::scene_viewer(parent);
	//ui::orbit_controller(viewer);
	ui::trackball_controller(viewer);

#if !IMMEDIATE
	Scene& scene = viewer.m_scene;
#endif

	static Material* cubemat = nullptr;
	static Material* spheremat = nullptr;

#if !IMMEDIATE
	static Node3* lights[2] = { nullptr, nullptr };
#endif

	static bool once = false;
	if(!once)
	{
		once = true;

		//scene.add(new THREE.AmbientLight(0x111122));

		Program* pbr = app.m_gfx.programs().file("pbr/pbr");

		Material& c = app.m_gfx.materials().fetch("cube");
		c.m_program = pbr;
		c.m_base.m_cull_mode = CullMode::Front;
		c.m_pbr = MaterialPbr(rgb(0xa0adaf));
		c.m_pbr.m_normal = -1.f;
		//	shininess: 10,
		//	specular: 0x111111,
		cubemat = &c;

		Material& s = app.m_gfx.materials().fetch("sphere");
		s.m_program = pbr;
		s.m_base.m_cull_mode = CullMode::None;
		s.m_base.m_uv0_scale = { 1.f, 3.5f };
		s.m_alpha.m_alpha_test = true;
		s.m_alpha.m_alpha = &generateTexture(app.m_gfx);
		s.m_pbr.m_normal = -1.f; // @todo @bug @hack check why gl_FrontFacing in shader inverts normals
		spheremat = &s;

#if !IMMEDIATE
		for(size_t i = 0; i < 2; ++i) {

			Colour color = rgb(colors[i]);

			Model& sphere0 = app.m_gfx.shape(Sphere(0.3f));
			Model& sphere1 = app.m_gfx.shape(Sphere(2.0f));

			Node3& node = gfx::nodes(scene).add(Node3());
			Item& inner = gfx::items(scene).add(Item(node, sphere0, 0, &app.m_gfx.symbol_material(Symbol(color))));// * intensity));
			Item& outer = gfx::items(scene).add(Item(node, sphere1, 0, spheremat));

			Light& light = gfx::lights(scene).add(Light(node, LightType::Point, false, color, intensity, range)); //, 0.6));// intensity);
			light.m_attenuation = 0.6f;
			//light.shadow_bias = 0.005;

			lights[i] = &node;
		}

		Node3& node = gfx::nodes(scene).add(Node3());// , Y3 * 10.0);
		gfx::items(scene).add(Item(node, app.m_gfx.shape(Cube(vec3(15.f))), 0, cubemat));
#endif

		viewer.m_scene.m_env.m_radiance.m_ambient = 0.2f;
	}

#if IMMEDIATE
	auto light_source = [&](Gnode& parent, Colour color, vec3 pos, quat rot) -> Gnode&
	{
		float intensity = 1.5f;
		float range = 20.f;

		Gnode& node = gfx::node(parent, {}, pos, rot);

		Item& inner = gfx::shape(node, Sphere(0.3f), Symbol::plain(color * intensity));
		Item& outer = gfx::shape(node, Sphere(2.0f), Symbol::plain(Colour::White), 0U, spheremat);
		
		Light& light = gfx::light(node, LightType::Point, true, color, range, 0.6f);// intensity);
		light.m_range = range;
		light.m_energy = intensity;
		light.m_shadow_bias = 0.005f;

		return node;
	};
#endif

	auto pos = [](float time) -> vec3
	{
		return vec3(sin(time * 0.6) * 9, sin(time * 0.7) * 9 + 5, sin(time * 0.8) * 9);
	};

	auto rot = [](float time) -> quat
	{
		return quat(vec3(time, 0.f, time));
	};
	
#if IMMEDIATE
	Gnode& scene = viewer.m_scene.begin();

	gfx::radiance(scene, "radiance/tiber_1_1k.hdr", BackgroundMode::Radiance);
#endif

	static bool moving = true;
	static float time = 0.f;
	if(moving)
		time = app.m_gfx.m_time;

	// vec3 pos = vec3(0.f, 10.f, 0.f);
	// quat rot = quat(vec3(0.f, c_pi / 2.f + c_pi / 4.f, c_pi / 2.f + c_pi / 4.f));

#if IMMEDIATE
	Gnode& light0 = light_source(scene, rgb(0x0088ff), pos(time), rot(time));
	Gnode& light1 = light_source(scene, rgb(0xff8888), pos(time + c_pi), rot(time + c_pi));
#else
	lights[0]->apply(pos(time), rot(time));
	lights[1]->apply(pos(time + c_pi), rot(time + c_pi));
#endif

#if IMMEDIATE
	Box box = Cube(vec3(15.f));
	Gnode& node = gfx::node(scene, {});// , Y3 * 10.f);
	gfx::shape(node, box, Symbol::plain(Colour::White), 0U, cubemat);

	gfx::shape(scene, Cylinder(X3, 0.1f, 1.f, Axis::X), Symbol::plain(Colour::Red));
	gfx::shape(scene, Cylinder(Y3, 0.1f, 1.f, Axis::Y), Symbol::plain(Colour::Green));
	gfx::shape(scene, Cylinder(Z3, 0.1f, 1.f, Axis::Z), Symbol::plain(Colour::Blue));
#endif

}