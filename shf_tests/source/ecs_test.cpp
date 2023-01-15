// A very simple proof of concept use case of the entity component system.
// This program demonstrates the ECS's ability to work with arbitrary custom components
// and systems in a data oriented, cache friendly, and safe manner.

// This library guarantees it's safety and stability with aggressive error checking.
// Any failure to comply with the libraries requirements will result in assertions.
// The error messages for each assertion are descriptive of exactly the issue and should
// hopefully lead to simple debugging. 

// To "link / include" the library in your project you must first
// #define SHF_ECS_IMPL in exactly 1 cpp file in your project prior to 
// including the header for the first time.
// From that point forward you can include shf_ecs.h anywhere in your project and
// utilize it's functionality.

#define SHF_ECS_IMPL
#include <shf_ecs.h>

#include <random>

// Components in the library are Plain Old Data structs that inherit from Component
// purely for the sake of template magic and type safety.
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

// Systems are a struct which inherit from System and implement the update(float) function.
// As long as the system is tracking any given component type, that type is 
// guaranteed safe to use and guaranteed to exist within the system. No safety checks required.
// More on tracking below during system setup (in main).
struct Combat_System : public shf::ecs::System {
	void update(float delta_time) {
		// We can imagine a basic system to iterate over all of the entities 
		// that the system is "watching". Accessible through the entities member
		// of anything inheriting system
		for(shf::ecs::Entity e : entities) {
			// Retreiving a component for system logic requries a single templated function call and the entity who owns the component.
			// These component pointers are live pointers to the actual contiguous packed component memory.
			Component_Combat* combat_component = shf::ecs::get_component<Component_Combat>(e);
			Component_Status* status_component = shf::ecs::get_component<Component_Status>(e);
			Component_Status* target_status_component = shf::ecs::get_component<Component_Status>(combat_component->target);
			Component_Health* target_health_component = shf::ecs::get_component<Component_Health>(combat_component->target);

			// If we are dead or our target is dead theres no need to attack, just skip this entity
			if (!status_component->alive || !target_status_component->alive) return;

			// Reduce targets health through components
			target_health_component->current_health -= combat_component->attack_damage;
			printf("Entity[%u] deals [%i] damage to Entity[%u]. Health Remaining - %i \n", e, combat_component->attack_damage, combat_component->target, target_health_component->current_health);
		
			// Check if the target should die. 
			if (target_health_component->current_health <= 0) {
				printf("    [+] The final blow was dealt! Entity[%u] is dead. \n", combat_component->target);
				target_health_component->current_health = 0;
				target_status_component->alive = false;
			}
		}
	}
};

// Forward declaration
void simulate_combat_rounds(Combat_System* system, uint32_t count);

void ecs_test() {
	// Any component types that we create need to be registered so the ECS is aware of them.
	shf::ecs::register_component<Component_Combat>();
	shf::ecs::register_component<Component_Health>();
	shf::ecs::register_component<Component_Status>();

	// Any system types we create need to be registered and flagged to track components relevant to the 
	// systems purpose. Systems will only know about and operate on entities that have component signatures
	// that match the system's.
	Combat_System* combat_system = shf::ecs::register_system<Combat_System>();
	combat_system->track_component_type<Component_Combat>();
	combat_system->track_component_type<Component_Health>();
	combat_system->track_component_type<Component_Status>();

	for (int i = 0; i < 100; i++) {
		// Creating an entity is a single function call
		shf::ecs::Entity e = shf::ecs::create_entity();

		// Fill out components as you would any POD struct
		// Constructors optionally can be added for quality of life
		// Destructors should never be added, memory is managed by the ECS and it could cause issues.
		// Components should also be stack allocated prior to being added to an entity. Not a heap allocated object.
		// After a component has been added shf::ecs::get_component<Component_Type> can be used to retreive
		// a pointer to the live component.
		Component_Combat combat_component;
		combat_component.target = (i == 99) ? 0 : i + 1; // set each entity to target the one in front of it. There's no honor among thieves!
		combat_component.attack_damage = (rand() % 100) + 15;

		Component_Health health_component;
		health_component.max_health = (rand() % 1200) + 1000; 
		health_component.current_health = health_component.max_health;

		Component_Status status_component;
		status_component.alive = true;

		// Adding components to an entity is a single templated function call
		shf::ecs::add_component<Component_Combat>(e, combat_component);
		shf::ecs::add_component<Component_Health>(e, health_component);
		shf::ecs::add_component<Component_Status>(e, status_component);
	}

	simulate_combat_rounds(combat_system, 25);
}

// Just call the update function on systems in the order it makes sense to
// within your use application of this library.
void simulate_combat_rounds(Combat_System* system, uint32_t count) {
	for (uint32_t i = 0; i < count; i++) {
		system->update(0);
	}
}