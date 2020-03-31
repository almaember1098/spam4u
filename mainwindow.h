#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QString>
#include <SmtpMime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_setupMailAccountButton_clicked();

    void on_beginSpammingButton_clicked();

private:
    Ui::MainWindow *ui;
    SmtpClient *smtp = nullptr;

    bool smtpLogin();
    void problemWithInput(QString problematicField);
    void spamPeople(QStringList victims, QString message, int timesForEachVictim);
};
#endif // MAINWINDOW_H
