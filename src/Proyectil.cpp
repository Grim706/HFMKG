#include "Proyectil.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/sprite_frames.hpp>
#include <godot_cpp/variant/callable.hpp>

using namespace godot;

void Proyectil::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_direccion", "p_dir"), &Proyectil::set_direccion);
    ClassDB::bind_method(D_METHOD("get_direccion"), &Proyectil::get_direccion);
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "direccion"), "set_direccion", "get_direccion");

    // En Proyectil::_bind_methods():
    ClassDB::bind_method(D_METHOD("_on_area_entered", "area"), &Proyectil::_on_area_entered);
    ClassDB::bind_method(D_METHOD("set_velocidad", "p_vel"), &Proyectil::set_velocidad);
    ClassDB::bind_method(D_METHOD("get_velocidad"), &Proyectil::get_velocidad);
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "velocidad"), "set_velocidad", "get_velocidad");

    ClassDB::bind_method(D_METHOD("set_dano", "p_dano"), &Proyectil::set_dano);
    ClassDB::bind_method(D_METHOD("get_dano"), &Proyectil::get_dano);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "dano"), "set_dano", "get_dano");

    ClassDB::bind_method(D_METHOD("_on_body_entered", "body"), &Proyectil::_on_body_entered);
}

void Proyectil::aplicar_orientacion() {
    if (anim == nullptr) {
        return;
    }

    if (direccion.x < 0.0) {
        anim->set_flip_h(true);
    } else if (direccion.x > 0.0) {
        anim->set_flip_h(false);
    }
    orientacion_aplicada = true;
}

void Proyectil::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }
    // En Proyectil::_ready():
    Callable area_cb(this, "_on_area_entered");
    if (!is_connected("area_entered", area_cb)) {
        connect("area_entered", area_cb);
    }
    if (has_node(NodePath("AnimatedSprite2D"))) {
        anim = get_node<AnimatedSprite2D>(NodePath("AnimatedSprite2D"));
        if (anim != nullptr && anim->get_sprite_frames().is_valid()) {
            if (anim->get_sprite_frames()->has_animation("default")) {
                anim->play("default");
            } else {
                anim->play();
            }
        }
    }
    
    aplicar_orientacion();

    Callable body_cb(this, "_on_body_entered");
    if (!is_connected("body_entered", body_cb)) {
        connect("body_entered", body_cb);
    }
}

void Proyectil::_physics_process(double delta) {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    if (!orientacion_aplicada && anim != nullptr) {
        aplicar_orientacion();
    }

    Vector2 dir_segura = direccion;
    if (dir_segura.length_squared() > 0.0) {
        dir_segura = dir_segura.normalized();
    } else {
        dir_segura = Vector2(1.0, 0.0);
    }

    Vector2 pos = get_position();
    pos += dir_segura * velocidad * delta;
    set_position(pos);

    tiempo_transcurrido += delta;
    if (tiempo_transcurrido >= tiempo_vida) {
       queue_free();
    }
}

void Proyectil::set_direccion(const Vector2 &p_dir) {
    direccion = p_dir;
    orientacion_aplicada = false;
    aplicar_orientacion();
}

Vector2 Proyectil::get_direccion() const {
    return direccion;
}

void Proyectil::set_velocidad(double p_vel) {
    velocidad = p_vel;
}

double Proyectil::get_velocidad() const {
    return velocidad;
}

void Proyectil::set_dano(int p_dano) {
    dano = p_dano;
}

int Proyectil::get_dano() const {
    return dano;
}

void Proyectil::_on_body_entered(Node *body) {
    if (body == nullptr) {
        return;
    }

    if (body->get_name() == StringName("Bandido") || body->is_in_group("Bandido")) {
        return;
    }

 if (body->is_in_group("Player")) {
        if (body->has_method("damage_player")) {
            body->call("damage_player", dano, this);
        }
        queue_free();
        return;
    }

     queue_free();
}

// En Proyectil::_on_area_entered(Area2D *area):
void Proyectil::_on_area_entered(Area2D *area) {
    if (area == nullptr) {
        return;
    }

    Node *parent = area->get_parent();
    if (parent != nullptr && parent->is_in_group("Player")) {
        if (parent->has_method("damage_player")) {
            parent->call("damage_player", dano, this);
        }
        queue_free();
    }
}