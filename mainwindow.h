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

// Kérdés struktúra
struct Question {
    QString text;
    QVector<QString> answers;
    int correctAnswer;
    int points;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Barta Csongor - Qt GUI felület kezelés
    void showMainMenu();
    void displayQuestion();
    void updateUIState();

    // Sztányi György - Interaktív elemek
    void showResults();
    void restartGame();
    void highlightAnswer(int answerIndex, bool isCorrect);

    // Hrabina Gergő - Kérdések és válaszok kezelése
    void handleAnswer(int answerIndex);
    void showFeedback(bool isCorrect);
    void enableNextQuestion();

private slots:
    // Navigációs slotok
    void onContinueClicked();
    void onBackToMenuClicked();
    void onNextQuestionClicked();
    void onRestartGameClicked();
    void onViewCourseClicked();

    // Sidebar slotok
    void onHomeClicked();
    void onCoursesClicked();
    void onProfileClicked();
    void onLogoutClicked();

private:
    Ui::MainWindow *ui;

    // Játék állapot változók
    QVector<Question> questions;
    int currentQuestionIndex;
    int correctAnswers;
    int totalPoints;
    bool answerSelected;

    // Válaszgombok tárolása
    QVector<QPushButton*> answerButtons;

    // Segéd függvények
    void setupConnections();
    void loadQuestions();
    void createAnswerButtons();
    void clearAnswerButtons();
    void addCourseCard(const QString& courseName, int courseIndex);
    void updateStatistics();
};

#endif // MAINWINDOW_H
