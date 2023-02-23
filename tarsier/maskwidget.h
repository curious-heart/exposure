#ifndef MASKWIDGET_H
#define MASKWIDGET_H

#include <QWidget>

namespace Ui {
class MaskWidget;
}

class MaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MaskWidget(QWidget *parent = nullptr);
    ~MaskWidget();

private:
    Ui::MaskWidget *ui;
};

#endif // MASKWIDGET_H
