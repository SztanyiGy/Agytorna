#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVector>
#include <QString>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// Nyelv enum
enum class Language {
    Hungarian,
    English,
    German,
    Russian
};

// Kategória enum
enum class Category {
    Vocabulary,
    Grammar,
    Sentences,
    Listening
};

// Nehézségi szint enum
enum class Difficulty {
    Beginner,
    Intermediate,
    Advanced
};

// Kérdés struktúra - most nyelvtanuláshoz optimalizálva
struct Question {
    QString questionText;        // "Mit jelent ez a szó?"
    QString word;                // A tanítandó szó
    QVector<QString> answers;    // Válaszlehetőségek
    int correctAnswer;           // Helyes válasz indexe
    int points;                  // Pontérték
    Language language;           // Nyelv
    Category category;           // Kategória
    Difficulty difficulty;       // Nehézségi szint
    QString explanation;         // Magyarázat (opcionális)
};

// Felhasználói statisztika struktúra
struct LanguageStats {
    int wordsLearned;
    int quizzesCompleted;
    int totalCorrect;
    int totalQuestions;
    double accuracy;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // ========================================================================
    // BARTA CSONGOR - Qt GUI felület kezelés
    // ========================================================================
    void showMainMenu();           // Főmenü megjelenítése
    void displayQuestion();        // Kérdés megjelenítése
    void updateUIState();          // UI állapot frissítése

    // ========================================================================
    // SZTÁNYI GYÖRGY - Interaktív elemek
    // ========================================================================
    void showResults();            // Eredménylap megjelenítése
    void restartGame();            // Játék újraindítása
    void highlightAnswer(int answerIndex, bool isCorrect);  // Válasz kiemelése

    // ========================================================================
    // HRABINA GERGŐ - Kérdések és válaszok kezelése
    // ========================================================================
    void handleAnswer(int answerIndex);     // Válasz feldolgozása
    void showFeedback(bool isCorrect);      // Visszajelzés megjelenítése
    void enableNextQuestion();              // Következő kérdés engedélyezése

private slots:
    // ========================================================================
    // Nyelvválasztó slotok
    // ========================================================================
    void onHungarianSelected();
    void onEnglishSelected();
    void onGermanSelected();
    void onRussianSelected();

    // ========================================================================
    // Kategória és nehézségi szint slotok
    // ========================================================================
    void onVocabularySelected();
    void onGrammarSelected();
    void onSentencesSelected();
    void onListeningSelected();

    void onBeginnerSelected();
    void onIntermediateSelected();
    void onAdvancedSelected();

    // ========================================================================
    // Navigációs slotok
    // ========================================================================
    void onStartQuiz();
    void onBackFromCategory();
    void onBackToMenu();
    void onNextQuestion();
    void onQuitQuiz();
    void onRestartSame();
    void onNextLevel();

    // ========================================================================
    // Sidebar slotok
    // ========================================================================
    void onHomeClicked();
    void onLanguagesClicked();
    void onStatsClicked();
    void onProfileClicked();
    void onLogoutClicked();

private:
    Ui::MainWindow *ui;

    // ========================================================================
    // Játék állapot változók
    // ========================================================================
    QVector<Question> questions;
    int currentQuestionIndex;
    int correctAnswers;
    int totalPoints;
    bool answerSelected;

    // Kiválasztott beállítások
    Language selectedLanguage;
    Category selectedCategory;
    Difficulty selectedDifficulty;

    // Statisztikák
    QMap<Language, LanguageStats> statistics;
    int currentStreak;              // Napi sorozat
    int totalWordsLearned;          // Összes megtanult szó

    // UI elemek
    QVector<QPushButton*> answerButtons;

    // ========================================================================
    // Segéd függvények
    // ========================================================================
    void setupConnections();
    void loadQuestions();
    void createAnswerButtons();
    void clearAnswerButtons();
    void updateStatistics();
    void updateQuestionCounter();
    void updateCurrentScore();
    void showCategoryPage();
    void showStatisticsPage();

    // Nyelv és kategória kezelés
    QString getLanguageName(Language lang);
    QString getLanguageFlag(Language lang);
    QString getCategoryName(Category cat);
    QString getCategoryIcon(Category cat);
    QString getDifficultyName(Difficulty diff);
    QString getDifficultyIcon(Difficulty diff);

    // Motivációs üzenet
    QString getMotivationalMessage(double accuracy);

    // Kérdések szűrése
    void filterQuestions();
};

#endif // MAINWINDOW_H
