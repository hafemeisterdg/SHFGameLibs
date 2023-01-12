#define SHF_ECS_IMPL
#include <shf_ecs.h>

#include <random>

struct Component_Combat : public shf::ecs::Component {
	shf::ecs::Entity target;
	int32_t attack_damage;
};

struct Component_Health : public shf::ecs::Component {
	int32_t max_health;
	int32_t current_health;
};

struct Component_Status : public shf::ecs::Component {
	bool alive;
};

struct Combat_System : public shf::ecs::System {
	void update_entity(shf::ecs::Entity e, float delta_time) {
		Component_Combat* combat_component = shf::ecs::get_component<Component_Combat>(e);
		Component_Status* status_component = shf::ecs::get_component<Component_Status>(e);
		Component_Status* target_status_component = shf::ecs::get_component<Component_Status>(combat_component->target);
		Component_Health* target_health_component = shf::ecs::get_component<Component_Health>(combat_component->target);

		if (!status_component->alive || !target_status_component->alive) return;


		target_health_component->current_health -= combat_component->attack_damage;
		printf("Entity[%u] deals [%i] damage to Entity[%u]. Health Remaining - %i \n", e, combat_component->attack_damage, combat_component->target, target_health_component->current_health);
		if (target_health_component->current_health <= 0) {
			printf("    [+] The final blow was dealt! Entity[%u] is dead. \n", combat_component->target);
			target_status_component->alive = false;
		}
	}
};

void simulate_combat_rounds(Combat_System* system, uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		system->update(0);
	}
}

int main(int argc, char* argv[]) {
	shf::ecs::register_component<Component_Combat>();
	shf::ecs::register_component<Component_Health>();
	shf::ecs::register_component<Component_Status>();

	Combat_System* combat_system = shf::ecs::register_system<Combat_System>();

	for (int i = 0; i < 100; i++) {
		shf::ecs::Entity e = shf::ecs::create_entity();

		Component_Combat combat_component;
		combat_component.target = (i == 99) ? 0 : i + 1;
		combat_component.attack_damage = (rand() % 100) + 15;

		Component_Health health_component;
		health_component.max_health = (rand() % 1200) + 1000;
		health_component.current_health = health_component.max_health;

		Component_Status status_component;
		status_component.alive = true;

		shf::ecs::add_component<Component_Combat>(e, combat_component);
		shf::ecs::add_component<Component_Health>(e, health_component);
		shf::ecs::add_component<Component_Status>(e, status_component);
	}

	simulate_combat_rounds(combat_system, 25);

	return 0;
}