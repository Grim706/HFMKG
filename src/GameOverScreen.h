#pragma once

#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/line_edit.hpp>
#include <godot_cpp/classes/rich_text_label.hpp>
#include <godot_cpp/variant/string.hpp>

using namespace godot;

class GameOverScreen : public Control {
    GDCLASS(GameOverScreen, Control)

private:
    RichTextLabel *game_over_label = nullptr;
    LineEdit *name_input = nullptr;
    Button *confirm_button = nullptr;
    String player_name;
    int score = 0;

protected:
    static void _bind_methods();

public:
    GameOverScreen() = default;
    ~GameOverScreen() override = default;
   void set_score(int p_score);
    void _ready() override;
    void _on_confirm_pressed();
    void _on_name_submitted(const String &p_text);
};