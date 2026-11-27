#ifndef PIG_H
#define PIG_H

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/ray_cast2d.hpp>
#include <godot_cpp/core/class_db.hpp>

namespace godot {

class Pig : public CharacterBody2D {
    GDCLASS(Pig, CharacterBody2D)

private:
    static constexpr double PIG_RUN_SPEED = 70.0;
    static constexpr double GRAVEDAD = 15.0; // Ajusta este valor al que uses en tu script
    AnimatedSprite2D *anim_sprite = nullptr;
    Area2D *atac_area = nullptr;
    static constexpr double DISTANCIA_PATRULLA = 120.0; // Prueba entre 80.0 y 200.0 según tu escala
    double patrulla_inicio_x = 0.0;
    int health = 100;
    bool is_hit = false;
    bool is_dead = false;
    static constexpr double VEL_PERSEGUIR = 110.0;
    bool perseguir = false;
    RayCast2D *ray_right = nullptr;
    RayCast2D *ray_left = nullptr;
    bool atacando = false;
    bool mirar_derecha = false;
protected:
    static void _bind_methods();

public:
    Pig() = default;
    ~Pig() override = default;

    void _ready() override;
    void _physics_process(double delta) override;
    void _on_atac_body_entered(Node *body);
    void detectar(Vector2 &vel);
    void damage_enemy(int damage, Node2D *source = nullptr);
    void hit_enemy(Node2D *source = nullptr);
    void _on_animation_finished();
    void _on_atac_body_exited(Node *body);
   
};

} // namespace godot

#endif // PIG_H