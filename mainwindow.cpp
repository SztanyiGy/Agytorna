#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentQuestionIndex(0)
    , correctAnswers(0)
    , totalPoints(0)
    , answerSelected(false)
{
    ui->setupUi(this);

    setupConnections();
    loadQuestions();

    // Kezdő állapot: főmenü
    showMainMenu();

    // Kurzuskártyák létrehozása
    addCourseCard("Matematika alapok", 0);
    addCourseCard("Logikai feladatok", 1);
    addCourseCard("Memória gyakorlatok", 2);
    addCourseCard("Szövegértés", 3);
    addCourseCard("Térbeli gondolkodás", 4);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ============================================================================
// BARTA CSONGOR - Qt GUI felület kezelés
// ============================================================================

void MainWindow::showMainMenu()
{
    ui->stackedWidget->setCurrentWidget(ui->mainMenuPage);
    updateStatistics();
}

void MainWindow::displayQuestion()
{
    if (currentQuestionIndex >= questions.size()) {
        showResults();
        return;
    }

    // Kérdés megjelenítése
    Question& q = questions[currentQuestionIndex];
    ui->questionLabel->setText(q.text);

    // Válaszgombok létrehozása
    clearAnswerButtons();
    createAnswerButtons();

    // Visszajelzés törlése
    ui->feedbackLabel->clear();
    ui->feedbackLabel->setStyleSheet("");

    answerSelected = false;
    ui->nextQuestionButton->setEnabled(false);
}

void MainWindow::updateUIState()
{
    // Válasz gombok állapotának frissítése
    for (QPushButton* btn : answerButtons) {
        btn->setEnabled(!answerSelected);
    }

    // Következő gomb frissítése
    ui->nextQuestionButton->setEnabled(answerSelected);
}

// ============================================================================
// SZTÁNYI GYÖRGY - Interaktív elemek
// ============================================================================

void MainWindow::showResults()
{
    ui->stackedWidget->setCurrentWidget(ui->resultsPage);

    // Eredmények megjelenítése
    QString scoreText = QString("%1 / %2")
                            .arg(correctAnswers)
                            .arg(questions.size());
    ui->resultLabel->setText(scoreText);

    QString pointsText = QString("Points: %1").arg(totalPoints);
    ui->pointsLabel->setText(pointsText);

    // Statisztika frissítése
    updateStatistics();
}

void MainWindow::restartGame()
{
    currentQuestionIndex = 0;
    correctAnswers = 0;
    totalPoints = 0;
    answerSelected = false;

    ui->stackedWidget->setCurrentWidget(ui->interactivePage);
    displayQuestion();
}

void MainWindow::highlightAnswer(int answerIndex, bool isCorrect)
{
    if (answerIndex >= 0 && answerIndex < answerButtons.size()) {
        QPushButton* btn = answerButtons[answerIndex];

        if (isCorrect) {
            btn->setStyleSheet(
                "QPushButton { "
                "background-color: #4CAF50; "
                "color: white; "
                "border: 2px solid #45a049; "
                "}"
                );
        } else {
            btn->setStyleSheet(
                "QPushButton { "
                "background-color: #f44336; "
                "color: white; "
                "border: 2px solid #da190b; "
                "}"
                );
        }

        // Helyes válasz kiemelése mindig
        if (!isCorrect && currentQuestionIndex < questions.size()) {
            int correctIdx = questions[currentQuestionIndex].correctAnswer;
            if (correctIdx >= 0 && correctIdx < answerButtons.size()) {
                answerButtons[correctIdx]->setStyleSheet(
                    "QPushButton { "
                    "background-color: #4CAF50; "
                    "color: white; "
                    "border: 2px solid #45a049; "
                    "}"
                    );
            }
        }
    }
}

// ============================================================================
// HRABINA GERGŐ - Kérdések és válaszok kezelése
// ============================================================================

void MainWindow::handleAnswer(int answerIndex)
{
    if (answerSelected || currentQuestionIndex >= questions.size()) {
        return;
    }

    Question& q = questions[currentQuestionIndex];
    bool isCorrect = (answerIndex == q.correctAnswer);

    if (isCorrect) {
        correctAnswers++;
        totalPoints += q.points;
    }

    answerSelected = true;

    // Válasz kiemelése
    highlightAnswer(answerIndex, isCorrect);

    // Visszajelzés megjelenítése
    showFeedback(isCorrect);

    // UI frissítése
    updateUIState();

    // Következő kérdés engedélyezése
    enableNextQuestion();
}

void MainWindow::showFeedback(bool isCorrect)
{
    if (isCorrect) {
        ui->feedbackLabel->setText("✓ Helyes válasz!");
        ui->feedbackLabel->setStyleSheet(
            "QLabel { "
            "background-color: #d4edda; "
            "color: #155724; "
            "border: 1px solid #c3e6cb; "
            "border-radius: 8px; "
            "padding: 10px; "
            "font-size: 16px; "
            "font-weight: bold; "
            "}"
            );
    } else {
        ui->feedbackLabel->setText("✗ Helytelen válasz!");
        ui->feedbackLabel->setStyleSheet(
            "QLabel { "
            "background-color: #f8d7da; "
            "color: #721c24; "
            "border: 1px solid #f5c6cb; "
            "border-radius: 8px; "
            "padding: 10px; "
            "font-size: 16px; "
            "font-weight: bold; "
            "}"
            );
    }
}

void MainWindow::enableNextQuestion()
{
    ui->nextQuestionButton->setEnabled(true);
}

// ============================================================================
// SLOTS - Navigációs és eseménykezelők
// ============================================================================

void MainWindow::onContinueClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->interactivePage);
    restartGame();
}

void MainWindow::onBackToMenuClicked()
{
    showMainMenu();
}

void MainWindow::onNextQuestionClicked()
{
    currentQuestionIndex++;
    displayQuestion();
}

void MainWindow::onRestartGameClicked()
{
    restartGame();
}

void MainWindow::onViewCourseClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->interactivePage);
    restartGame();
}

void MainWindow::onHomeClicked()
{
    showMainMenu();
}

void MainWindow::onCoursesClicked()
{
    showMainMenu();
}

void MainWindow::onProfileClicked()
{
    QMessageBox::information(this, "Profil", "Profil oldal hamarosan...");
}

void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Kijelentkezés",
        "Biztosan ki szeretnél jelentkezni?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        close();
    }
}

// ============================================================================
// SEGÉD FÜGGVÉNYEK
// ============================================================================

void MainWindow::setupConnections()
{
    // Főmenü gombok
    connect(ui->continueButton, &QPushButton::clicked,
            this, &MainWindow::onContinueClicked);

    // Interaktív oldal gombok
    connect(ui->backToMenuButton, &QPushButton::clicked,
            this, &MainWindow::onBackToMenuClicked);
    connect(ui->nextQuestionButton, &QPushButton::clicked,
            this, &MainWindow::onNextQuestionClicked);
    connect(ui->restartGameButton, &QPushButton::clicked,
            this, &MainWindow::onRestartGameClicked);

    // Eredmény oldal gombok
    connect(ui->backToMenuFromResults, &QPushButton::clicked,
            this, &MainWindow::onBackToMenuClicked);

    // Sidebar gombok
    connect(ui->homeButton, &QPushButton::clicked,
            this, &MainWindow::onHomeClicked);
    connect(ui->coursesButton, &QPushButton::clicked,
            this, &MainWindow::onCoursesClicked);
    connect(ui->profileButton, &QPushButton::clicked,
            this, &MainWindow::onProfileClicked);
    connect(ui->logoutButton, &QPushButton::clicked,
            this, &MainWindow::onLogoutClicked);
}

void MainWindow::loadQuestions()
{
    questions.clear();

    // Példa kérdések
    questions.append({
        "Melyik évben volt a második világháború vége?",
        {"1943", "1944", "1945", "1946"},
        2,
        10
    });

    questions.append({
        "Mi a Pi értéke két tizedesjegyre kerekítve?",
        {"3.12", "3.14", "3.16", "3.18"},
        1,
        10
    });

    questions.append({
        "Melyik a legnagyobb bolygó a Naprendszerben?",
        {"Föld", "Mars", "Jupiter", "Szaturnusz"},
        2,
        10
    });

    questions.append({
        "Ki festette a Mona Lisát?",
        {"Michelangelo", "Leonardo da Vinci", "Raphael", "Donatello"},
        1,
        10
    });

    questions.append({
        "Hány kontinens van a Földön?",
        {"5", "6", "7", "8"},
        2,
        10
    });
}

void MainWindow::createAnswerButtons()
{
    if (currentQuestionIndex >= questions.size()) {
        return;
    }

    Question& q = questions[currentQuestionIndex];
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->answersWidget->layout());

    if (!layout) {
        layout = new QVBoxLayout(ui->answersWidget);
        layout->setSpacing(10);
    }

    for (int i = 0; i < q.answers.size(); i++) {
        QPushButton* btn = new QPushButton(q.answers[i], ui->answersWidget);
        btn->setMinimumHeight(50);
        btn->setStyleSheet(
            "QPushButton { "
            "background-color: white; "
            "border: 2px solid #ddd; "
            "border-radius: 8px; "
            "padding: 10px; "
            "font-size: 14px; "
            "text-align: left; "
            "} "
            "QPushButton:hover { "
            "background-color: #f0f0f0; "
            "border-color: #000; "
            "}"
            );

        connect(btn, &QPushButton::clicked, [this, i]() {
            handleAnswer(i);
        });

        layout->addWidget(btn);
        answerButtons.append(btn);
    }
}

void MainWindow::clearAnswerButtons()
{
    for (QPushButton* btn : answerButtons) {
        btn->deleteLater();
    }
    answerButtons.clear();
}

void MainWindow::addCourseCard(const QString& courseName, int courseIndex)
{
    QFrame* card = new QFrame(ui->scrollAreaWidgetContents);
    card->setObjectName("courseCard");
    card->setMinimumHeight(80);
    card->setStyleSheet(
        "QFrame#courseCard { "
        "background-color: white; "
        "border-radius: 10px; "
        "padding: 15px; "
        "}"
        );

    QHBoxLayout* layout = new QHBoxLayout(card);

    QLabel* label = new QLabel(courseName, card);
    label->setStyleSheet("font-size: 16px; font-weight: bold;");
    layout->addWidget(label);

    layout->addStretch();

    QPushButton* btn = new QPushButton("View course", card);
    btn->setMinimumSize(100, 35);
    connect(btn, &QPushButton::clicked, this, &MainWindow::onViewCourseClicked);
    layout->addWidget(btn);

    ui->coursesLayout->addWidget(card);
}

void MainWindow::updateStatistics()
{
    ui->completedCoursesLabel->setText(QString::number(correctAnswers));
}
