#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentQuestionIndex(0)
    , correctAnswers(0)
    , totalPoints(0)
    , answerSelected(false)
    , selectedLanguage(Language::Hungarian)
    , selectedCategory(Category::Vocabulary)
    , selectedDifficulty(Difficulty::Beginner)
    , currentStreak(5)
    , totalWordsLearned(97)
{
    ui->setupUi(this);

    setupConnections();
    loadQuestions();

    // Kezdő állapot: főmenü
    showMainMenu();

    // Statisztikák inicializálása (példa adatok)
    statistics[Language::Hungarian] = {25, 10, 85, 100, 85.0};
    statistics[Language::English] = {42, 15, 138, 150, 92.0};
    statistics[Language::German] = {18, 7, 55, 70, 78.6};
    statistics[Language::Russian] = {12, 5, 35, 50, 70.0};
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

    Question& q = questions[currentQuestionIndex];

    // Kérdés szöveg
    ui->questionLabel->setText(q.questionText);

    // Tanítandó szó/mondat
    ui->wordLabel->setText(q.word);

    // Válaszgombok létrehozása
    clearAnswerButtons();
    createAnswerButtons();

    // Visszajelzés törlése
    ui->feedbackLabel->clear();
    ui->feedbackLabel->setStyleSheet("");

    // UI frissítés
    answerSelected = false;
    ui->nextQuestionButton->setEnabled(false);

    updateQuestionCounter();
    updateCurrentScore();
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

    // Eredmények számítása
    QString scoreText = QString("%1 / %2")
                            .arg(correctAnswers)
                            .arg(questions.size());
    ui->resultLabel->setText(scoreText);

    double percentage = (questions.size() > 0) ?
                            (correctAnswers * 100.0 / questions.size()) : 0.0;
    ui->percentageLabel->setText(QString("%1%").arg(QString::number(percentage, 'f', 0)));

    ui->pointsLabel->setText(QString("Pontszám: %1").arg(totalPoints));

    // Motivációs üzenet
    ui->messageLabel->setText(getMotivationalMessage(percentage));

    // Statisztika frissítése
    updateStatistics();
}

void MainWindow::restartGame()
{
    currentQuestionIndex = 0;
    correctAnswers = 0;
    totalPoints = 0;
    answerSelected = false;

    filterQuestions();

    ui->stackedWidget->setCurrentWidget(ui->quizPage);
    displayQuestion();
}

void MainWindow::highlightAnswer(int answerIndex, bool isCorrect)
{
    if (answerIndex >= 0 && answerIndex < answerButtons.size()) {
        QPushButton* btn = answerButtons[answerIndex];

        if (isCorrect) {
            btn->setStyleSheet(
                "QPushButton { "
                "background-color: #27ae60; "
                "color: white; "
                "border: 3px solid #1e8449; "
                "font-size: 16px; "
                "}"
                );
        } else {
            btn->setStyleSheet(
                "QPushButton { "
                "background-color: #e74c3c; "
                "color: white; "
                "border: 3px solid #c0392b; "
                "font-size: 16px; "
                "}"
                );
        }

        // Helyes válasz mindig zöld
        if (!isCorrect && currentQuestionIndex < questions.size()) {
            int correctIdx = questions[currentQuestionIndex].correctAnswer;
            if (correctIdx >= 0 && correctIdx < answerButtons.size()) {
                answerButtons[correctIdx]->setStyleSheet(
                    "QPushButton { "
                    "background-color: #27ae60; "
                    "color: white; "
                    "border: 3px solid #1e8449; "
                    "font-size: 16px; "
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

    // Visszajelzés
    showFeedback(isCorrect);

    // UI frissítés
    updateUIState();
    updateCurrentScore();

    // Következő kérdés engedélyezése
    enableNextQuestion();
}

void MainWindow::showFeedback(bool isCorrect)
{
    if (isCorrect) {
        ui->feedbackLabel->setText("✓ Helyes válasz! Nagyszerű! 🎉");
        ui->feedbackLabel->setStyleSheet(
            "QLabel { "
            "background-color: #d5f4e6; "
            "color: #0f5132; "
            "border: 2px solid #27ae60; "
            "border-radius: 12px; "
            "padding: 15px; "
            "font-size: 18px; "
            "font-weight: bold; "
            "}"
            );
    } else {
        QString correctAns = "";
        if (currentQuestionIndex < questions.size()) {
            int idx = questions[currentQuestionIndex].correctAnswer;
            if (idx >= 0 && idx < questions[currentQuestionIndex].answers.size()) {
                correctAns = questions[currentQuestionIndex].answers[idx];
            }
        }

        ui->feedbackLabel->setText(QString("✗ Helytelen! A helyes válasz: %1").arg(correctAns));
        ui->feedbackLabel->setStyleSheet(
            "QLabel { "
            "background-color: #f8d7da; "
            "color: #721c24; "
            "border: 2px solid #e74c3c; "
            "border-radius: 12px; "
            "padding: 15px; "
            "font-size: 18px; "
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
// SLOTS - Kategória és nehézség
// ============================================================================

void MainWindow::onVocabularySelected()
{
    selectedCategory = Category::Vocabulary;
    ui->vocabularyButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; }"
        );
}

void MainWindow::onGrammarSelected()
{
    selectedCategory = Category::Grammar;
    ui->grammarButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; }"
        );
}

void MainWindow::onSentencesSelected()
{
    selectedCategory = Category::Sentences;
    ui->sentencesButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; }"
        );
}

void MainWindow::onListeningSelected()
{
    selectedCategory = Category::Listening;
    ui->listeningButton->setStyleSheet(
        "QPushButton { background-color: #e74c3c; color: white; }"
        );
}

void MainWindow::onBeginnerSelected()
{
    selectedDifficulty = Difficulty::Beginner;
    ui->beginnerButton->setStyleSheet(
        "QPushButton { background-color: #f39c12; color: white; }"
        );
}

void MainWindow::onIntermediateSelected()
{
    selectedDifficulty = Difficulty::Intermediate;
    ui->intermediateButton->setStyleSheet(
        "QPushButton { background-color: #f39c12; color: white; }"
        );
}

void MainWindow::onAdvancedSelected()
{
    selectedDifficulty = Difficulty::Advanced;
    ui->advancedButton->setStyleSheet(
        "QPushButton { background-color: #f39c12; color: white; }"
        );
}

// ============================================================================
// SLOTS - Navigáció
// ============================================================================

void MainWindow::onStartQuiz()
{
    filterQuestions();

    if (questions.isEmpty()) {
        QMessageBox::information(this, "Nincs kérdés",
                                 "Ehhez a kombinációhoz még nincsenek kérdések.");
        return;
    }

    currentQuestionIndex = 0;
    correctAnswers = 0;
    totalPoints = 0;
    answerSelected = false;

    // Kategória és nehézség megjelenítése
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
    currentQuestionIndex++;
    displayQuestion();
}

void MainWindow::onQuitQuiz()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "Kilépés",
        "Biztosan abbahagyod a kvízt? Az eredmények nem lesznek mentve.",
        QMessageBox::Yes | QMessageBox::No
        );

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
        QMessageBox::information(this, "Maximális szint",
                                 "Már a legmagasabb szinten vagy! 🏆");
        return;
    }

    restartGame();
}

// ============================================================================
// SLOTS - Sidebar
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
    // Nyelvválasztó gombok
    connect(ui->hungarianButton, &QPushButton::clicked,
            this, &MainWindow::onHungarianSelected);
    connect(ui->englishButton, &QPushButton::clicked,
            this, &MainWindow::onEnglishSelected);
    connect(ui->germanButton, &QPushButton::clicked,
            this, &MainWindow::onGermanSelected);
    connect(ui->russianButton, &QPushButton::clicked,
            this, &MainWindow::onRussianSelected);

    // Kategória gombok
    connect(ui->vocabularyButton, &QPushButton::clicked,
            this, &MainWindow::onVocabularySelected);
    connect(ui->grammarButton, &QPushButton::clicked,
            this, &MainWindow::onGrammarSelected);
    connect(ui->sentencesButton, &QPushButton::clicked,
            this, &MainWindow::onSentencesSelected);
    connect(ui->listeningButton, &QPushButton::clicked,
            this, &MainWindow::onListeningSelected);

    // Nehézségi szint gombok
    connect(ui->beginnerButton, &QPushButton::clicked,
            this, &MainWindow::onBeginnerSelected);
    connect(ui->intermediateButton, &QPushButton::clicked,
            this, &MainWindow::onIntermediateSelected);
    connect(ui->advancedButton, &QPushButton::clicked,
            this, &MainWindow::onAdvancedSelected);

    // Navigációs gombok
    connect(ui->startQuizButton, &QPushButton::clicked,
            this, &MainWindow::onStartQuiz);
    connect(ui->backFromCategoryButton, &QPushButton::clicked,
            this, &MainWindow::onBackFromCategory);
    connect(ui->backToMenuButton, &QPushButton::clicked,
            this, &MainWindow::onBackToMenu);
    connect(ui->nextQuestionButton, &QPushButton::clicked,
            this, &MainWindow::onNextQuestion);
    connect(ui->quitQuizButton, &QPushButton::clicked,
            this, &MainWindow::onQuitQuiz);
    connect(ui->restartSameButton, &QPushButton::clicked,
            this, &MainWindow::onRestartSame);
    connect(ui->nextLevelButton, &QPushButton::clicked,
            this, &MainWindow::onNextLevel);
    connect(ui->backFromStatsButton, &QPushButton::clicked,
            this, &MainWindow::onBackToMenu);

    // Sidebar gombok
    connect(ui->homeButton, &QPushButton::clicked,
            this, &MainWindow::onHomeClicked);
    connect(ui->languagesButton, &QPushButton::clicked,
            this, &MainWindow::onLanguagesClicked);
    connect(ui->statsButton, &QPushButton::clicked,
            this, &MainWindow::onStatsClicked);
    connect(ui->profileButton, &QPushButton::clicked,
            this, &MainWindow::onProfileClicked);
    connect(ui->logoutButton, &QPushButton::clicked,
            this, &MainWindow::onLogoutClicked);
}

void MainWindow::loadQuestions()
{
    questions.clear();

    // ========================================================================
    // ANGOL - Szókincs - Kezdő
    // ========================================================================
    questions.append({
        "Mit jelent ez a szó magyarul?",
        "apple",
        {"alma", "banán", "körte", "szilva"},
        0, 10,
        Language::English, Category::Vocabulary, Difficulty::Beginner,
        "Az 'apple' magyarul almát jelent."
    });

    questions.append({
        "Mit jelent ez a szó magyarul?",
        "dog",
        {"macska", "kutya", "madár", "hal"},
        1, 10,
        Language::English, Category::Vocabulary, Difficulty::Beginner,
        "A 'dog' magyarul kutyát jelent."
    });

    questions.append({
        "Mit jelent ez a szó magyarul?",
        "water",
        {"tűz", "föld", "víz", "levegő"},
        2, 10,
        Language::English, Category::Vocabulary, Difficulty::Beginner,
        "A 'water' magyarul vizet jelent."
    });

    // ========================================================================
    // ANGOL - Szókincs - Haladó
    // ========================================================================
    questions.append({
        "Mit jelent ez a szó magyarul?",
        "achievement",
        {"kudarc", "eredmény", "teljesítmény", "próbálkozás"},
        2, 15,
        Language::English, Category::Vocabulary, Difficulty::Intermediate,
        "Az 'achievement' magyarul teljesítményt jelent."
    });

    // ========================================================================
    // NÉMET - Szókincs - Kezdő
    // ========================================================================
    questions.append({
        "Mit jelent ez a szó magyarul?",
        "Haus",
        {"ház", "autó", "fa", "kert"},
        0, 10,
        Language::German, Category::Vocabulary, Difficulty::Beginner,
        "A 'Haus' magyarul házat jelent."
    });

    questions.append({
        "Mit jelent ez a szó magyarul?",
        "Katze",
        {"kutya", "macska", "egér", "madár"},
        1, 10,
        Language::German, Category::Vocabulary, Difficulty::Beginner,
        "A 'Katze' magyarul macskát jelent."
    });

    // ========================================================================
    // OROSZ - Szókincs - Kezdő
    // ========================================================================
    questions.append({
        "Mit jelent ez a szó magyarul?",
        "книга",
        {"könyv", "ceruza", "papír", "toll"},
        0, 10,
        Language::Russian, Category::Vocabulary, Difficulty::Beginner,
        "A 'книга' (knyiga) magyarul könyvet jelent."
    });

    // ========================================================================
    // ANGOL - Nyelvtan - Kezdő
    // ========================================================================
    questions.append({
        "Melyik a helyes mondat?",
        "I ___ a student.",
        {"am", "is", "are", "be"},
        0, 10,
        Language::English, Category::Grammar, Difficulty::Beginner,
        "Az 'I' után mindig 'am' áll jelen időben."
    });

    // ========================================================================
    // ANGOL - Mondatok - Kezdő
    // ========================================================================
    questions.append({
        "Fordítsd le magyarra:",
        "Hello, how are you?",
        {"Szia, hogy vagy?", "Viszlát!", "Köszönöm!", "Szép napot!"},
        0, 10,
        Language::English, Category::Sentences, Difficulty::Beginner,
        "Ez egy alap üdvözlő mondat."
    });

    questions.append({
        "Fordítsd le magyarra:",
        "I love you.",
        {"Utállak.", "Szeretlek.", "Kedvellek.", "Hiányzol."},
        1, 10,
        Language::English, Category::Sentences, Difficulty::Beginner,
        "Az 'I love you' magyarul 'Szeretlek' jelentésű."
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
        layout->setSpacing(15);
    }

    for (int i = 0; i < q.answers.size(); i++) {
        QPushButton* btn = new QPushButton(q.answers[i], ui->answersWidget);
        btn->setMinimumHeight(60);
        btn->setStyleSheet(
            "QPushButton { "
            "background-color: white; "
            "border: 2px solid #bdc3c7; "
            "border-radius: 10px; "
            "padding: 15px; "
            "font-size: 16px; "
            "text-align: left; "
            "} "
            "QPushButton:hover { "
            "background-color: #ecf0f1; "
            "border-color: #3498db; "
            "border-width: 3px; "
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

void MainWindow::updateStatistics()
{
    // Főmenü statisztikák frissítése
    int totalCompleted = 0;
    for (auto it = statistics.begin(); it != statistics.end(); ++it) {
        totalCompleted += it.value().quizzesCompleted;
    }

    ui->completedLabel->setText(QString::number(totalCompleted));
    ui->wordsLearnedLabel->setText(QString::number(totalWordsLearned));
    ui->streakLabel->setText(QString::number(currentStreak));

    // Statisztika oldal frissítése nyelvenkénti adatokkal
    ui->langStats1->setText(QString("%1 szó | %2%")
                                .arg(statistics[Language::Hungarian].wordsLearned)
                                .arg(QString::number(statistics[Language::Hungarian].accuracy, 'f', 0)));

    ui->langStats2->setText(QString("%1 szó | %2%")
                                .arg(statistics[Language::English].wordsLearned)
                                .arg(QString::number(statistics[Language::English].accuracy, 'f', 0)));

    ui->langStats3->setText(QString("%1 szó | %2%")
                                .arg(statistics[Language::German].wordsLearned)
                                .arg(QString::number(statistics[Language::German].accuracy, 'f', 0)));

    ui->langStats4->setText(QString("%1 szó | %2%")
                                .arg(statistics[Language::Russian].wordsLearned)
                                .arg(QString::number(statistics[Language::Russian].accuracy, 'f', 0)));
}

void MainWindow::updateQuestionCounter()
{
    QString counterText = QString("Kérdés %1/%2")
                              .arg(currentQuestionIndex + 1)
                              .arg(questions.size());
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
    updateStatistics();
    ui->stackedWidget->setCurrentWidget(ui->statisticsPage);
}

// ============================================================================
// Nyelv és kategória segédfüggvények
// ============================================================================

QString MainWindow::getLanguageName(Language lang)
{
    switch (lang) {
    case Language::Hungarian: return "Magyar";
    case Language::English: return "English";
    case Language::German: return "Deutsch";
    case Language::Russian: return "Русский";
    default: return "Ismeretlen";
    }
}

QString MainWindow::getLanguageFlag(Language lang)
{
    switch (lang) {
    case Language::Hungarian: return "🇭🇺";
    case Language::English: return "🇬🇧";
    case Language::German: return "🇩🇪";
    case Language::Russian: return "🇷🇺";
    default: return "🌍";
    }
}

QString MainWindow::getCategoryName(Category cat)
{
    switch (cat) {
    case Category::Vocabulary: return "Szókincs";
    case Category::Grammar: return "Nyelvtan";
    case Category::Sentences: return "Mondatok";
    case Category::Listening: return "Hallás utáni";
    default: return "Ismeretlen";
    }
}

QString MainWindow::getCategoryIcon(Category cat)
{
    switch (cat) {
    case Category::Vocabulary: return "📖";
    case Category::Grammar: return "✏️";
    case Category::Sentences: return "💬";
    case Category::Listening: return "🎧";
    default: return "📚";
    }
}

QString MainWindow::getDifficultyName(Difficulty diff)
{
    switch (diff) {
    case Difficulty::Beginner: return "Kezdő";
    case Difficulty::Intermediate: return "Haladó";
    case Difficulty::Advanced: return "Profi";
    default: return "Ismeretlen";
    }
}

QString MainWindow::getDifficultyIcon(Difficulty diff)
{
    switch (diff) {
    case Difficulty::Beginner: return "🌱";
    case Difficulty::Intermediate: return "⭐";
    case Difficulty::Advanced: return "🏆";
    default: return "🎯";
    }
}

QString MainWindow::getMotivationalMessage(double accuracy)
{
    if (accuracy >= 90) {
        return "Fantasztikus! Te egy nyelvzseni vagy! 🌟";
    } else if (accuracy >= 80) {
        return "Nagyszerű munka! Így tovább! 👏";
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

void MainWindow::filterQuestions()
{
    // Kérdések szűrése a kiválasztott nyelv, kategória és nehézség alapján
    QVector<Question> allQuestions = questions;
    questions.clear();

    for (const Question& q : allQuestions) {
        if (q.language == selectedLanguage &&
            q.category == selectedCategory &&
            q.difficulty == selectedDifficulty) {
            questions.append(q);
        }
    }

    // Ha nincs elég kérdés, töltsük fel az összeset a nyelvből
    if (questions.size() < 5) {
        questions.clear();
        for (const Question& q : allQuestions) {
            if (q.language == selectedLanguage) {
                questions.append(q);
            }
        }
    }

    qDebug() << "Szűrt kérdések száma:" << questions.size();
}
