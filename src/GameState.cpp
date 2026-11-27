#include "GameState.h"

#include <godot_cpp/core/class_db.hpp>

using namespace godot;

void GameState::add_score(int amount) {
    score += amount;
}

int GameState::get_score() const {
    return score;
}

void GameState::reset_score() {
    score = 0;
}

void GameState::_bind_methods() {
    ClassDB::bind_method(D_METHOD("add_score", "amount"), &GameState::add_score);
    ClassDB::bind_method(D_METHOD("get_score"), &GameState::get_score);
    ClassDB::bind_method(D_METHOD("reset_score"), &GameState::reset_score);
}