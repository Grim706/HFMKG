#pragma once

#include <godot_cpp/classes/node.hpp>

using namespace godot;

class GameState : public Node {
    GDCLASS(GameState, Node)

private:
    int score = 0;

protected:
    static void _bind_methods();

public:
    GameState() = default;

    void add_score(int amount);
    int get_score() const;
    void reset_score();
};