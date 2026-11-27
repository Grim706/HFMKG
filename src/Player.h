#pragma once

#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/character_body2d.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/classes/texture_progress_bar.hpp>
#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/collision_shape2d.hpp>
namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D);

private:
    double speed = 100.0;
    double jump_velocity = -300.0;
    int health = 100;
    bool atackar = false;
    Area2D *hurtbox = nullptr;
    AnimatedSprite2D *anim_sprite = nullptr;
    TextureProgressBar *health_bar = nullptr;
    NodePath health_bar_path = NodePath("../CanvasLayer2/Health/TextureProgressBar");
    CollisionShape2D *area_collision = nullptr;
    Area2D *area_2d = nullptr;
    bool hitPlayer = false;
     Vector2 velocity = Vector2(0, 0);
     int score = 0;
//AnimatedSprite2D* animated_sprite = nullptr;
protected:
    static void _bind_methods();

public:
    Player();
    ~Player();
    void _ready() override; // 3. Añade _ready para buscar el nodo
    void _physics_process(double delta) override;
    void damage_player(int damage, Node2D *source = nullptr);
    void _on_hurtbox_area_entered(Area2D *area);
    void set_health(int p_health);
    int get_health() const;
    void set_health_bar_path(const NodePath &p_path);
    NodePath get_health_bar_path() const;
    void actualizar_barra_vida();
    void _on_area_2d_body_entered(Node *body);
    bool _on_animation_finished();
    void evaluar_cuerpo_golpe(Node *body);
    void HitPlayer(Node2D *source = nullptr);
    void add_score(int amount) { score += amount; }
    int get_score() const { return score; }

    void Game_Over();
};

}