#ifndef MENU_H
#define MENU_H

#include <godot_cpp/classes/control.hpp>

namespace godot {

class Menu : public Control {
    GDCLASS(Menu, Control);

protected:
    static void _bind_methods();

public:
    Menu();
    ~Menu();

    void _ready() override;
    
    // Función que se ejecutará cuando presionen el botón Play
    void on_play_pressed();
     void on_quit_pressed();
};

}

#endif