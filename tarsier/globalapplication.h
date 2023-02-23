#ifndef GLOBALAPPLICATION_H
#define GLOBALAPPLICATION_H

#include <QApplication>

class GlobalApplication : public QApplication
{
    Q_OBJECT
public:
    GlobalApplication(int&argc,char **argv);
    ~GlobalApplication();
    bool notify(QObject*, QEvent *);
    void setWindowInstance(QWidget *wnd);
private:
     QWidget *widget;
signals:
     void keyOrMouseEventOccurred();
};

#endif // GLOBALAPPLICATION_H
