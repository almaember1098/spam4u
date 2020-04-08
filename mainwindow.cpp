#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QDebug>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    smtp->quit();
    delete ui;
}


void MainWindow::on_setupMailAccountButton_clicked()
{
    if (smtp != nullptr)
    {
        smtp->quit();
        delete smtp;
    }

    QString serverAddress;
    int serverPort;
    QString serverData = ui->smtpServerEdit->text();

    // check for problems
    if (serverData.length() != 2)
    {
        problemWithInput("SMTP server address");
        return; // return before the problem could crash the program
    }

    // setup SMTP client
    serverAddress = serverData;
    serverPort = ui->portSpinBox->value();
    qDebug() << serverAddress << serverPort;
    smtp = new SmtpClient(serverAddress, serverPort, SmtpClient::TlsConnection);

    smtp->setUser(ui->usernameEdit->text());
    smtp->setPassword(ui->passwordEdit->text());
}

bool MainWindow::smtpLogin()
{
    smtp->connectToHost();
    smtp->login();
    return true;
}

void MainWindow::problemWithInput(QString problematicField)
{
    QMessageBox msgbox(this);
    msgbox.setWindowTitle("Error");
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setText("There is a problem with the following input field:");
    msgbox.setDetailedText(problematicField);
    msgbox.exec();
}

void MainWindow::spamPeople(QStringList victims, QString message, int timesForEachVictim)
{
    int messagesToSend = victims.length() * timesForEachVictim;
    ui->progressBar->setMaximum(messagesToSend);
    smtpLogin();
    for (int i = 0; i<timesForEachVictim; ++i)
    {
        foreach (const QString& victim, victims) {
            QCoreApplication::processEvents();

            // the eMail we will send
            MimeMessage msg;
            msg.setSender(new EmailAddress(smtp->getUser()));
            msg.addRecipient(new EmailAddress(victim));
            msg.setSubject(message.split('\n')[0]);

            // the message itself
            MimeText text;
            text.setText(message);
            msg.addPart(&text); // add the text to the rest of the eMail

            // send the eMail
            smtp->sendMail(msg);

            ui->progressBar->setValue(ui->progressBar->value() + 1);
            QCoreApplication::processEvents();
        }
    }

    QMessageBox msgbox(
                QMessageBox::Information,
                "Ready",
                "Spamming ready!");
    msgbox.exec();
}

QStringList MainWindow::listFromListWidget(QListWidget *widget)
{
    QStringList resultList;
    for(int i=0; i<widget->count(); ++i)
    {
        resultList.append(widget->item(i)->text());
    }
}

void MainWindow::on_beginSpammingButton_clicked()
{
    QStringList victims = listFromListWidget(ui->victimListWidget);
    int timesForEachVictim = ui->sendTimeSpinBox->value();
    QString message = ui->messageEdit->toPlainText();

    spamPeople(victims, message, timesForEachVictim);
}

void MainWindow::on_addVictimButton_clicked()
{
    bool ok;
    QString newVictim = QInputDialog::getText(this, tr("Add new victim"),
                                              tr("Victim's email address:"),
                                              QLineEdit::Normal,
                                              QString(""), &ok);
    if(ok && !newVictim.isEmpty())
        ui->victimListWidget->addItem(newVictim);
}

void MainWindow::on_removeSelectedVictimButton_clicked()
{
    QListWidgetItem *itemToRemove = ui->victimListWidget->takeItem(
                ui->victimListWidget->currentRow());
    delete itemToRemove;
}

void MainWindow::on_editSelectedVictimButton_clicked()
{
    bool ok;
    QString newVictim = QInputDialog::getText(this, tr("Edit victim"),
                                              tr("Victim's email address:"),
                                              QLineEdit::Normal,
                                              ui->victimListWidget->currentItem()->text(), &ok);
    if(ok && !newVictim.isEmpty())
        ui->victimListWidget->currentItem()->setText(newVictim);
}
