#ifndef BANDIDO_H
#define BANDIDO_H

#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/marker2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/timer.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/node_path.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/classes/sprite2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>

using namespace godot;

class Bandido : public CharacterBody2D {
    GDCLASS(Bandido, CharacterBody2D)

private:
    double speed = 50.0;
    bool puede_disparar = false;
    Sprite2D *sprite = nullptr;
    Node2D *player = nullptr;
    Marker2D *marker = nullptr;
    AnimatedSprite2D *anim_sprite = nullptr;
    Timer *timer = nullptr;
    Area2D *vision = nullptr;
    static constexpr double GRAVEDAD = 15.0;
    Ref<PackedScene> proyectil_scene;
    NodePath proyectil_scene_path = NodePath("res://Scenes/proyectil.tscn");

    void seguir_jugador();
    void disparar();

    int health = 100;
    bool is_hit = false;
    bool is_dead = false;

    bool mirar_derecha = false;
    bool atacando = false;

protected:
    static void _bind_methods();

public:
    Bandido() = default;
    ~Bandido() override = default;

    void _ready() override;
    void _physics_process(double delta) override;

    void set_speed(double p_speed);
    double get_speed() const;

    void set_proyectil_scene_path(const NodePath &p_path);
    NodePath get_proyectil_scene_path() const;

    void _on_vision_body_entered(Node *body);
    void _on_vision_body_exited(Node *body);
    void _on_timer_timeout();

    void _on_atac_body_entered(Node *body);
    void _on_atac_body_exited(Node *body);
    void _on_animation_finished();

    void damage_enemy(int damage, Node2D *source = nullptr);
    void hit_enemy(Node2D *source = nullptr);

   
};

#endif