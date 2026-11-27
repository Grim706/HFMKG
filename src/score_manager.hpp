#pragma once

#include <heap.hpp> // Tu cabecera de Designar
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>
#include <vector>

using namespace godot;

struct PlayerScore {
    String name;
    int score = 0;

    // Necesario para std::less<PlayerScore> (Min-Heap por puntaje)
    bool operator<(const PlayerScore &other) const {
        return score < other.score;
    }
};

class ScoreManager : public Node {
    GDCLASS(ScoreManager, Node);

public:
    static constexpr Designar::nat_t MAX_PLAYERS = 10;

private:
    int last_score = 0;
    Designar::DynHeap<PlayerScore, std::less<PlayerScore>> min_heap;

protected:
    static void _bind_methods();

public:
    ScoreManager() = default;
    ~ScoreManager() override = default;

    void set_last_score(int p_score);
    int get_last_score() const;

    void add_score(const String &p_name, int p_score);
    std::vector<PlayerScore> get_top_scores_descending() const;
    void clear_scores();
};