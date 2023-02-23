#include "maskwidget.h"
#include "ui_maskwidget.h"

MaskWidget::MaskWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaskWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog|Qt::FramelessWindowHint);
    this->setFixedSize(1024, 600);
    this->setWindowOpacity(0.4);
    this->setStyleSheet("background-color: rgb(0, 0, 0);" );
}

MaskWidget::~MaskWidget()
{
    delete ui;
}
