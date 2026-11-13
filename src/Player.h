#pragma once

#include <godot_cpp/classes/character_body2d.hpp>
namespace godot {

class Player : public CharacterBody2D {
    GDCLASS(Player, CharacterBody2D);

private:
    double speed = 100.0;
    double jump_velocity = -300.0;
//AnimatedSprite2D* animated_sprite = nullptr;
protected:
    static void _bind_methods();

public:
    Player();
    ~Player();
    void _ready() override; // 3. Añade _ready para buscar el nodo
    void _physics_process(double delta) override;
};

}