#include "GameOverScreen.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/callable.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include "GameState.h"

using namespace godot;

void GameOverScreen::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_confirm_pressed"), &GameOverScreen::_on_confirm_pressed);
    ClassDB::bind_method(D_METHOD("_on_name_submitted", "p_text"), &GameOverScreen::_on_name_submitted);
}

void GameOverScreen::_ready() {
    if (Engine::get_singleton()->is_editor_hint()) {
        return;
    }

    game_over_label = Object::cast_to<RichTextLabel>(get_node_or_null(NodePath("Center/GameOver")));
    name_input = Object::cast_to<LineEdit>(get_node_or_null(NodePath("LineEdit")));
    confirm_button = Object::cast_to<Button>(get_node_or_null(NodePath("Button")));
    player_name = "";

    int score = 0;
    Node *n = get_tree()->get_current_scene()->find_child("GameState", true, false);
    GameState *state = Object::cast_to<GameState>(n);
    if (state != nullptr) {
        score = state->get_score();
        state->reset_score();
    }

    if (game_over_label != nullptr) {
        String texto = String("[center][wave]GAME OVER\nSCORE: ") + String::num_int64(score) + String(" [/wave][/center]");
        game_over_label->set_text(texto);
    }
    if (name_input != nullptr) {
        name_input->clear();
        name_input->grab_focus();
        Callable submit_cb(this, "_on_name_submitted");
        if (!name_input->is_connected("text_submitted", submit_cb)) {
            name_input->connect("text_submitted", submit_cb);
        }
    }

    if (confirm_button != nullptr) {
        Callable press_cb(this, "_on_confirm_pressed");
        if (!confirm_button->is_connected("pressed", press_cb)) {
            confirm_button->connect("pressed", press_cb);
        }
    }
}
void GameOverScreen::set_score(int p_score) {
    score = p_score;
    if (game_over_label != nullptr) {
        String texto = String("[center][wave]GAME OVER\nSCORE: ") + String::num_int64(score) + String(" [/wave][/center]");
        game_over_label->set_text(texto);
    }
}

void GameOverScreen::_on_confirm_pressed() {
    if (name_input == nullptr) {
        return;
    }
    _on_name_submitted(name_input->get_text());
}

void GameOverScreen::_on_name_submitted(const String &p_text) {
    String limpio = p_text.strip_edges();
    if (limpio.is_empty()) {
        if (name_input != nullptr) {
            name_input->set_placeholder("Debes escribir un nombre...");
            name_input->grab_focus();
        }
        return;
    }

    player_name = limpio;
    UtilityFunctions::print("Jugador en Game Over: ", player_name);

    if (name_input != nullptr) {
        name_input->set_editable(false);
    }
    if (confirm_button != nullptr) {
        confirm_button->set_disabled(true);
    }
    /* Node *curtain = get_node_or_null(NodePath("/root/Curtain"));
    if (curtain != nullptr && curtain->has_method("change_scene_to_file")) {
        curtain->call("change_scene_to_file", "res://UI/MainScreens/menu_screen.tscn");
        return;
    }
*/
   
    SceneTree *tree = get_tree();
    if (tree != nullptr) {
        tree->change_scene_to_file("res://Scenes/menu.tscn");
    }
}