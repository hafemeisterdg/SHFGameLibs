//#include "ecs_test.cpp" // defines and includes shf_ecs

#define SHF_MATH_IMPL
#include <shf_math.h>

#define SHF_PLATFORM_IMPL
#include <shf_platform.h>

#define SHF_GL_IMPL
#include <shf_gl.h>

#define SHF_ECS_IMPL
#include <shf_ecs.h>

struct Component_Transform : public shf::ecs::Component {
	shf::math::Vec3 position = shf::math::Vec3(0, 0, 0);
	shf::math::Vec3 rotation = shf::math::Vec3(0, 0, 0);
	shf::math::Vec3 scale    = shf::math::Vec3(1, 1, 1);
};

struct Component_Velocity : public shf::ecs::Component {
	shf::math::Vec3 velocity = shf::math::Vec3(0, 0, 0);
};

struct Component_Renderable : public shf::ecs::Component {
	shf::gl::Vertex_Buffer* vertex_buffer;
	shf::gl::Index_Buffer*  index_buffer;
	shf::gl::Texture*       texture;

	bool indexed  = false;
	bool textured = false;
};

struct Physics_System : public shf::ecs::System {
	void update(float delta_time) {
		for (shf::ecs::Entity e : entities) {
			Component_Transform* transform = shf::ecs::get_component<Component_Transform>(e);
			Component_Velocity*  velocity  = shf::ecs::get_component<Component_Velocity>(e);

			transform->position.x += velocity->velocity.x * delta_time;
			transform->position.y += velocity->velocity.y * delta_time;
			transform->position.z += velocity->velocity.z * delta_time;
		}
	}
};

struct Rendering_System : public shf::ecs::System {
	shf::gl::Vertex_Array_Object main_vao;

	void draw_renderable(Component_Renderable* renderable) {
		if (renderable->indexed) {
			if (renderable->textured) shf::gl::draw_textured_indexed(renderable->vertex_buffer, renderable->index_buffer, renderable->texture);
			else                      shf::gl::draw_indexed(renderable->vertex_buffer, renderable->index_buffer);
		} else {
			if (renderable->textured) shf::gl::draw_textured(renderable->vertex_buffer, renderable->texture);
			else                      shf::gl::draw(renderable->vertex_buffer);
		}
	}

	void update(float delta_time) {
		for (shf::ecs::Entity e : entities) {
			Component_Transform*  transform  = shf::ecs::get_component<Component_Transform>(e);
			Component_Renderable* renderable = shf::ecs::get_component<Component_Renderable>(e);

			// Do stuff with transform and camera and whatnot 
			draw_renderable(renderable);
		}
	}
};

struct Game_State {
	shf::platform::Window* window;
	shf::gl::GL_Context    gl_ctx;
	Physics_System*        physics;
	Rendering_System*      rendering;
	shf::ecs::Entity       player;
};
static Game_State* g_game_state = 0;

void keyboard_callback(shf::platform::Key_Code code, shf::platform::Key_Modifiers mods, shf::platform::Key_Action action) {
	switch (action) {
		case shf::platform::Key_Action_Pressed: {
			Component_Velocity* player_velocity = shf::ecs::get_component<Component_Velocity>(g_game_state->player);

			switch (code) {
				case shf::platform::Key_Code_W: {
					player_velocity->velocity.y = 1.0f;
				} break;

				case shf::platform::Key_Code_A: {
					player_velocity->velocity.x = -1.0f;
				} break;

				case shf::platform::Key_Code_S: {
					player_velocity->velocity.y = -1.0f;
				} break;

				case shf::platform::Key_Code_D: {
					player_velocity->velocity.x = 1.0f;
				} break;
			}

			printf("Key pressed %u \n", code);
		} break;
	}
}

bool setup_game_state() {
	g_game_state = (Game_State*)malloc(sizeof(Game_State));

	if (!g_game_state) return false;

	g_game_state->window = shf::platform::create_window("Testing", 1280, 720);
	if (!shf::gl::create_context(g_game_state->window->platform_window, 4, 6, &g_game_state->gl_ctx)) {
		shf::platform::destroy_window(g_game_state->window);
		free(g_game_state);

		return false;
	}

	shf::ecs::register_component<Component_Transform>();
	shf::ecs::register_component<Component_Velocity>();
	shf::ecs::register_component<Component_Renderable>();

	g_game_state->physics = shf::ecs::register_system<Physics_System>();
	g_game_state->physics->track_component_type<Component_Transform>();
	g_game_state->physics->track_component_type<Component_Velocity>();

	g_game_state->rendering = shf::ecs::register_system<Rendering_System>();
	g_game_state->rendering->track_component_type<Component_Transform>();
	g_game_state->rendering->track_component_type<Component_Renderable>();

	g_game_state->player = shf::ecs::create_entity();
	
	shf::gl::Vertex quad_vertices[4];
	quad_vertices[0].position.x = -0.5f;
	quad_vertices[0].position.y = -0.5f;

	quad_vertices[1].position.x = -0.5f;
	quad_vertices[1].position.y = 0.5f;

	quad_vertices[2].position.x = 0.5f;
	quad_vertices[2].position.y = 0.5f;

	quad_vertices[3].position.x = 0.5f;
	quad_vertices[3].position.y = -0.5f;

	uint32_t quad_indices[6] = {
		0, 1, 2,
		2, 1, 3
	};

	Component_Transform  transform;
	Component_Velocity   velocity;
	Component_Renderable renderable;

	renderable.vertex_buffer = (shf::gl::Vertex_Buffer*)malloc(sizeof(shf::gl::Vertex_Buffer));
	renderable.index_buffer = (shf::gl::Index_Buffer*)malloc(sizeof(shf::gl::Index_Buffer));
	renderable.indexed = true;
	shf::gl::create_vertex_buffer(renderable.vertex_buffer);
	shf::gl::create_index_buffer(renderable.index_buffer);
	renderable.vertex_buffer->buffer_data(&quad_vertices[0], 4, GL_STATIC_DRAW);
	renderable.index_buffer->buffer_data(&quad_indices[0], 6, GL_STATIC_DRAW);

	shf::ecs::add_component<Component_Transform>(g_game_state->player, transform);
	shf::ecs::add_component<Component_Velocity>(g_game_state->player, velocity);
	shf::ecs::add_component<Component_Renderable>(g_game_state->player, renderable);

	if (!shf::gl::create_vertex_array_object(&g_game_state->rendering->main_vao)) {
		shf::gl::destroy_context(&g_game_state->gl_ctx);
		shf::platform::destroy_window(g_game_state->window);
		free(g_game_state);

		return false;
	}

	g_game_state->rendering->main_vao.bind();
	g_game_state->window->set_keyboard_callback(&keyboard_callback);

	return true;
}

void cleanup_game_state() {
	shf::gl::destroy_vertex_array_object(&g_game_state->rendering->main_vao);
	shf::gl::destroy_context(&g_game_state->gl_ctx);
	shf::platform::destroy_window(g_game_state->window);
	free(g_game_state);
}

void game_loop() {
	while (!g_game_state->window->should_close) {
		g_game_state->window->poll_input();

		shf::gl::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shf::gl::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Simulate 60fps at 16ms a frame
		g_game_state->physics->update(.016f);
		g_game_state->rendering->update(.016f);

		g_game_state->gl_ctx.swap_buffers();
	}
}

int main(int argc, char* argv[]) {
	if (!setup_game_state()) return -1;

	game_loop();

	cleanup_game_state();

	return 0;
}
