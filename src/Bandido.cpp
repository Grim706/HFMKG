#include "Bandido.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include "GameState.h"
#include "Player.h"
#include <godot_cpp/classes/scene_tree.hpp>
using namespace godot;

void Bandido::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_speed", "p_speed"), &Bandido::set_speed);
    ClassDB::bind_method(D_METHOD("get_speed"), &Bandido::get_speed);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed"), "set_speed", "get_speed");

    ClassDB::bind_method(D_METHOD("set_proyectil_scene_path", "p_path"), &Bandido::set_proyectil_scene_path);
    ClassDB::bind_method(D_METHOD("get_proyectil_scene_path"), &Bandido::get_proyectil_scene_path);
    ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "proyectil_scene_path"), "set_proyectil_scene_path", "get_proyectil_scene_path");

    ClassDB::bind_method(D_METHOD("_on_vision_body_entered", "body"), &Bandido::_on_vision_body_entered);
    ClassDB::bind_method(D_METHOD("_on_vision_body_exited", "body"), &Bandido::_on_vision_body_exited);
    ClassDB::bind_method(D_METHOD("_on_timer_timeout"), &Bandido::_on_timer_timeout);
     //ClassDB::bind_method(D_METHOD("_on_atac_body_entered", "body"), &Bandido::_on_atac_body_entered);
   // ClassDB::bind_method(D_METHOD("_on_atac_body_exited", "body"), &Bandido::_on_atac_body_exited);
    ClassDB::bind_method(D_METHOD("_on_animation_finished"), &Bandido::_on_animation_finished);

    ClassDB::bind_method(D_METHOD("damage_enemy", "damage", "source"), &Bandido::damage_enemy);
}

void Bandido::_ready() {
    if (!proyectil_scene_path.is_empty()) {
    String ruta = String(proyectil_scene_path);
    proyectil_scene = ResourceLoader::get_singleton()->load(ruta);
    if (proyectil_scene.is_valid()) {
        UtilityFunctions::print("Bandido: proyectil cargado correctamente desde -> ", ruta);
    } else {
        UtilityFunctions::print("Bandido: ERROR al cargar proyectil desde -> ", ruta);
    }
} else {
    UtilityFunctions::print("Bandido: proyectil_scene_path esta vacio");
}
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    marker = get_node<Marker2D>(NodePath("Marker2D"));
    timer = get_node<Timer>(NodePath("Timer"));
    vision = get_node<Area2D>(NodePath("Vision"));
    anim_sprite = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
  

    if (anim_sprite != nullptr) {
    Callable cb(this, "_on_animation_finished");
    if (!anim_sprite->is_connected("animation_finished", cb)) {
        anim_sprite->connect("animation_finished", cb);
    }
    }
    
    if (!proyectil_scene_path.is_empty()) {
        proyectil_scene = ResourceLoader::get_singleton()->load(String(proyectil_scene_path));
    }

    if (vision != nullptr) {
        Callable enter_cb(this, "_on_vision_body_entered");
        if (!vision->is_connected("body_entered", enter_cb)) {
            vision->connect("body_entered", enter_cb);
        }

        Callable exit_cb(this, "_on_vision_body_exited");
        if (!vision->is_connected("body_exited", exit_cb)) {
            vision->connect("body_exited", exit_cb);
        }
    }

    if (timer != nullptr) {
        Callable timeout_cb(this, "_on_timer_timeout");
        if (!timer->is_connected("timeout", timeout_cb)) {
            timer->connect("timeout", timeout_cb);
        }
    }
}

void Bandido::seguir_jugador() {
    Vector2 vel = get_velocity();
    vel.y += GRAVEDAD;

    if (is_dead) {
        vel.x = 0.0;
        set_velocity(vel);
        return;
    }

    if (is_hit) {
        // Conserva el retroceso horizontal del golpe mientras aplica gravedad
       
        set_velocity(vel);
        return;
    }

    if (player != nullptr && puede_disparar) {
        double dx = player->get_global_position().x - get_global_position().x;
        if (std::abs(dx) > 8.0) {
            mirar_derecha = (dx > 0.0);
            vel.x = mirar_derecha ? speed : -speed;

            if (anim_sprite != nullptr) {
                // Ajusta a !mirar_derecha si el sprite base del Bandido mira a la derecha
                anim_sprite->set_flip_h(mirar_derecha);
            }

            if (marker != nullptr) {
                Vector2 m_pos = marker->get_position();
                m_pos.x = mirar_derecha ? std::abs(m_pos.x) : -std::abs(m_pos.x);
                marker->set_position(m_pos);
            }
        } else {
            vel.x = 0.0;
        }
    } else {
        vel.x = 0.0;
    }

    set_velocity(vel);
}

void Bandido::_on_vision_body_entered(Node *body) {
    if (body == nullptr || is_hit || is_dead) {
        return;
    }

    if (body->is_in_group("Player")) {
        player = Object::cast_to<Node2D>(body);
        puede_disparar = true;

        if (player != nullptr) {
            double dx = player->get_global_position().x - get_global_position().x;
            mirar_derecha = (dx >= 0.0);
        }

        if (anim_sprite != nullptr) {
            anim_sprite->set_flip_h(mirar_derecha);
          //  anim_sprite->play("attack");
        }
    }
}

void Bandido::disparar() {
   if (!puede_disparar || is_hit || is_dead || player == nullptr || marker == nullptr || !proyectil_scene.is_valid()) {
        return;
    }
//UtilityFunctions::print("DISPARANDO bala desde Marker2D!");
    Node *instancia = proyectil_scene->instantiate();
    Node2D *bala = Object::cast_to<Node2D>(instancia);
    if (bala == nullptr) {
        if (instancia != nullptr) {
          //  instancia->queue_free();
        }
        return;
    }

   Vector2 origen = marker->get_global_position();
    double dx = player->get_global_position().x - origen.x;
    Vector2 dir = (dx >= 0.0) ? Vector2(1.0, 0.0) : Vector2(-1.0, 0.0);

    Node *parent = get_parent();
    if (parent != nullptr) {
        parent->add_child(bala);
    } else {
        add_child(bala);
    }

    bala->set_global_position(origen);
    bala->set("direccion", dir);
    /*
    UtilityFunctions::print(
    "Bandido global: ", get_global_position(),
    " | Marker global: ", marker->get_global_position(),
    " | Bala global: ", bala->get_global_position()
);
    UtilityFunctions::print(
        "Bala instanciada -> clase: ", bala->get_class(),
        " pos_global: ", bala->get_global_position(),
        " dir: ", dir
    );*/
}

void Bandido::_physics_process(double delta) {
  if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    seguir_jugador();
    move_and_slide();
}

void Bandido::set_speed(double p_speed) {
    speed = p_speed;
}

double Bandido::get_speed() const {
    return speed;
}

void Bandido::set_proyectil_scene_path(const NodePath &p_path) {
    proyectil_scene_path = p_path;
    if (Engine::get_singleton() != nullptr && !Engine::get_singleton()->is_editor_hint() && !proyectil_scene_path.is_empty()) {
        proyectil_scene = ResourceLoader::get_singleton()->load(String(proyectil_scene_path));
    }
}

NodePath Bandido::get_proyectil_scene_path() const {
    return proyectil_scene_path;
}
void Bandido::_on_vision_body_exited(Node *body) {
   if (body == nullptr || is_hit || is_dead) {
        return;
    }

    if (body == player || body->is_in_group("Player")) {
        puede_disparar = false;
        player = nullptr;
        if (anim_sprite != nullptr) {
            anim_sprite->play("idle");
        }
    }
}



void Bandido::_on_timer_timeout() {
//UtilityFunctions::print("Timer timeout -> puede_disparar: ", puede_disparar, " player_valido: ", (player != nullptr));
   if (puede_disparar && !is_hit && !is_dead && player != nullptr) {
        disparar();
    }
}

/*void Bandido::_on_atac_body_entered(Node *body) {
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
}*/



void Bandido::_on_animation_finished() {
    if (anim_sprite == nullptr) {
        return;
    }

    StringName anim_name = anim_sprite->get_animation();
    if (anim_name == StringName("Hit") && !is_dead) {
         UtilityFunctions::print(
    "si"
    );
       is_hit = false;
    Vector2 v = get_velocity();
    v.x = 0.0;
    set_velocity(v);
    anim_sprite->play("idle");
    
    if (vision != nullptr) {
            TypedArray<Node2D> bodies = vision->get_overlapping_bodies();
            for (int i = 0; i < bodies.size(); i++) {
                Node2D *b = Object::cast_to<Node2D>(bodies[i]);
                if (b != nullptr && b->is_in_group("Player")) {
                    player = b;
                    puede_disparar = true;
                    break;
                }
            }
        }
    } else if (anim_name == StringName("Dead")) {
        queue_free();
    }
}

void Bandido::damage_enemy(int damage, Node2D *source) {
    UtilityFunctions::print("Bandido damage_enemy recibido -> dmg: ", damage, " health antes: ", health);
    if (is_dead) {
        return;
    }

    health -= damage;
    if (health <= 0) {
        health = 0;
        is_dead = true;
        puede_disparar = false;
        atacando = false;
        set_velocity(Vector2(0.0, get_velocity().y));

        // Desactiva colisiones de forma diferida para evitar errores durante consultas físicas
        if (vision != nullptr) {
            vision->set_deferred("monitoring", false);
        }

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

void Bandido::hit_enemy(Node2D *source) {
    is_hit = true;
    atacando = false;

    double knockback_x = 60.0;
    double knockback_y = -180.0;
    double dir_x = 0.0;

    double bandido_x = get_global_position().x;

    if (source != nullptr) {
        double source_x = source->get_global_position().x;
        if (source_x > bandido_x) {
            dir_x = -1.0;
        } else if (source_x < bandido_x) {
            dir_x = 1.0;
        }
    }

    if (dir_x == 0.0) {
        if (player != nullptr) {
            dir_x = (player->get_global_position().x >= bandido_x) ? -1.0 : 1.0;
        } else {
            dir_x = mirar_derecha ? -1.0 : 1.0;
        }
    }

    set_velocity(Vector2(dir_x * knockback_x, knockback_y));

    if (anim_sprite != nullptr) {
        anim_sprite->play("Hit");
    }
}