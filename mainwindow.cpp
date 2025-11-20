#include "mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QVBoxLayout>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentQuestionIndex(0)
    , correctAnswers(0)
    , totalPoints(0)
    , answerSelected(false)
    , selectedAnswerIndex(-1)
    , selectedLanguage(Language::Hungarian)
    , selectedCategory(Category::Vocabulary)
    , selectedDifficulty(Difficulty::Beginner)
    , currentStreak(5)
    , totalWordsLearned(97)
    , selectedCategoryButton(nullptr)
    , selectedDifficultyButton(nullptr)
{
    ui->setupUi(this);
    setupConnections();

    // Kezdő állapot: főmenü
    showMainMenu();

    // Demo statisztikák betöltése (később adatbázisból)
    loadStatisticsFromBackend();
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
    loadStatisticsFromBackend();
}

void MainWindow::displayQuestion()
{
    if (currentQuestionIndex >= currentQuestions.size()) {
        showResults();
        return;
    }

    Question &q = currentQuestions[currentQuestionIndex];

    // Kérdés szöveg megjelenítése
    ui->questionLabel->setText(q.questionText);

    // Tanítandó szó/mondat megjelenítése
    ui->wordLabel->setText(q.word);

    // Válaszgombok létrehozása
    clearAnswerButtons();
    createAnswerButtons();

    // Visszajelzés törlése
    ui->feedbackLabel->clear();
    ui->feedbackLabel->setStyleSheet("");

    // UI állapot frissítése
    answerSelected = false;
    selectedAnswerIndex = -1;
    ui->nextQuestionButton->setEnabled(false);
    ui->nextQuestionButton->setText("Válasz ellenőrzése");

    updateQuestionCounter();
    updateCurrentScore();
}

void MainWindow::updateUIState()
{
    // Válasz gombok letiltása válasz után
    for (QPushButton *btn : answerButtons) {
        btn->setEnabled(!answerSelected);
    }

    // Következő gomb engedélyezése
    ui->nextQuestionButton->setEnabled(answerSelected);
}

// ============================================================================
// SZTÁNYI GYÖRGY - Interaktív elemek
// ============================================================================

void MainWindow::showResults()
{
    ui->stackedWidget->setCurrentWidget(ui->resultsPage);

    // Eredmények számítása és megjelenítése
    QString scoreText = QString("%1 / %2").arg(correctAnswers).arg(currentQuestions.size());
    ui->resultLabel->setText(scoreText);

    double percentage = (currentQuestions.size() > 0)
                            ? (correctAnswers * 100.0 / currentQuestions.size())
                            : 0.0;
    ui->percentageLabel->setText(QString("%1%").arg(QString::number(percentage, 'f', 0)));

    ui->pointsLabel->setText(QString("Pontszám: %1").arg(totalPoints));

    // Motivációs üzenet
    ui->messageLabel->setText(getMotivationalMessage(percentage));

    // Statisztika mentése backend-be
    updateStatisticsInBackend();
}

void MainWindow::restartGame()
{
    currentQuestionIndex = 0;
    correctAnswers = 0;
    totalPoints = 0;
    answerSelected = false;
    selectedAnswerIndex = -1;

    // Új kérdések betöltése backend-ből
    loadQuestionsFromBackend();

    if (currentQuestions.isEmpty()) {
        QMessageBox::information(this,
                                 "Nincs kérdés",
                                 "Ehhez a kombinációhoz még nincsenek kérdések az adatbázisban.");
        return;
    }

    ui->stackedWidget->setCurrentWidget(ui->quizPage);
    displayQuestion();
}

void MainWindow::highlightAnswer(int answerIndex, bool isCorrect)
{
    if (answerIndex >= 0 && answerIndex < answerButtons.size()) {
        QPushButton *btn = answerButtons[answerIndex];

        if (isCorrect) {
            // Zöld kiemelés helyes válaszhoz
            btn->setStyleSheet("QPushButton { "
                               "background-color: #27ae60; "
                               "color: white; "
                               "border: 3px solid #1e8449; "
                               "border-radius: 10px; "
                               "padding: 15px; "
                               "font-size: 16px; "
                               "font-weight: bold; "
                               "text-align: left; "
                               "}");
        } else {
            // Piros kiemelés helytelen válaszhoz
            btn->setStyleSheet("QPushButton { "
                               "background-color: #e74c3c; "
                               "color: white; "
                               "border: 3px solid #c0392b; "
                               "border-radius: 10px; "
                               "padding: 15px; "
                               "font-size: 16px; "
                               "font-weight: bold; "
                               "text-align: left; "
                               "}");
        }

        // Helyes válasz mindig zöld (ha hibáztunk)
        if (!isCorrect && currentQuestionIndex < currentQuestions.size()) {
            int correctIdx = currentQuestions[currentQuestionIndex].correctAnswer;
            if (correctIdx >= 0 && correctIdx < answerButtons.size()) {
                answerButtons[correctIdx]->setStyleSheet("QPushButton { "
                                                         "background-color: #27ae60; "
                                                         "color: white; "
                                                         "border: 3px solid #1e8449; "
                                                         "border-radius: 10px; "
                                                         "padding: 15px; "
                                                         "font-size: 16px; "
                                                         "font-weight: bold; "
                                                         "text-align: left; "
                                                         "}");
            }
        }
    }
}

// ============================================================================
// HRABINA GERGŐ - Kérdések és válaszok kezelése
// ============================================================================

void MainWindow::handleAnswer(int answerIndex)
{
    if (answerSelected) {
        return; // Ha már ellenőrizve van, ne csináljon semmit
    }

    // Előző kijelölés törlése
    if (selectedAnswerIndex >= 0 && selectedAnswerIndex < answerButtons.size()) {
        answerButtons[selectedAnswerIndex]->setStyleSheet("QPushButton { "
                                                          "background-color: white; "
                                                          "color: #000000; "
                                                          "border: 2px solid #bdc3c7; "
                                                          "border-radius: 10px; "
                                                          "padding: 15px; "
                                                          "font-size: 16px; "
                                                          "text-align: left; "
                                                          "} "
                                                          "QPushButton:hover { "
                                                          "background-color: #ecf0f1; "
                                                          "border-color: #000000; "
                                                          "}");
    }

    // Új válasz kijelölése (KÉK kiemelés)
    selectedAnswerIndex = answerIndex;
    if (answerIndex >= 0 && answerIndex < answerButtons.size()) {
        answerButtons[answerIndex]->setStyleSheet("QPushButton { "
                                                  "background-color: #3498db; "
                                                  "color: white; "
                                                  "border: 3px solid #2980b9; "
                                                  "border-radius: 10px; "
                                                  "padding: 15px; "
                                                  "font-size: 16px; "
                                                  "text-align: left; "
                                                  "font-weight: bold; "
                                                  "} "
                                                  "QPushButton:hover { "
                                                  "background-color: #2980b9; "
                                                  "}");

        // Következő gomb engedélyezése
        ui->nextQuestionButton->setEnabled(true);
    }
}

void MainWindow::checkSelectedAnswer()
{
    if (selectedAnswerIndex < 0 || currentQuestionIndex >= currentQuestions.size()) {
        return; // Nincs kiválasztva válasz
    }

    Question &q = currentQuestions[currentQuestionIndex];
    // Válasz ellenőrzése
    bool isCorrect = gameLogic.checkAnswer(q, selectedAnswerIndex);

    if (isCorrect) {
        correctAnswers++;
        totalPoints = gameLogic.calculateScore(totalPoints, q.points); // ITT A LÉNYEG
    }

    answerSelected = true;

    // Backend-be küldés
    sendAnswerToBackend(selectedAnswerIndex);

    // Válasz vizuális kiemelése (ZÖLD vagy PIROS)
    highlightAnswer(selectedAnswerIndex, isCorrect);

    // Visszajelzés megjelenítése
    showFeedback(isCorrect);

    // UI állapot frissítése
    updateUIState();
    updateCurrentScore();
}

void MainWindow::showFeedback(bool isCorrect)
{
    if (isCorrect) {
        ui->feedbackLabel->setText("✓ Helyes válasz! Nagyszerű! 🎉");
        ui->feedbackLabel->setStyleSheet("QLabel { "
                                         "background-color: #d5f4e6; "
                                         "color: #0f5132; "
                                         "border: 2px solid #27ae60; "
                                         "border-radius: 12px; "
                                         "padding: 15px; "
                                         "font-size: 18px; "
                                         "font-weight: bold; "
                                         "}");
    } else {
        QString correctAns = "";
        if (currentQuestionIndex < currentQuestions.size()) {
            int idx = currentQuestions[currentQuestionIndex].correctAnswer;
            if (idx >= 0 && idx < currentQuestions[currentQuestionIndex].answers.size()) {
                correctAns = currentQuestions[currentQuestionIndex].answers[idx];
            }
        }

        ui->feedbackLabel->setText(QString("✗ Helytelen! A helyes válasz: %1").arg(correctAns));
        ui->feedbackLabel->setStyleSheet("QLabel { "
                                         "background-color: #f8d7da; "
                                         "color: #721c24; "
                                         "border: 2px solid #e74c3c; "
                                         "border-radius: 12px; "
                                         "padding: 15px; "
                                         "font-size: 18px; "
                                         "font-weight: bold; "
                                         "}");
    }
}

void MainWindow::enableNextQuestion()
{
    ui->nextQuestionButton->setEnabled(true);
}

// ============================================================================
// SLOTS - Nyelvválasztás
// ============================================================================

void MainWindow::onHungarianSelected()
{
    selectedLanguage = Language::Hungarian;
    showCategoryPage();
}

void MainWindow::onEnglishSelected()
{
    selectedLanguage = Language::English;
    showCategoryPage();
}

void MainWindow::onGermanSelected()
{
    selectedLanguage = Language::German;
    showCategoryPage();
}

void MainWindow::onRussianSelected()
{
    selectedLanguage = Language::Russian;
    showCategoryPage();
}

// ============================================================================
// SLOTS - Kategória választás (KIJELÖLÉS FUNKCIÓVAL)
// ============================================================================

void MainWindow::onVocabularySelected()
{
    handleCategorySelection(ui->vocabularyButton, Category::Vocabulary);
}

void MainWindow::onGrammarSelected()
{
    handleCategorySelection(ui->grammarButton, Category::Grammar);
}

void MainWindow::onSentencesSelected()
{
    handleCategorySelection(ui->sentencesButton, Category::Sentences);
}

void MainWindow::onListeningSelected()
{
    handleCategorySelection(ui->listeningButton, Category::Listening);
}

void MainWindow::handleCategorySelection(QPushButton *button, Category category)
{
    // Ha ugyanarra kattintunk újra - visszavonjuk
    if (selectedCategoryButton == button) {
        button->setStyleSheet("background-color: rgb(0, 0, 0);");
        selectedCategoryButton = nullptr;
        return;
    }

    // Előző gomb visszaállítása
    if (selectedCategoryButton != nullptr) {
        selectedCategoryButton->setStyleSheet("background-color: rgb(0, 0, 0);");
    }

    // Új gomb kijelölése (PIROS)
    button->setStyleSheet("QPushButton { "
                          "background-color: rgb(231, 76, 60); "
                          "color: white; "
                          "border: 3px solid rgb(192, 57, 43); "
                          "border-radius: 10px; "
                          "} "
                          "QPushButton:hover { "
                          "background-color: rgb(192, 57, 43); "
                          "}");

    selectedCategoryButton = button;
    selectedCategory = category;
}

// ============================================================================
// SLOTS - Nehézségi szint választás (KIJELÖLÉS FUNKCIÓVAL)
// ============================================================================

void MainWindow::onBeginnerSelected()
{
    handleDifficultySelection(ui->beginnerButton, Difficulty::Beginner);
}

void MainWindow::onIntermediateSelected()
{
    handleDifficultySelection(ui->intermediateButton, Difficulty::Intermediate);
}

void MainWindow::onAdvancedSelected()
{
    handleDifficultySelection(ui->advancedButton, Difficulty::Advanced);
}

void MainWindow::handleDifficultySelection(QPushButton *button, Difficulty difficulty)
{
    // Ha ugyanarra kattintunk újra - visszavonjuk
    if (selectedDifficultyButton == button) {
        button->setStyleSheet("background-color: rgb(0, 0, 0);");
        selectedDifficultyButton = nullptr;
        return;
    }

    // Előző gomb visszaállítása
    if (selectedDifficultyButton != nullptr) {
        selectedDifficultyButton->setStyleSheet("background-color: rgb(0, 0, 0);");
    }

    // Új gomb kijelölése (NARANCSSÁRGA)
    button->setStyleSheet("QPushButton { "
                          "background-color: rgb(255, 165, 0); "
                          "color: white; "
                          "border: 3px solid rgb(255, 140, 0); "
                          "border-radius: 10px; "
                          "} "
                          "QPushButton:hover { "
                          "background-color: rgb(255, 140, 0); "
                          "}");

    selectedDifficultyButton = button;
    selectedDifficulty = difficulty;
}

// ============================================================================
// SLOTS - Navigáció
// ============================================================================

void MainWindow::onStartQuiz()
{
    // Kérdések betöltése backend-ből a kiválasztott beállításokkal
    loadQuestionsFromBackend();

    if (currentQuestions.isEmpty()) {
        QMessageBox::information(this,
                                 "Nincs kérdés",
                                 "Ehhez a kombinációhoz még nincsenek kérdések az adatbázisban.");
        return;
    }

    currentQuestionIndex = 0;
    correctAnswers = 0;
    totalPoints = 0;
    answerSelected = false;
    selectedAnswerIndex = -1;

    // Kategória és nehézség megjelenítése a kvíz oldalon
    QString categoryInfo = QString("%1 %2 | %3 %4")
                               .arg(getCategoryIcon(selectedCategory))
                               .arg(getCategoryName(selectedCategory))
                               .arg(getDifficultyIcon(selectedDifficulty))
                               .arg(getDifficultyName(selectedDifficulty));
    ui->categoryInfoLabel->setText(categoryInfo);

    ui->stackedWidget->setCurrentWidget(ui->quizPage);
    displayQuestion();
}

void MainWindow::onBackFromCategory()
{
    showMainMenu();
}

void MainWindow::onBackToMenu()
{
    showMainMenu();
}

void MainWindow::onNextQuestion()
{
    // Ha még nem ellenőriztük a választ, először ellenőrizzük
    if (!answerSelected && selectedAnswerIndex >= 0) {
        checkSelectedAnswer();
        ui->nextQuestionButton->setText("Következő ➡");
        return;
    }

    // Ha már ellenőrizve van, ugrunk a következő kérdésre
    if (answerSelected) {
        currentQuestionIndex++;
        displayQuestion();
    }
}

void MainWindow::onQuitQuiz()
{
    QMessageBox::StandardButton reply
        = QMessageBox::question(this,
                                "Kilépés",
                                "Biztosan abbahagyod a kvízt? Az eredmények nem lesznek mentve.",
                                QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        showMainMenu();
    }
}

void MainWindow::onRestartSame()
{
    restartGame();
}

void MainWindow::onNextLevel()
{
    if (selectedDifficulty == Difficulty::Beginner) {
        selectedDifficulty = Difficulty::Intermediate;
    } else if (selectedDifficulty == Difficulty::Intermediate) {
        selectedDifficulty = Difficulty::Advanced;
    } else {
        QMessageBox::information(this, "Maximális szint", "Már a legmagasabb szinten vagy! 🏆");
        return;
    }

    restartGame();
}

// ============================================================================
// SLOTS - Sidebar navigáció
// ============================================================================

void MainWindow::onHomeClicked()
{
    showMainMenu();
}

void MainWindow::onLanguagesClicked()
{
    showMainMenu();
}

void MainWindow::onStatsClicked()
{
    showStatisticsPage();
}

void MainWindow::onProfileClicked()
{
    QMessageBox::information(this, "Profil", "Profil oldal hamarosan elérhető...");
}

void MainWindow::onLogoutClicked()
{
    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Kijelentkezés",
                                                              "Biztosan ki szeretnél jelentkezni?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        close();
    }
}

void MainWindow::onBackFromStatsClicked()
{
    showMainMenu();
}

// ============================================================================
// SEGÉD FÜGGVÉNYEK - UI kezelés
// ============================================================================

void MainWindow::setupConnections()
{
    // Nyelvválasztó gombok
    connect(ui->hungarianButton, &QPushButton::clicked, this, &MainWindow::onHungarianSelected);
    connect(ui->englishButton, &QPushButton::clicked, this, &MainWindow::onEnglishSelected);
    connect(ui->germanButton, &QPushButton::clicked, this, &MainWindow::onGermanSelected);
    connect(ui->russianButton, &QPushButton::clicked, this, &MainWindow::onRussianSelected);

    // Kategória gombok
    connect(ui->vocabularyButton, &QPushButton::clicked, this, &MainWindow::onVocabularySelected);
    connect(ui->grammarButton, &QPushButton::clicked, this, &MainWindow::onGrammarSelected);
    connect(ui->sentencesButton, &QPushButton::clicked, this, &MainWindow::onSentencesSelected);
    connect(ui->listeningButton, &QPushButton::clicked, this, &MainWindow::onListeningSelected);

    // Nehézségi szint gombok
    connect(ui->beginnerButton, &QPushButton::clicked, this, &MainWindow::onBeginnerSelected);
    connect(ui->intermediateButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onIntermediateSelected);
    connect(ui->advancedButton, &QPushButton::clicked, this, &MainWindow::onAdvancedSelected);

    // Navigációs gombok
    connect(ui->startQuizButton, &QPushButton::clicked, this, &MainWindow::onStartQuiz);
    connect(ui->backFromCategoryButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onBackFromCategory);
    connect(ui->backToMenuButton, &QPushButton::clicked, this, &MainWindow::onBackToMenu);
    connect(ui->nextQuestionButton, &QPushButton::clicked, this, &MainWindow::onNextQuestion);
    connect(ui->quitQuizButton, &QPushButton::clicked, this, &MainWindow::onQuitQuiz);
    connect(ui->restartSameButton, &QPushButton::clicked, this, &MainWindow::onRestartSame);
    connect(ui->nextLevelButton, &QPushButton::clicked, this, &MainWindow::onNextLevel);
    connect(ui->backFromStatsButton,
            &QPushButton::clicked,
            this,
            &MainWindow::onBackFromStatsClicked);

    // Sidebar gombok
    connect(ui->homeButton, &QPushButton::clicked, this, &MainWindow::onHomeClicked);
    connect(ui->languagesButton, &QPushButton::clicked, this, &MainWindow::onLanguagesClicked);
    connect(ui->statsButton, &QPushButton::clicked, this, &MainWindow::onStatsClicked);
    connect(ui->profileButton, &QPushButton::clicked, this, &MainWindow::onProfileClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
}

void MainWindow::createAnswerButtons()
{
    if (currentQuestionIndex >= currentQuestions.size()) {
        return;
    }

    Question &q = currentQuestions[currentQuestionIndex];
    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(ui->answersWidget->layout());

    if (!layout) {
        layout = new QVBoxLayout(ui->answersWidget);
        layout->setSpacing(15);
        layout->setContentsMargins(0, 0, 0, 0);
    }

    for (int i = 0; i < q.answers.size(); i++) {
        QPushButton *btn = new QPushButton(q.answers[i], ui->answersWidget);
        btn->setMinimumHeight(60);
        btn->setMaximumHeight(60);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setStyleSheet("QPushButton { "
                           "background-color: white; "
                           "color: #000000; "
                           "border: 2px solid #bdc3c7; "
                           "border-radius: 10px; "
                           "padding: 15px; "
                           "font-size: 16px; "
                           "text-align: left; "
                           "} "
                           "QPushButton:hover { "
                           "background-color: #ecf0f1; "
                           "border-color: #000000; "
                           "color: #000000; "
                           "}");

        connect(btn, &QPushButton::clicked, [this, i]() { handleAnswer(i); });

        layout->addWidget(btn);
        answerButtons.append(btn);
    }
}

void MainWindow::clearAnswerButtons()
{
    for (QPushButton *btn : answerButtons) {
        btn->deleteLater();
    }
    answerButtons.clear();
}

void MainWindow::updateQuestionCounter()
{
    QString counterText
        = QString("Kérdés %1/%2").arg(currentQuestionIndex + 1).arg(currentQuestions.size());
    ui->questionNumberLabel->setText(counterText);
}

void MainWindow::updateCurrentScore()
{
    ui->currentScoreLabel->setText(QString("Pontszám: %1").arg(totalPoints));
}

void MainWindow::showCategoryPage()
{
    QString langText = QString("Kiválasztott nyelv: %1 %2")
                           .arg(getLanguageName(selectedLanguage))
                           .arg(getLanguageFlag(selectedLanguage));
    ui->selectedLanguageLabel->setText(langText);

    ui->stackedWidget->setCurrentWidget(ui->categoryPage);
}

void MainWindow::showStatisticsPage()
{
    loadStatisticsFromBackend();
    ui->stackedWidget->setCurrentWidget(ui->statisticsPage);
}

// ============================================================================
// UI Szövegek és ikonok
// ============================================================================

QString MainWindow::getLanguageName(Language lang)
{
    switch (lang) {
    case Language::Hungarian:
        return "Magyar";
    case Language::English:
        return "English";
    case Language::German:
        return "Deutsch";
    case Language::Russian:
        return "Русский";
    default:
        return "Ismeretlen";
    }
}

QString MainWindow::getLanguageFlag(Language lang)
{
    switch (lang) {
    case Language::Hungarian:
        return "🇭🇺";
    case Language::English:
        return "🇬🇧";
    case Language::German:
        return "🇩🇪";
    case Language::Russian:
        return "🇷🇺";
    default:
        return "🌍";
    }
}

QString MainWindow::getCategoryName(Category cat)
{
    switch (cat) {
    case Category::Vocabulary:
        return "Szókincs";
    case Category::Grammar:
        return "Nyelvtan";
    case Category::Sentences:
        return "Mondatok";
    case Category::Listening:
        return "Hallás utáni";
    default:
        return "Ismeretlen";
    }
}

QString MainWindow::getCategoryIcon(Category cat)
{
    switch (cat) {
    case Category::Vocabulary:
        return "📖";
    case Category::Grammar:
        return "✏️";
    case Category::Sentences:
        return "💬";
    case Category::Listening:
        return "🎧";
    default:
        return "📚";
    }
}

QString MainWindow::getDifficultyName(Difficulty diff)
{
    switch (diff) {
    case Difficulty::Beginner:
        return "Kezdő";
    case Difficulty::Intermediate:
        return "Haladó";
    case Difficulty::Advanced:
        return "Profi";
    default:
        return "Ismeretlen";
    }
}

QString MainWindow::getDifficultyIcon(Difficulty diff)
{
    switch (diff) {
    case Difficulty::Beginner:
        return "🌱";
    case Difficulty::Intermediate:
        return "⭐";
    case Difficulty::Advanced:
        return "🏆";
    default:
        return "🎯";
    }
}

QString MainWindow::getMotivationalMessage(double accuracy)
{
    if (accuracy >= 90) {
        return "Fantasztikus! Te egy nyelvzseni vagy! 🌟";
    } else if (accuracy >= 80) {
        return "Nagyszerű munka! Így tovább! 👍";
    } else if (accuracy >= 70) {
        return "Jó munka! Még egy kicsit és tökéletes leszel! 💪";
    } else if (accuracy >= 60) {
        return "Szép teljesítmény! Folytasd a gyakorlást! 📚";
    } else if (accuracy >= 50) {
        return "Jó kezdés! A gyakorlás teszi a mestert! 🎯";
    } else {
        return "Ne add fel! Minden mester gyakorlással kezdte! 🚀";
    }
}

// ============================================================================
// BACKEND INTERFACE - Placeholder függvények
// ============================================================================

void MainWindow::loadQuestionsFromBackend()
{
    // Bányai Bence: getRandomQuestion implementálva
    // TODO: KOSZTEL TAMÁS - loadLanguageData() implementálása
    // TODO: LUKÁCS VIKTÓRIA - loadQuestions() MySQL-ből
    currentQuestions.clear();

    for (int i = 0; i < 10; i++) {
        Question q = gameLogic.getRandomQuestion(selectedLanguage,
                                                 selectedCategory,
                                                 selectedDifficulty);

        if (q.id != -1) {
            currentQuestions.append(q);
        }
    }

    qDebug() << "Backend hívás: loadQuestions(" << (int) selectedLanguage << ", "
             << (int) selectedCategory << ", " << (int) selectedDifficulty << ")";
}

void MainWindow::sendAnswerToBackend(int answerIndex)
{
    // Bányai Bence: A logikai ellenőrzés már megtörtént a GameLogic-ban.
    // TODO: Később itt kell majd elküldeni az adatot az adatbázisnak.

    qDebug() << "Backend hívás: checkAnswer(" << answerIndex << ")";
}

void MainWindow::updateStatisticsInBackend()
{
    // TODO: NAGY ATTILA - updateStatistics() implementálása
    // TODO: LUKÁCS VIKTÓRIA - saveUserScore() MySQL-be

    qDebug() << "Backend hívás: updateStatistics(" << correctAnswers << "/"
             << currentQuestions.size() << ", points: " << totalPoints << ")";
}

void MainWindow::loadStatisticsFromBackend()
{
    // TODO: NAGY ATTILA - getPlayerStats() implementálása
    // TODO: LUKÁCS VIKTÓRIA - loadHighScores() MySQL-ből

    qDebug() << "Backend hívás: loadStatistics()";

    // Ideiglenes demo statisztikák
    ui->completedLabel->setText("12");
    ui->wordsLearnedLabel->setText("97");
    ui->streakLabel->setText("5");

    ui->langStats1->setText("25 szó | 85%");
    ui->langStats2->setText("42 szó | 92%");
    ui->langStats3->setText("18 szó | 78%");
    ui->langStats4->setText("12 szó | 70%");
}

// ============================================================================
// DEMO KÉRDÉSEK - Ezt később törölni kell amikor az adatbázis kész van
// ============================================================================

void MainWindow::loadDemoQuestions()
{
    currentQuestions.clear();

    for (int i = 0; i < 10; i++) {
        Question q = gameLogic.getRandomQuestion(selectedLanguage,
                                                 selectedCategory,
                                                 selectedDifficulty);

        // Csak érvényes kérdést adunk hozzá
        if (q.id != -1) {
            currentQuestions.append(q);
        }
    }

    qDebug() << "Backend hívás: loadQuestions(" << (int) selectedLanguage << ", "
             << (int) selectedCategory << ", " << (int) selectedDifficulty << ")";
}

void MainWindow::updateStatistics()
{
    // TODO: NAGY ATTILA - később implementálja
    // Statisztikák frissítése az UI-ban
    qDebug() << "updateStatistics() called";
}

void MainWindow::filterQuestions()
{
    // TODO: KOSZTEL TAMÁS - később implementálja
    // Kérdések szűrése nyelv, kategória, nehézség alapján
    qDebug() << "filterQuestions() called";
}
