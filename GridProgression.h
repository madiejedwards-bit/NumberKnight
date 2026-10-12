#pragma once
#include <string>
#include "Questions.h"

static const int BOARD_ROWS = 5;
static const int BOARD_COLS = 5;

struct State {
    int answerInput = 0;
    int targetRow = -1;
    int targetCol = -1;
    bool awaitingAnswer = false;
    bool focusAnswerBox = false;
    bool showVictory = false;
    double questionStartTime = 0.0;
    std::string statusMessage = "Move one step at a time toward the \"End\" tile.";
    bool visited[BOARD_ROWS][BOARD_COLS] = {};
};
class GridProgression
{
public:
    static const int BOARD_SIZE = 5;
    static const int GOAL_ROW = 4;
    static const int GOAL_COL = 4;
    static const int STARTING_HEALTH = 3;

    int PlayerRow;
    int PlayerCol;
    int Health;
    int Score;

    GridProgression();
    void reset();
    bool isAdjacent(int targetRow, int targetCol) const;
    bool isGoal(int row, int col) const;
    bool isAtGoal() const;
    void moveTo(int row, int col);
    void loseHealth();
    bool isDefeated() const;

    static Difficulty DifficultyForSquare(int row, int col);
};
