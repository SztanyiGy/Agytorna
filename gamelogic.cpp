#include "gamelogic.h"
#include <QRandomGenerator>

// ========================================================================
// Bányai Bence
// =======================================================================

GameLogic::GameLogic()
{
    // Inicializáláskor betöltjük a tesztadatokat
    initMockDatabase();
}

Question GameLogic::getRandomQuestion(Language lang, Category cat, Difficulty diff)
{
    QVector<Question> filteredQuestions;

    // Keresés a mock adatbázisban a megadott szűrők alapján
    for (const Question &q : mockDatabase) {
        if (q.language == lang && q.category == cat && q.difficulty == diff) {
            filteredQuestions.append(q);
        }
    }

    // Ha nincs találat, visszaad egy üres/hiba kérdést
    if (filteredQuestions.isEmpty()) {
        Question emptyQ;
        emptyQ.id = -1;
        emptyQ.questionText = "Nincs elérhető kérdés ebben a kategóriában.";
        return emptyQ;
    }

    // Véletlenszerű kiválasztás a szűrt listából
    int randomIndex = QRandomGenerator::global()->bounded(filteredQuestions.size());
    return filteredQuestions[randomIndex];
}

bool GameLogic::checkAnswer(const Question &question, int selectedAnswerIndex)
{
    // Index összehasonlítás
    return question.correctAnswer == selectedAnswerIndex;
}

int GameLogic::calculateScore(int currentScore, int pointsToAdd)
{
    // Pontszám növelése
    return currentScore + pointsToAdd;
}

void GameLogic::initMockDatabase()
{
    // Tesztadatok feltöltése
    mockDatabase.clear();

    int currentId = 1;

    // 1. Angol Szókincs Kezdő
    // ==========================================
    // ANGOL (English)
    // ==========================================

    // --- Szókincs (Vocabulary) - Kezdő ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent ez a szó magyarul?";
        q.word = "Apple";
        q.answers = {"Körte", "Alma", "Szilva", "Barack"};
        q.correctAnswer = 1; // Alma
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent ez a szó magyarul?";
        q.word = "Dog";
        q.answers = {"Macska", "Egér", "Kutya", "Ló"};
        q.correctAnswer = 2; // Kutya
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent ez a szó magyarul?";
        q.word = "House";
        q.answers = {"Ház", "Kert", "Utca", "Város"};
        q.correctAnswer = 0; // Ház
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Milyen szín a 'Blue'?";
        q.word = "Blue";
        q.answers = {"Piros", "Zöld", "Kék", "Sárga"};
        q.correctAnswer = 2; // Kék
        mockDatabase.append(q);
    }

    // --- Nyelvtan (Grammar) - Kezdő ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Grammar;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Egészítsd ki a mondatot!";
        q.word = "I ___ a student.";
        q.answers = {"is", "are", "am", "be"};
        q.correctAnswer = 2; // am
        mockDatabase.append(q);
    }

    // --- Mondatok (Sentences) - Haladó ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::English;
        q.category = Category::Sentences;
        q.difficulty = Difficulty::Intermediate;
        q.points = 1;
        q.questionText = "Hogyan kérdezed meg angolul: 'Hogy vagy?'";
        q.word = "Hogy vagy?";
        q.answers = {"Who are you?", "How represent you?", "How are you?", "Where are you?"};
        q.correctAnswer = 2; // How are you?
        mockDatabase.append(q);
    }

    // ==========================================
    // NÉMET (German)
    // ==========================================

    // --- Szókincs (Vocabulary) - Kezdő ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::German;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent ez a szó magyarul?";
        q.word = "Der Baum";
        q.answers = {"A fa", "A ház", "Az autó", "A virág"};
        q.correctAnswer = 0; // A fa
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::German;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent: 'Danke'?";
        q.word = "Danke";
        q.answers = {"Szia", "Kérem", "Köszönöm", "Viszlát"};
        q.correctAnswer = 2; // Köszönöm
        mockDatabase.append(q);
    }

    // --- Nyelvtan (Grammar) - Haladó ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::German;
        q.category = Category::Grammar;
        q.difficulty = Difficulty::Intermediate;
        q.points = 1;
        q.questionText = "Válaszd ki a helyes névelőt!";
        q.word = "___ Tisch (asztal)";
        q.answers = {"Der", "Die", "Das", "Den"};
        q.correctAnswer = 0; // Der
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::German;
        q.category = Category::Grammar;
        q.difficulty = Difficulty::Intermediate;
        q.points = 1;
        q.questionText = "Válaszd ki a helyes névelőt!";
        q.word = "___ Sonne (nap)";
        q.answers = {"Der", "Die", "Das", "Ein"};
        q.correctAnswer = 1; // Die
        mockDatabase.append(q);
    }

    // ==========================================
    // OROSZ (Russian)
    // ==========================================

    // --- Szókincs (Vocabulary) - Kezdő ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::Russian;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent: 'Да'?";
        q.word = "Да";
        q.answers = {"Nem", "Talán", "Igen", "Kérem"};
        q.correctAnswer = 2; // Igen
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::Russian;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent: 'Нет'?";
        q.word = "Нет";
        q.answers = {"Igen", "Nem", "Jó", "Rossz"};
        q.correctAnswer = 1; // Nem
        mockDatabase.append(q);
    }
    {
        Question q;
        q.id = currentId++;
        q.language = Language::Russian;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Beginner;
        q.points = 1;
        q.questionText = "Mit jelent: 'Привет'?";
        q.word = "Привет";
        q.answers = {"Viszlát", "Köszönöm", "Bocsánat", "Szia"};
        q.correctAnswer = 3; // Szia
        mockDatabase.append(q);
    }

    // --- Szókincs - Haladó ---
    {
        Question q;
        q.id = currentId++;
        q.language = Language::Russian;
        q.category = Category::Vocabulary;
        q.difficulty = Difficulty::Intermediate;
        q.points = 1;
        q.questionText = "Mit jelent: 'Друг'?";
        q.word = "Друг";
        q.answers = {"Ellenség", "Barát", "Testvér", "Szomszéd"};
        q.correctAnswer = 1; // Barát
        mockDatabase.append(q);
    }
}
