#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    setWindowTitle("Singly Linked List Visualizer");
    resize(500, 400);

    // Main layout
    layout = new QVBoxLayout();

    // Input field for user values
    inputField = new QLineEdit();
    inputField->setPlaceholderText("Enter a number");
    inputField->setFixedHeight(40);
    inputField->setStyleSheet("font-size: 18px; padding: 5px; border-radius: 8px;");

    // Buttons
    insertButton = new QPushButton("Insert");
    deleteButton = new QPushButton("Delete");
    searchButton = new QPushButton("Search");

    // Styling buttons
    QString buttonStyle = "QPushButton {"
                          "background-color: #007BFF; color: white; font-size: 16px; padding: 10px;"
                          "border-radius: 8px; border: none;}"
                          "QPushButton:hover { background-color: #0056b3; }";

    insertButton->setStyleSheet(buttonStyle);
    deleteButton->setStyleSheet(buttonStyle);
    searchButton->setStyleSheet(buttonStyle);

    insertButton->setFixedSize(120, 50);
    deleteButton->setFixedSize(120, 50);
    searchButton->setFixedSize(120, 50);

    // Display Label
    displayLabel = new QLabel("List: ");
    displayLabel->setAlignment(Qt::AlignCenter);
    displayLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #333;");

    // Layout for buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(insertButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(searchButton);

    // Add widgets to main layout
    layout->addWidget(inputField);
    layout->addLayout(buttonLayout);
    layout->addWidget(displayLabel);

    centralWidget->setLayout(layout);
    centralWidget->setStyleSheet("background-color: #F8F9FA; padding: 20px;");

    // Connect button clicks
    connect(insertButton, &QPushButton::clicked, this, &MainWindow::onInsertClicked);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(searchButton, &QPushButton::clicked, this, &MainWindow::onSearchClicked);
}

void MainWindow::updateDisplay() {
    std::vector<int> values = linkedList.getListValues();
    QString displayText = "<b>List:</b> ";
    for (int v : values) {
        displayText += "<span style='color: #007BFF; font-size: 20px; font-weight: bold;'>" + 
                       QString::number(v) + "</span> ➝ ";
    }
    displayText += "<span style='color: #FF0000;'>NULL</span>";
    displayLabel->setText(displayText);
}
