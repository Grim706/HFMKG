#ifndef PROYECTIL_H
#define PROYECTIL_H

#include <godot_cpp/classes/animated_sprite2d.hpp>
#include <godot_cpp/classes/area2d.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/vector2.hpp>


using namespace godot;

class Proyectil : public Area2D {
    GDCLASS(Proyectil, Area2D);

private:
    Vector2 direccion = Vector2(1.0, 0.0);
    double velocidad = 320.0;
    int dano = 10;
    double tiempo_vida = 3.0;
    double tiempo_transcurrido = 0.0;
    bool orientacion_aplicada = false;

    AnimatedSprite2D *anim = nullptr;

    void aplicar_orientacion();

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _physics_process(double delta) override;

    void set_direccion(const Vector2 &p_dir);
    Vector2 get_direccion() const;

    void set_velocidad(double p_vel);
    double get_velocidad() const;

    void _on_area_entered(Area2D *area);

    void set_dano(int p_dano);
    int get_dano() const;

    void _on_body_entered(Node *body);
};

#endif