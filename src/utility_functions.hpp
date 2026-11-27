#include "Pig.h"

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/vector2.hpp>

using namespace godot;

void Pig::_ready() {
    UtilityFunctions::print("Pig::_ready() ejecutado");

    anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));

    if (anim_sprite == nullptr) {
        UtilityFunctions::push_warning("No se encontro el nodo hijo AnimatedSprite2D en Pig.");
        return;
    }

    UtilityFunctions::print("Reproduciendo animacion idle en Pig...");
    anim_sprite->play("idle");
}