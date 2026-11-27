#include "Pig.h"
#include <cmath>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/object.hpp>
#include "Player.h"
#include <godot_cpp/classes/scene_tree.hpp>


using namespace godot;

void Pig::_bind_methods() {
 ClassDB::bind_method(D_METHOD("_on_atac_body_entered", "body"), &Pig::_on_atac_body_entered);
 ClassDB::bind_method(D_METHOD("_on_atac_body_exited", "body"), &Pig::_on_atac_body_exited);
 ClassDB::bind_method(D_METHOD("damage_enemy", "damage", "source"), &Pig::damage_enemy);
ClassDB::bind_method(D_METHOD("_on_animation_finished"), &Pig::_on_animation_finished);

 //ClassDB::bind_method(D_METHOD("dead"), &Pig::dead);
}


void Pig::detectar(Vector2 &vel) {

  if (ray_right == nullptr) {
        ray_right = get_node<RayCast2D>(NodePath("right"));
    }
    if (ray_left == nullptr) {
        ray_left = get_node<RayCast2D>(NodePath("left"));
    }
    if (anim_sprite == nullptr) {
        anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
    }

    bool vio_derecha = false;
    bool vio_izquierda = false;

    if (ray_right != nullptr && ray_right->is_colliding()) {
        Node *obj = Object::cast_to<Node>(ray_right->get_collider());
        if (obj != nullptr && obj->is_in_group("Player")) {
            vio_derecha = true;
        }
    }

    if (ray_left != nullptr && ray_left->is_colliding()) {
        Node *obj = Object::cast_to<Node>(ray_left->get_collider());
        if (obj != nullptr && obj->is_in_group("Player")) {
            vio_izquierda = true;
        }
    }
    
    if (vio_derecha) {
        perseguir = true;
        mirar_derecha = true;
        if (!atacando) {
            vel.x = VEL_PERSEGUIR;
            if (anim_sprite != nullptr) {
                anim_sprite->set_flip_h(true);
            }
        }
    } else if (vio_izquierda) {
        perseguir = true;
        mirar_derecha = false;
        if (!atacando) {
            vel.x = -VEL_PERSEGUIR;
            if (anim_sprite != nullptr) {
                anim_sprite->set_flip_h(false);
            }
        }
    } else {
        perseguir = false;
    }
}
void Pig::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
    atac_area = get_node<Area2D>(NodePath("Area2D"));
    ray_right = get_node<RayCast2D>(NodePath("right"));
    ray_left = get_node<RayCast2D>(NodePath("left"));
       //UtilityFunctions::print("Pig::_ready() -> patrulla_inicio_x: ");
        
    if (anim_sprite != nullptr) {
    Callable cb(this, "_on_animation_finished");
    if (!anim_sprite->is_connected("animation_finished", cb)) {
        anim_sprite->connect("animation_finished", cb);
    }
    }
    patrulla_inicio_x = get_global_position().x;/*
     UtilityFunctions::print(
           
            " | inicio: ", patrulla_inicio_x,
       
            " | wall: ", is_on_wall()
        );*/


   if (atac_area != nullptr) {
        Callable enter_cb(this, "_on_atac_body_entered");
        if (!atac_area->is_connected("body_entered", enter_cb)) {
            atac_area->connect("body_entered", enter_cb);
        }

        Callable exit_cb(this, "_on_atac_body_exited");
        if (!atac_area->is_connected("body_exited", exit_cb)) {
            atac_area->connect("body_exited", exit_cb);
        }
    }

    Vector2 vel = get_velocity();
    vel.x = -PIG_RUN_SPEED;
    mirar_derecha = false;
    set_velocity(vel);

    if (anim_sprite != nullptr) {
        anim_sprite->set_flip_h(false);
        anim_sprite->play("idle");
    } else {
        UtilityFunctions::push_warning("No se encontró el nodo hijo AnimatedSprite2D en Pig.");
    }
}
void Pig::damage_enemy(int damage, Node2D *source) {
    if (is_dead) {
        return;
    }

    health -= damage;
    if (health <= 0) {
        health = 0;
        is_dead = true;
        // Aquí llamas a tu lógica actual de muerte (animación Dead, desactivar colisiones, etc.)
        if (anim_sprite != nullptr) {
            anim_sprite->play("Dead");
        }
    if (get_tree() != nullptr && get_tree()->get_current_scene() != nullptr) {
    Player *player = Object::cast_to<Player>(
        get_tree()->get_current_scene()->find_child("Player", true, false)
    );
    if (player != nullptr) {
        player->add_score(100);
    }
}
        return;
    }

    hit_enemy(source);
}

void Pig::hit_enemy(Node2D *source) {
    is_hit = true;

    double knockback_x = 50.0;
    double knockback_y = -180.0;
    double dir_x = 0.0;

    if (source != nullptr) {
        // Si el jugador está a la izquierda del enemigo, empuja al enemigo a la derecha (+1)
        // Si el jugador está a la derecha del enemigo, empuja al enemigo a la izquierda (-1)
        double enemy_x = get_global_position().x;
        double source_x = source->get_global_position().x;
        dir_x = (source_x > enemy_x) ? -1.0 : 1.0;
    } else if (anim_sprite != nullptr) {
        dir_x = anim_sprite->is_flipped_h() ? 1.0 : -1.0;
    }

    set_velocity(Vector2(dir_x * knockback_x, knockback_y));

    if (anim_sprite != nullptr) {
        anim_sprite->play("Hit");
    }
}

// 4. En _on_animation_finished()

void Pig::_physics_process(double delta) {
   if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Vector2 vel = get_velocity();
    vel.y += GRAVEDAD;

    if (!is_on_floor()) {
        vel += get_gravity() * delta;
    }

    if (is_dead) {
        vel.x = 0.0;
        set_velocity(vel);
        move_and_slide();
        return;
    }

    if (is_hit) {
        // Mantiene el impulso horizontal del golpe mientras cae por gravedad
        set_velocity(vel);
        move_and_slide();
        return;
    }
    detectar(vel);
    /*
     UtilityFunctions::print(
            "Patrulla -> x: ", get_global_position().x
        );*/


   if (atacando) {
        vel.x = 0.0;
        if (anim_sprite != nullptr) {
            anim_sprite->set_flip_h(mirar_derecha);
        }
    } else if (perseguir) {
        vel.x = mirar_derecha ? VEL_PERSEGUIR : -VEL_PERSEGUIR;
        if (anim_sprite != nullptr) {
            anim_sprite->set_flip_h(mirar_derecha);
        }
    } else {
         const double x = get_global_position().x;
    const double limite_derecho = patrulla_inicio_x + DISTANCIA_PATRULLA;
    const double limite_izquierdo = patrulla_inicio_x - DISTANCIA_PATRULLA;
    

    if (is_on_wall() ||
        (mirar_derecha && x >= limite_derecho) ||
        (!mirar_derecha && x <= limite_izquierdo)) {
        mirar_derecha = !mirar_derecha;
    }

    vel.x = mirar_derecha ? PIG_RUN_SPEED : -PIG_RUN_SPEED;
    /*
    UtilityFunctions::print(
            "Patrulla -> x: ", x,
            " | inicio: ", patrulla_inicio_x,
            " | izq: ", limite_izquierdo,
            " | der: ", limite_derecho,
            " | mirar_der: ", mirar_derecha,
            " | wall: ", is_on_wall()
        );*/

    if (anim_sprite != nullptr) {
        anim_sprite->set_flip_h(mirar_derecha);
    }
    
    }

    set_velocity(vel);
    move_and_slide();
}
void Pig::_on_atac_body_entered(Node *body) {
    if (body == nullptr|| is_hit || is_dead) {
        return;
    }

    if (body->is_in_group("Player")) {
        atacando = true;
        if (anim_sprite == nullptr) {
            anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
        }
        if (anim_sprite != nullptr) {
            anim_sprite->set_flip_h(mirar_derecha);
            anim_sprite->play("attack");
        }
    }
}

void Pig::_on_atac_body_exited(Node *body) {
    
    if (body == nullptr) {
        return;
    }

    if (body->is_in_group("Player")) {
        atacando = false;
        if (anim_sprite == nullptr) {
            anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
        }
        if (anim_sprite != nullptr) {
            anim_sprite->play("idle");
        }
    }
}
void Pig::_on_animation_finished() {
    if (anim_sprite == nullptr) {
        return;
    }

    StringName anim_name = anim_sprite->get_animation();
    if (anim_name == StringName("Hit") && !is_dead) {
        is_hit = false;
        anim_sprite->play("idle"); // O volver a correr/patrullar
    } else if (anim_name == StringName("Dead")) {
        queue_free();
    }
}