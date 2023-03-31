#ifndef EXITSYSTEM_H
#define EXITSYSTEM_H

#include <QDialog>

namespace Ui {
class ExitSystem;
}

class ExitSystem : public QDialog
{
    Q_OBJECT

public:
    explicit ExitSystem(QWidget *parent = nullptr);
    ~ExitSystem();

signals:
    /**
     * @brief maskWidgetClosed 关闭
     */
    void maskWidgetClosed();
    void shutdown_system();

private slots:
    void on_exitOk_clicked();

    void on_exitCancel_clicked();

    void on_exitShutdown_clicked();

private:
    Ui::ExitSystem *ui;
    QWidget *p;
};

#endif // EXITSYSTEM_H
