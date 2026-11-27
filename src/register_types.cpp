#include "register_types.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

#include "Player.h"
#include "Menu.h"
#include "Pig.h"
#include "Proyectil.h"
#include "Bandido.h"
#include "GameOverScreen.h"
#include "GameState.h"
#include "register_types.h"
#include "score_manager.hpp"



using namespace godot;
static GameState *game_state_instance = nullptr;

void initialize_gdextension_types(ModuleInitializationLevel p_level)
{
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	GDREGISTER_CLASS(Player);
	GDREGISTER_CLASS(Menu);
	GDREGISTER_CLASS(Pig);
	GDREGISTER_CLASS(Proyectil);
	GDREGISTER_CLASS(Bandido);
	GDREGISTER_CLASS(GameOverScreen);
	GDREGISTER_CLASS(GameState);
	//GDREGISTER_CLASS(ScoreManager);
	if (game_state_instance == nullptr) {
		game_state_instance = memnew(GameState);
	}
}

void uninitialize_gdextension_types(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
	if (game_state_instance != nullptr) {
		memdelete(game_state_instance);
		game_state_instance = nullptr;
	}
}

extern "C"
{
	// Initialization
	GDExtensionBool GDE_EXPORT hfmkg_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization)
	{
		GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
		init_obj.register_initializer(initialize_gdextension_types);
		init_obj.register_terminator(uninitialize_gdextension_types);
		init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

		return init_obj.init();
	}
}