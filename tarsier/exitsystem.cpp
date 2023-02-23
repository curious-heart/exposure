#include "exitsystem.h"
#include "ui_exitsystem.h"
#include <iostream>

ExitSystem::ExitSystem(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitSystem)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(590, 302);
    ui->exitOk->setText(NULL);
    ui->exitShutdown->setText(NULL);
    ui->exitCancel->setText(NULL);
    p=parent;
}

ExitSystem::~ExitSystem()
{
    delete ui;
}

void ExitSystem::on_exitOk_clicked()
{
    this->close();
    p->close();
}

void ExitSystem::on_exitCancel_clicked()
{
    this->close();
    emit maskWidgetClosed();
}

void ExitSystem::on_exitShutdown_clicked()
{
    system("shutdown -s -t 00");
    this->close();
    p->close();
}
