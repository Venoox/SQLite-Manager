#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QSql>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_pushButton_clicked() {
  ui->tableWidget->clear();
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
  db.close();
  QString filename = ui->lineEdit->text();
  QFile file(filename);
  if (!file.exists()) {
    ui->label->setText("Status: File Not Found!");
    return;
  }
  db.setDatabaseName(filename);
  if (!db.open()) {
    ui->label->setText("Status: " + db.lastError().text());
    return;
  } else {
    ui->label->setText("Status: Connected");
  }
  QSqlQuery query(db);

  /*query.exec("CREATE TABLE database ( name varchar(20), surname varchar(20),
  age int);"); query.prepare("INSERT INTO database (name, surname, age) "
                    "VALUES (:name, :surname, :age)");
  query.bindValue(":name", "Tomaz");
  query.bindValue(":surname", "Cuk");
  query.bindValue(":age", 17);
  query.exec();*/
  QString table = ui->lineEdit_2->text();

  // Counts and sets row count
  query.exec("SELECT COUNT(EmployeeId) from " + table);
  query.next();
  int rows = query.value(0).toInt();
  ui->tableWidget->setRowCount(rows);

  // Sets column names
  query.exec("PRAGMA table_info(" + table + ");");
  for (int i = 0; i < rows + 1; i++) {
    query.next();
    QString tmp = query.value(1).toString();
    ui->tableWidget->setHorizontalHeaderItem(i, new QTableWidgetItem(tmp));
  }

  // Sets column count
  query.exec("SELECT * FROM " + table);
  int columns = query.record().count();
  ui->tableWidget->setColumnCount(columns);

  // Fills up the table with data from database
  for (int i = 0; query.next(); i++) {
    for (int j = 0; j < columns; j++) {
      ui->tableWidget->setItem(i, j,
                               new QTableWidgetItem(query.value(j).toString()));
    }
  }
}

void MainWindow::on_pushButton_2_clicked() {
  QString filename =
      QFileDialog::getOpenFileName(this, tr("Open File"), "C:\\",
                                   tr("SQLite database files (*.db *.sqlite "
                                      "*.sqlite3 *.db3);;All files (*.*)"));
  ui->lineEdit->setText(filename);
}

void MainWindow::on_pushButton_3_clicked() {
  QSqlDatabase db = QSqlDatabase::database();
  if (db.isOpen()) {
    QString sqlstring = ui->textEdit->toPlainText();
    QSqlQuery query(db);
    if (query.exec(sqlstring)) {
      ui->textEdit_2->setText(
          "Query executed successfully: " + query.lastQuery() + " (" +
          QString::number(query.numRowsAffected()) + " rows affected)");
    } else {
      ui->textEdit_2->setText("Error");
    }
  } else {
    ui->textEdit_2->setText("Database connection is not open!");
  }
}
