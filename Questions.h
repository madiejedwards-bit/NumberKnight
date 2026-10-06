#pragma once
#include <string>

using namespace std;

enum class Difficulty
{
    EASY,
    MEDIUM,
    HARD,
    DIFFICULTYCOUNT
};
enum Operations {
    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    OPERATIONCOUNT
};
class Questions
{
    private:
        string questionPrompt;
        int correctAnswer;
        int points;
        int timeLimit;
        Difficulty difficulty;
        bool timedOut;

    public:
        Difficulty getDifficulty();
        void generateEasyQuestion();
        void generateMediumQuestion();
        void generateHardQuestion();
        string getQuestionPrompt();
        void setTimeLimit(int endurance);
        int getTimeLimit();
        bool checkAnswer(int playerAnswer);
        bool isTimedOut(int elapsedTime);
        int getPoints();
};

