#include "Menu.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/scene_tree.hpp>

using namespace godot;

Menu::Menu() {}

Menu::~Menu() {}

void Menu::_bind_methods() {
    // Exponemos la función para que Godot la reconozca al conectar la señal
    ClassDB::bind_method(D_METHOD("on_play_pressed"), &Menu::on_play_pressed);
    ClassDB::bind_method(D_METHOD("on_quit_pressed"), &Menu::on_quit_pressed); // <--- Obligatorio para que Callable lo encuentre
}

void Menu::_ready() {
    // Buscamos el botón Play dentro del VBoxContainer usando la ruta relativa
    Node *node = find_child("Play", true, false);
    Button *play_btn = Object::cast_to<Button>(node);
    if (play_btn) {
        // Conectamos la señal "pressed" del botón directamente a nuestra función en C++
        play_btn->connect("pressed", Callable(this, "on_play_pressed"));
    }

    Node *quit_node = find_child("Quit", true, false);
    Button *quit_btn = Object::cast_to<Button>(quit_node);
    if (quit_btn) {
        quit_btn->connect("pressed", Callable(this, "on_quit_pressed"));
        UtilityFunctions::print("¡Botón Quit conectado exitosamente desde C++!");
    }
}

void Menu::on_play_pressed() {
    UtilityFunctions::print("¡Botón Play presionado desde C++!");

    // Como heredas de Control (y este de Node), get_tree() funciona de forma directa:
    SceneTree *tree = get_tree();
    if (tree) {
        // Cambia "res://escena_juego.tscn" por la ruta real de tu escena de juego
        tree->change_scene_to_file("res://Scenes/mapa.tscn");
    }
}
void Menu::on_quit_pressed() {
    UtilityFunctions::print("¡Cerrando el juego desde C++!");

    SceneTree *tree = get_tree();
    if (tree) {
        // Cierra la aplicación de manera limpia
        tree->quit();
    }
}