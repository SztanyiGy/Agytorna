#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QVector>
#include "gamelogic.h"
#include "gametypes.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // ========================================================================
    // BARTA CSONGOR - Qt GUI felület kezelés
    // ========================================================================
    void showMainMenu();    // Főmenü és nyelvválasztás megjelenítése
    void displayQuestion(); // Kérdés és válaszok kirajzolása
    void updateUIState();   // Válasz után vizuális visszajelzés

    // ========================================================================
    // SZTÁNYI GYÖRGY - Interaktív elemek (Qt-ben)
    // ========================================================================
    void showResults();                                    // Eredménylap és pontszám kijelzése
    void restartGame();                                    // Új játékmenet indítása
    void highlightAnswer(int answerIndex, bool isCorrect); // Helyes/hibás válasz kiemelése

    // ========================================================================
    // HRABINA GERGŐ - Kérdések és válaszok interaktív kezelése
    // ========================================================================
    void handleAnswer(int answerIndex); // Válasz kijelölése
    void checkSelectedAnswer();         // Kijelölt válasz ellenőrzése
    void showFeedback(bool isCorrect);  // Helyes/hibás válasz vizuális visszajelzése
    void enableNextQuestion();          // Következő kérdés aktiválása

private slots:
    // Nyelvválasztó gombok
    void onHungarianSelected();
    void onEnglishSelected();
    void onGermanSelected();
    void onRussianSelected();

    // Kategória gombok
    void onVocabularySelected();
    void onGrammarSelected();
    void onSentencesSelected();
    void onListeningSelected();

    // Nehézségi szint gombok
    void onBeginnerSelected();
    void onIntermediateSelected();
    void onAdvancedSelected();

    // Navigációs gombok
    void onStartQuiz();
    void onBackFromCategory();
    void onBackToMenu();
    void onNextQuestion();
    void onQuitQuiz();
    void onRestartSame();
    void onNextLevel();

    // Sidebar gombok
    void onHomeClicked();
    void onLanguagesClicked();
    void onStatsClicked();
    void onProfileClicked();
    void onLogoutClicked();

    // Statisztika
    void onBackFromStatsClicked();

private:
    Ui::MainWindow *ui;
    GameLogic gameLogic;

    // Játék állapot változók
    QVector<Question> currentQuestions; // Aktuális kvíz kérdései
    QVector<Question> questions;        // Összes kérdés (később adatbázisból)
    int currentQuestionIndex;
    int correctAnswers;
    int totalPoints;
    bool answerSelected;
    int selectedAnswerIndex; // Kiválasztott válasz indexe (ÚJ)

    // Kiválasztott beállítások
    Language selectedLanguage;
    Category selectedCategory;
    Difficulty selectedDifficulty;

    // Statisztika változók
    int currentStreak;
    int totalWordsLearned;

    // UI elemek
    QVector<QPushButton *> answerButtons;

    // Kijelölt gombok tárolása
    QPushButton *selectedCategoryButton;
    QPushButton *selectedDifficultyButton;

    // ========================================================================
    // Frontend belső segédfüggvények
    // ========================================================================
    void setupConnections();
    void createAnswerButtons();
    void clearAnswerButtons();
    void updateQuestionCounter();
    void updateCurrentScore();
    void showCategoryPage();
    void showStatisticsPage();

    // Kijelölés kezelő függvények
    void handleCategorySelection(QPushButton *button, Category category);
    void handleDifficultySelection(QPushButton *button, Difficulty difficulty);

    // UI szövegek
    QString getLanguageName(Language lang);
    QString getLanguageFlag(Language lang);
    QString getCategoryName(Category cat);
    QString getCategoryIcon(Category cat);
    QString getDifficultyName(Difficulty diff);
    QString getDifficultyIcon(Difficulty diff);
    QString getMotivationalMessage(double accuracy);

    // ========================================================================
    // BACKEND INTERFACE - Ezeket később a backend implementálja
    // ========================================================================
    void loadQuestionsFromBackend();           // Backend: getRandomQuestion()
    void sendAnswerToBackend(int answerIndex); // Backend: checkAnswer()
    void updateStatisticsInBackend();          // Backend: updateStatistics()
    void loadStatisticsFromBackend();          // Backend: getPlayerStats()

    // Ideiglenes függvények demo kérdésekhez (később törlendő)
    void loadDemoQuestions();

    // Statisztika kezelés (placeholder)
    void updateStatistics();
    void filterQuestions();
};

#endif // MAINWINDOW_H
