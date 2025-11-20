#ifndef GAMETYPES_H
#define GAMETYPES_H

#include <QString>
#include <QVector>

// Nyelv enum
enum class Language { Hungarian, English, German, Russian };

// Kategória enum
enum class Category { Vocabulary, Grammar, Sentences, Listening };

// Nehézségi szint enum
enum class Difficulty { Beginner, Intermediate, Advanced };

// Kérdés struktúra
struct Question
{
    int id;
    QString questionText;
    QString word;
    QVector<QString> answers;
    int correctAnswer; // 0-3 index
    int points;
    Language language;
    Category category;
    Difficulty difficulty;
};

#endif // GAMETYPES_H
