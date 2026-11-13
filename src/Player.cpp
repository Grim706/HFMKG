#include "Player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>

using namespace godot;

void Player::_bind_methods() {
    // Si quieres exponer variables al editor de Godot, se registran aquí.
}

Player::Player() {}

Player::~Player() {}
void Player::_ready() {
    /*
    // Busca un nodo hijo llamado "AnimatedSprite2D" (cambia el nombre si el tuyo se llama distinto)
    animated_sprite = get_node<AnimatedSprite2D>("AnimatedSprite2D");
    if (animated_sprite) {
        UtilityFunctions::print("¡EXITO: Nodo AnimatedSprite2D encontrado!");

        // 2. Comprobar si tiene asignado el recurso de SpriteFrames
        Ref<SpriteFrames> frames = animated_sprite->get_sprite_frames();
        if (frames.is_valid()) {
            UtilityFunctions::print("¡EXITO: El recurso SpriteFrames esta cargado!");

            // 3. Comprobar si existen las animaciones que intentas reproducir
            if (frames->has_animation("Idle")) {
                UtilityFunctions::print("-> Animacion 'Idle' encontrada.");
            } else {
                UtilityFunctions::print("-> ADVERTENCIA: La animacion 'Idle' NO existe en el SpriteFrames.");
            }

            if (frames->has_animation("jump")) {
                UtilityFunctions::print("-> Animacion 'jump' encontrada.");
            } else {
                UtilityFunctions::print("-> ADVERTENCIA: La animacion 'jump' NO existe en el SpriteFrames.");
            }
        } else {
            UtilityFunctions::print("-> ERROR: El AnimatedSprite2D no tiene un SpriteFrames asignado en el inspector de Godot.");
        }
    } else {
        UtilityFunctions::print("-> ERROR: No se encontro el nodo AnimatedSprite2D en la escena.");
    }*/
}
void Player::_physics_process(double delta) {
    Input *input = Input::get_singleton();

    // Añadir la gravedad
    if (!is_on_floor()) {
        Vector2 gravity = get_gravity();
        set_velocity(get_velocity() + gravity * delta);
    }
   
    // Manejar el salto
    if (input->is_action_just_pressed("ui_accept") && is_on_floor()) {
        Vector2 velocity = get_velocity();
        velocity.y = jump_velocity;
        set_velocity(velocity);
    }

    // Obtener la dirección de entrada y manejar movimiento/desaceleración
    double direction = input->get_axis("ui_left", "ui_right");
    Vector2 velocity = get_velocity();
    
    if (direction != 0.0) {
        velocity.x = direction * speed;
    } else {
        // move_toward equivalente en C++ para Godot
        velocity.x = Math::move_toward(velocity.x, 0.0, speed * delta); // Nota: ajusta según tu fricción
    }
    
    set_velocity(velocity);
    move_and_slide();

    /*
if (animated_sprite) {
        if (!is_on_floor() && get_velocity().y < 0) {
            animated_sprite->play("jump"); // Reproduce la animación de salto
        } 
        else if (direction != 0) {
            animated_sprite->play("walk"); // Reproduce la animación de caminar
            animated_sprite->set_flip_h(direction < 0); // Voltea el sprite según la dirección
        } 
        else {
            animated_sprite->play("Idle"); // Reproduce la animación por defecto
        }
    }*/
}
