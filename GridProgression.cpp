#include "GridProgression.h"
#include "Questions.h"
#include <cstdlib>

GridProgression::GridProgression()
{
    reset();
}

void GridProgression::reset()
{
    PlayerRow = 0;
    PlayerCol = 0;
    Health = STARTING_HEALTH;
    Score = 0;
}

bool GridProgression::isAdjacent(int targetRow, int targetCol) const
{
    return abs(PlayerRow - targetRow) + abs(PlayerCol - targetCol) == 1;
}

bool GridProgression::isGoal(int row, int col) const
{
    return row == GOAL_ROW && col == GOAL_COL;
}

bool GridProgression::isAtGoal() const
{
    return isGoal(PlayerRow, PlayerCol);
}

void GridProgression::moveTo(int row, int col)
{
    PlayerRow = row;
    PlayerCol = col;
}

void GridProgression::loseHealth()
{
    if (Health > 0)
        Health -= 1;
}

bool GridProgression::isDefeated() const
{
    return Health <= 0;
}
Difficulty GridProgression::DifficultyForSquare(int row, int col)
{
    const int dist = row + col;
    if (dist <= 2)
        return Difficulty::EASY;
    if (dist <= 5)
        return Difficulty::MEDIUM;
    return Difficulty::HARD;
}
