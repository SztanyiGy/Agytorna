#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QVector>
#include "gametypes.h"

class GameLogic
{
public:
    // ========================================================================
    // Bányai Bence
    // ========================================================================
    GameLogic();

    // 1. Véletlenszerű kérdés lekérése az adatbázisból (jelenleg memória)
    Question getRandomQuestion(Language lang, Category cat, Difficulty diff);

    // 2. Válasz ellenőrzése
    bool checkAnswer(const Question &question, int selectedAnswerIndex);

    // 3. Pontszám frissítése
    int calculateScore(int currentScore, int pointsToAdd);

private:
    // ========================================================================
    // Bányai Bence
    // ========================================================================
    // Ideiglenes adatbázis a teszteléshez (amíg Viktória el nem készül az SQL-lel)
    QVector<Question> mockDatabase;
    void initMockDatabase();
};

#endif // GAMELOGIC_H
