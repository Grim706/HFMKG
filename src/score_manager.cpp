#include "score_manager.hpp"

#include <godot_cpp/core/class_db.hpp>
#include "score_manager.hpp"
#include <godot_cpp/core/class_db.hpp>

void ScoreManager::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_last_score", "p_score"), &ScoreManager::set_last_score);
    ClassDB::bind_method(D_METHOD("get_last_score"), &ScoreManager::get_last_score);
    ClassDB::bind_method(D_METHOD("add_score", "p_name", "p_score"), &ScoreManager::add_score);
    ClassDB::bind_method(D_METHOD("clear_scores"), &ScoreManager::clear_scores);
}

void ScoreManager::set_last_score(int p_score) {
    last_score = p_score;
}

int ScoreManager::get_last_score() const {
    return last_score;
}

void ScoreManager::add_score(const String &p_name, int p_score) {
    String clean_name = p_name.strip_edges();
    if (clean_name.is_empty()) {
        clean_name = "ANONYMOUS";
    }

    PlayerScore entry{clean_name, p_score};

    // Si hay menos de 10 jugadores, entra directo al Min-Heap
    if (min_heap.size() < MAX_PLAYERS) {
        min_heap.insert(std::move(entry));
        return;
    }

    // Si ya hay 10 jugadores, comparamos contra el mínimo (top())
    if (entry.score > min_heap.top().score) {
        min_heap.get(); // Extrae el menor de los 10
        min_heap.insert(std::move(entry)); // Inserta el nuevo puntaje
    }
}

std::vector<PlayerScore> ScoreManager::get_top_scores_descending() const {
    // Copiamos el DynHeap para extraer con get() sin vaciar el original
    Designar::DynHeap<PlayerScore, std::less<PlayerScore>> temp_heap = min_heap;

    std::vector<PlayerScore> ascending;
    ascending.reserve(temp_heap.size());

    // get() extrae de MENOR a MAYOR
    while (!temp_heap.is_empty()) {
        ascending.push_back(temp_heap.get());
    }

    // Invertimos para devolver de MAYOR a MENOR (1.º lugar arriba)
    return std::vector<PlayerScore>(ascending.rbegin(), ascending.rend());
}

void ScoreManager::clear_scores() {
    min_heap.clear();
    last_score = 0;
}