#if !defined(SHF_ECS_H)
#define SHF_ECS_H

#if defined(_WIN32)
#if defined(BUILD_LIBYTPE_SHARED)
#define SHF_ECS_API __declspec(dllexport)
#elif defined(USE_LIBTYPE_SHARED)
#define SHF_ECS_API __declspec(dllimport)
#else
#define SHF_ECS_API 
#endif // dll check
#else
#define SHF_ECS_API 
#endif // API decls

#define SHF_ECS_MAX_ENTITY_COUNT    65535
#define SHF_ECS_MAX_COMPONENT_TYPES 32

#include <varargs.h>
#include <stdint.h>
#include <bitset>
#include <set>

namespace shf {
	namespace ecs {
		typedef uint32_t                                 Entity;
		typedef std::bitset<SHF_ECS_MAX_COMPONENT_TYPES> Component_Signature;

		struct Component {
			
		};

		struct System {
			std::set<Entity> entities;
			uint32_t         type_id = -1;

			template<typename T>
			void SHF_ECS_API track_component_type();

			virtual void update(float delta_time) = 0;
		};

		template <typename T>
		SHF_ECS_API void add_component(Entity e, T comp);

		Entity create_entity();
		SHF_ECS_API void   destroy_entity(Entity e);

		template <typename T>
		SHF_ECS_API T* get_component(Entity e);

		template <typename T>
		SHF_ECS_API void register_component();

		template <typename T>
		SHF_ECS_API T* register_system();

		template <typename T>
		SHF_ECS_API void remove_component(Entity e);
	}
}

#endif // SHF_ECS_H

#if defined(SHF_ECS_IMPL)

#include <assert.h>
#include <stdio.h>

#include <array>
#include <unordered_map>
#include <queue>

namespace shf {
	namespace ecs {
		typedef struct Component_Manager Component_Manager;
		typedef struct Entity_Manager Entity_Manager;
		typedef struct System_Manager System_Manager;

		static Component_Manager*	get_component_manager();
		static Entity_Manager*		get_entity_manager();
		static System_Manager*		get_system_manager();

		enum Notification_Type {
			Notification_Type_Undefined = 0,
			
			Notification_Type_Entity_Component_Update,
			Notification_Type_Entity_Created,
			Notification_Type_Entity_Destroyed,

			Notification_Type_Count
		};

		typedef void* I_System;

		struct I_Component_Table {
			virtual void notify(Notification_Type type, Entity e) = 0;
		};

		template <typename T>
		struct Component_Table : public I_Component_Table {
			std::array<T, SHF_ECS_MAX_ENTITY_COUNT> packed_components;
			std::unordered_map<Entity, uint32_t>    entity_to_packed_index_map;
			std::unordered_map<uint32_t, Entity>    packed_index_to_entity_map;

			uint32_t component_count = 0;

			void add_component(Entity e, T comp) {
				assert(entity_to_packed_index_map.find(e) == entity_to_packed_index_map.end() && "[SHF ECS]: add_component<T> - Component already exists for entity.");

				uint32_t new_component_index = component_count;
				entity_to_packed_index_map.insert({ e, new_component_index });
				packed_index_to_entity_map.insert({ new_component_index, e });
				packed_components[new_component_index] = comp;
				component_count++;
			}

			void notify(Notification_Type type, Entity e) {
				switch (type) {
					case Notification_Type_Entity_Destroyed: {
						remove_component(e);
					} break;
				}
			}

			void remove_component(Entity e) {
				assert(this->entity_to_packed_index_map.find(e) != this->entity_to_packed_index_map.end() && "[SHF ECS]: remove_component<%s> - Entity doesn't own component of requested type.");

				uint32_t packed_index_of_removed_entity = entity_to_packed_index_map[e];
				uint32_t packed_index_of_last_element = component_count - 1;

				packed_components[packed_index_of_removed_entity] = packed_components[packed_index_of_last_element];

				Entity last_entity = packed_index_to_entity_map[packed_index_of_last_element];
				entity_to_packed_index_map[last_entity] = packed_index_of_removed_entity;
				packed_index_to_entity_map[packed_index_of_removed_entity] = last_entity;

				entity_to_packed_index_map.erase(e);
				packed_index_to_entity_map.erase(packed_index_of_last_element);
			}
		};

		struct Component_Manager {
			Component_Manager() {
		
			}

			uint32_t registered_component_type_count = 0;
			std::unordered_map<const char*, uint32_t>        type_name_table;
			std::unordered_map<uint32_t, I_Component_Table*> component_table_map;

			void notify(Notification_Type type, Entity e) {
				for (int i = 0; i < registered_component_type_count; i++) {
					auto pair = component_table_map.find(i);
					if (pair == component_table_map.end()) continue;

					I_Component_Table* component_table = (I_Component_Table*)pair->second;
					component_table->notify(type, e);
				}
			}
		};

		struct Entity_Manager {
			Entity_Manager() {
				for (uint32_t i = 0; i < SHF_ECS_MAX_ENTITY_COUNT; i++) available_entity_queue.push(i);
			}

			std::queue<Entity> available_entity_queue;
			std::array<Component_Signature, SHF_ECS_MAX_ENTITY_COUNT> entity_signature_table;
			uint32_t entity_count = 0;
		};

		struct System_Manager {
			System_Manager() {
		
			}

			uint32_t registered_system_type_count = 0;
			std::unordered_map<const char*, uint32_t>         type_name_table;
			std::unordered_map<uint32_t, I_System>	          system_table;
			std::unordered_map<uint32_t, Component_Signature> system_signature_map;

			void notify(Notification_Type type, Entity e) {
				switch (type) {
					case Notification_Type_Entity_Component_Update: {
						for (int i = 0; i < registered_system_type_count; i++) {
							auto pair = system_table.find(i);
							if (pair == system_table.end()) continue;

							System* system = (System*)pair->second;
							Component_Signature system_signature = system_signature_map[pair->first];
							Component_Signature entity_signature = get_entity_manager()->entity_signature_table[e];

							if ((entity_signature & system_signature) == system_signature) {
								system->entities.insert(e);
							} else {
								system->entities.erase(e);
							}
						}
					} break;

					case Notification_Type_Entity_Destroyed: {
						for (int i = 0; i < registered_system_type_count; i++) {
							auto pair = system_table.find(i);
							if (pair == system_table.end()) continue;

							System* system = (System*)pair->second;
							system->entities.erase(e);
						}
					} break;
				}
			}
		};

		static Component_Manager*	_component_manager = 0;
		static Entity_Manager*		_entity_manager = 0;
		static System_Manager*		_system_manager = 0;

		static Component_Manager* get_component_manager() {
			if (!_component_manager) _component_manager = new Component_Manager();

			return _component_manager;
		}

		static Entity_Manager* get_entity_manager() {
			if (!_entity_manager) _entity_manager = new Entity_Manager();

			return _entity_manager;
		}

		static System_Manager* get_system_manager() {
			if (!_system_manager) _system_manager = new System_Manager();

			return _system_manager;
		}	

		template <typename T>
		void System::track_component_type() {
			static_assert(std::is_base_of<Component, T>::value && "[SHF ECS]: System::track_component_type<T> - T must derive from shf::ecs::Component");

			assert(type_id != -1 && "[SHF ECS]:  System::track_component_type<T> - System not registered.");
			assert(get_component_manager()->type_name_table.find(typeid(T).name()) != get_component_manager()->type_name_table.end() && "[SHF ECS]:  System::track_component_type<T> - Component type not registered.");

			uint32_t component_type_index = get_component_manager()->type_name_table[typeid(T).name()];
			get_system_manager()->system_signature_map[type_id].set(component_type_index, true);
		}

		template <typename T>
		void add_component(Entity e, T comp) {
			static_assert(std::is_base_of<Component, T>::value && "[SHF ECS]: add_component<T> - T must derive from shf::ecs::Component");
			assert(get_component_manager()->type_name_table.find(typeid(T).name()) != get_component_manager()->type_name_table.end() && "[SHF ECS]: add_component<T> - Component type not registered.");

			uint32_t component_type_index = get_component_manager()->type_name_table[typeid(T).name()];
			Component_Table<T>* component_table_for_type = (Component_Table<T>*) get_component_manager()->component_table_map[component_type_index];
			component_table_for_type->add_component(e, comp);

			get_entity_manager()->entity_signature_table[e].set(component_type_index, true);
			get_component_manager()->notify(Notification_Type_Entity_Component_Update, e);
			get_system_manager()->notify(Notification_Type_Entity_Component_Update, e);
		}

		Entity create_entity() {
			assert(get_entity_manager()->entity_count < SHF_ECS_MAX_ENTITY_COUNT && "[SHF ECS]: create_entity() - Maximum entity capacity reached.");

			Entity new_entity = get_entity_manager()->available_entity_queue.front();
			get_entity_manager()->available_entity_queue.pop();
			get_entity_manager()->entity_signature_table[new_entity].reset();
			get_entity_manager()->entity_count++;

			return new_entity;
		}

		void destroy_entity(Entity e) {
			assert(e < get_entity_manager()->entity_count && "[SHF ECS]: destroy_entity(%u) - Entity ID exceeds current registered bounds." && e);

			get_system_manager()->notify(Notification_Type_Entity_Destroyed, e);
			get_component_manager()->notify(Notification_Type_Entity_Destroyed, e);

			get_entity_manager()->entity_signature_table[e].reset();
			get_entity_manager()->available_entity_queue.push(e);
			get_entity_manager()->entity_count--;
		}

		template <typename T>
		T* get_component(Entity e) {
			assert(get_component_manager()->type_name_table.find(typeid(T).name()) != get_component_manager()->type_name_table.end() && "[SHF ECS]: get_component<%s> - Component type not registered." && typeid(T).name());
			
			uint32_t component_type_index = get_component_manager()->type_name_table[typeid(T).name()];
			Component_Table<T>* component_table_for_type = (Component_Table<T>*) get_component_manager()->component_table_map[component_type_index];

			assert(component_table_for_type->entity_to_packed_index_map.find(e) != component_table_for_type->entity_to_packed_index_map.end() && "[SHF ECS]: get_component<%s> - Entity doesn't own component of requested type." && typeid(T).name());

			return &component_table_for_type->packed_components[component_table_for_type->entity_to_packed_index_map[e]];
		}

		template <typename T>
		void register_component() {
			static_assert(std::is_base_of<Component, T>::value && "[SHF ECS]: register_component<T> - T must derive from shf::ecs::Component");
			assert(get_component_manager()->registered_component_type_count < SHF_ECS_MAX_COMPONENT_TYPES && "[SHF ECS]: register_component<%s> - Maximum component types reached." && typeid(T).name());

			const char* component_type_name = typeid(T).name();
			auto entry = get_component_manager()->type_name_table.find(component_type_name);
			if (entry != get_component_manager()->type_name_table.end()) return; // Already registered

			Component_Table<T>* new_table_for_type = new Component_Table<T>();
			get_component_manager()->type_name_table.insert({ component_type_name, get_component_manager()->registered_component_type_count });
			get_component_manager()->component_table_map.insert({ get_component_manager()->registered_component_type_count, new_table_for_type });
			get_component_manager()->registered_component_type_count++;
		}

		template <typename T>
		T* register_system() {
			static_assert(std::is_base_of<System, T>::value && "[SHF ECS]: register_component<T> - T must derive from shf::ecs::System");

			const char* system_type_name = typeid(T).name();
			auto entry = get_system_manager()->type_name_table.find(system_type_name);
			if (entry != get_system_manager()->type_name_table.end()) return (T*)get_system_manager()->system_table[entry->second];


			T* new_system = new T();
			uint32_t new_system_id = get_system_manager()->registered_system_type_count;
			get_system_manager()->system_signature_map[new_system_id].reset();
			get_system_manager()->type_name_table.insert({ system_type_name, new_system_id });
			get_system_manager()->system_table.insert({ new_system_id, new_system });
			((System*)new_system)->type_id = new_system_id;
			get_system_manager()->registered_system_type_count++;

			return new_system;
		}

		template <typename T>
		void remove_component(Entity e) {
			static_assert(std::is_base_of<Component, T>::value && "[SHF ECS]: remove_component<%s> - T must derive from shf::ecs::Component");
			assert(get_component_manager()->type_name_table.find(typeid(T).name()) != get_component_manager()->type_name_table.end() && "[SHF ECS]: remove_component<%s> - Component type not registered." && typeid(T).name());

			uint32_t component_type_index = get_component_manager()->type_name_table[typeid(T).name()];
			Component_Table<T>* component_table_for_type = (Component_Table<T>*) get_component_manager()->component_table_map[component_type_index];
			component_table_for_type->remove_component(e);

			get_entity_manager()->entity_signature_table[e].set(component_type_index, false);
			get_system_manager()->notify(Notification_Type_Entity_Component_Update, e);
		}
	}
}
#endif
