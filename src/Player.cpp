#include "Player.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/typed_array.hpp> 
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/node.hpp> 
 #include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/typed_array.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/window.hpp>
#include "GameOverScreen.h"
using namespace godot;

void Player::_bind_methods() {
    ClassDB::bind_method(D_METHOD("damage_player", "damage", "source"), &Player::damage_player);
    ClassDB::bind_method(D_METHOD("_on_hurtbox_area_entered", "area"), &Player::_on_hurtbox_area_entered);

    ClassDB::bind_method(D_METHOD("set_health", "p_health"), &Player::set_health);
    ClassDB::bind_method(D_METHOD("get_health"), &Player::get_health);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "health"), "set_health", "get_health");

    ClassDB::bind_method(D_METHOD("_on_animation_finished"), &Player::_on_animation_finished);
    ClassDB::bind_method(D_METHOD("_on_area_2d_body_entered", "body"), &Player::_on_area_2d_body_entered);


    ClassDB::bind_method(D_METHOD("set_health_bar_path", "p_path"), &Player::set_health_bar_path);
    ClassDB::bind_method(D_METHOD("get_health_bar_path"), &Player::get_health_bar_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "health_bar_path"), "set_health_bar_path", "get_health_bar_path");
}

Player::Player() {}
void Player::set_health(int p_health) {
    health = p_health;
    if (health < 0) {
        health = 0;
    }
    actualizar_barra_vida();
}

int Player::get_health() const {
    return health;
}

void Player::set_health_bar_path(const NodePath &p_path) {
    health_bar_path = p_path;
    health_bar = nullptr;
    actualizar_barra_vida();
}

NodePath Player::get_health_bar_path() const {
    return health_bar_path;
}

void Player::actualizar_barra_vida() {
    if (!is_inside_tree()) {
        return;
    }

    if (health_bar == nullptr && !health_bar_path.is_empty()) {
        health_bar = get_node<TextureProgressBar>(health_bar_path);
    }

    if (health_bar != nullptr) {
        health_bar->set_value(static_cast<double>(health));
    }
}
Player::~Player() {}



void Player::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    area_collision = get_node<CollisionShape2D>(NodePath("Area2D/CollisionShape2D"));
    area_2d = get_node<Area2D>(NodePath("Area2D"));
    if (area_2d != nullptr) {
        Callable cb(this, "_on_area_2d_body_entered");
        if (!area_2d->is_connected("body_entered", cb)) {
            area_2d->connect("body_entered", cb);
        }
    }
    actualizar_barra_vida();
    hurtbox = get_node<Area2D>(NodePath("Hurtbox"));
    if (hurtbox != nullptr) {
        Callable cb(this, "_on_hurtbox_area_entered");
        if (!hurtbox->is_connected("area_entered", cb)) {
            hurtbox->connect("area_entered", cb);
        }
    }
    anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
    if (anim_sprite != nullptr) {
    Callable cb(this, "_on_animation_finished");
    if (!anim_sprite->is_connected("animation_finished", cb)) {
        anim_sprite->connect("animation_finished", cb);
    }
    }
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
void Player:: damage_player(int damage, Node2D *source) {
    health -= damage;
    if (health < 0 || health == 0) {
        health = 0;
        Game_Over();
        return;
    }
    atackar = false;
     area_collision->set_deferred("disabled", true);
    HitPlayer(source);
    actualizar_barra_vida();
    UtilityFunctions::print("Vida del jugador: ", health);
}
void Player::_on_hurtbox_area_entered(Area2D *area) {
    if (area == nullptr) {
        return;
    }

    Node *parent = area->get_parent();
    if (parent != nullptr && parent->is_in_group("enemy")) {
        Variant dmg_var = parent->get("damage");
        int dmg = (dmg_var.get_type() != Variant::NIL) ? int(dmg_var) : 10;
       Node2D *source_2d = Object::cast_to<Node2D>(parent);
     damage_player(dmg, source_2d);
    }
}
void Player::_physics_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    Input *input = Input::get_singleton();
    Vector2 velocity = get_velocity();

    // Gravedad
    if (!is_on_floor()) {
        velocity += get_gravity() * delta;
    }
    if (hitPlayer) {
    // Mantiene el empuje horizontal del golpe y solo aplica gravedad/salto vertical
    set_velocity(velocity);
    move_and_slide();
    return;
    }
   if (!atackar) {
        // Salto
        if (input->is_action_just_pressed("ui_accept") && is_on_floor()) {
            velocity.y = jump_velocity;
        }

        // Movimiento horizontal
      double direction = input->get_axis("ui_left", "ui_right");
    if (direction != 0.0) {
    velocity.x = direction * speed;

    bool mirando_izquierda = (direction < 0.0);
    if (anim_sprite != nullptr) {
        anim_sprite->set_flip_h(mirando_izquierda);
    }

    /* if (collision_shape != nullptr) {
        Vector2 p = collision_shape->get_position();
        p.x = mirando_izquierda ? -21.54 : 20;
        collision_shape->set_position(p);
    }*/
   

    if (area_2d != nullptr) {
        Vector2 p = area_2d->get_position();
        p.x = mirando_izquierda ? -40.56 : 0.0;
        area_2d->set_position(p);
    }
} else {
    velocity.x = 0.0;
}

        // Iniciar ataque
     if (input->is_action_just_pressed("atack") && !hitPlayer) {
    atackar = true;
    if (area_collision != nullptr) {
        area_collision->set_deferred("disabled", false);
    }
    velocity.x = 0.0;
    if (anim_sprite != nullptr) {
        anim_sprite->play("Atack");
    }
  if (area_2d != nullptr) {
    TypedArray<Node2D> overlapping = area_2d->get_overlapping_bodies();
    for (int i = 0; i < overlapping.size(); i++) {
        Node *body = Object::cast_to<Node>(overlapping[i]);
        if (body != nullptr) {
            evaluar_cuerpo_golpe(body);
        }
    }
}
}
    } else {
        velocity.x = 0.0;
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
bool Player::_on_animation_finished() {
   
    if (anim_sprite != nullptr && (anim_sprite->get_animation() == StringName("Atack"))) {
         anim_sprite->play("Idle");
         area_collision->set_deferred("disabled", true);
         atackar = false;
        return true;
    }
    if(anim_sprite !=nullptr && (anim_sprite->get_animation() == StringName("Hit"))){
        anim_sprite->play("Idle");
        hitPlayer = false;
        velocity = Vector2(0, 0);
        return true;
    }
    return false;
}
void Player::HitPlayer(Node2D *source) {
    hitPlayer = true;

    double knockback_x = 50.0;
    double knockback_y = -200.0;
    double dir_x = 0.0;

    if (source != nullptr) {
      Variant dir_var = source->get("direccion");
        if (dir_var.get_type() == Variant::VECTOR2) {
            Vector2 d = dir_var;
            if (std::abs(d.x) > 0.001) {
                dir_x = (d.x > 0.0) ? 1.0 : -1.0;
            }
        }

        // 2. Respaldo por velocidad si el nodo origen usa "velocity"
        if (dir_x == 0.0) {
            Variant vel_var = source->get("velocity");
            if (vel_var.get_type() == Variant::VECTOR2) {
                Vector2 v = vel_var;
                if (std::abs(v.x) > 1.0) {
                    dir_x = (v.x > 0.0) ? 1.0 : -1.0;
                }
            }
        }
    } else if (anim_sprite != nullptr) {
        UtilityFunctions::print("no colo: ", health);

        dir_x = anim_sprite->is_flipped_h() ? 1.0 : -1.0;
    }

    set_velocity(Vector2(dir_x * knockback_x, knockback_y));

    if (anim_sprite != nullptr) {
        anim_sprite->play("Hit");
    }
}   
// En Player.cpp
void Player::_on_area_2d_body_entered(Node *body) {
    evaluar_cuerpo_golpe(body);
}

void Player::evaluar_cuerpo_golpe(Node *body) {
    if (body == nullptr) {
        return;
    }
   //atack
  if (body->is_in_group("enemy")) {
        if (body->has_method("damage_enemy")) {
            body->call("damage_enemy", 20, this); // 20 de daño, y 'this' es el Player
        }
    }
}
void Player::Game_Over() {
    // Deshabilita el área de colisión para que no pueda recibir más daño
    if (area_collision != nullptr) {
        area_collision->set_deferred("disabled", true);
    }

    // Cambia la escena a la pantalla de Game Over
    // En Player.cpp justo antes de ir a game_over:
Ref<PackedScene> scene = ResourceLoader::get_singleton()->load("res://Scenes/game_over.tscn");
if (scene.is_valid() && get_tree() != nullptr) {
    Node *instance = scene->instantiate();
    GameOverScreen *screen = Object::cast_to<GameOverScreen>(instance);
    if (screen != nullptr) {
        get_tree()->get_root()->add_child(screen);
        screen->set_score(score);
        if (get_tree()->get_current_scene() != nullptr) {
            get_tree()->get_current_scene()->queue_free();
        }
        get_tree()->set_current_scene(screen);
    }
}
}
