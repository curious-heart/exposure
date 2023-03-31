#include "mainwindow.h"

#include <QApplication>
//#include "globalapplication.h"

#include "thirdpart/qtsingleapplication.h"
#include "logger.h"
#include "common_tool_func.h"

int main(int argc, char *argv[])
{
    int ret;
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    //qputenv("QT_DEBUG_PLUGINS", "1");

    if(QT_VERSION>=QT_VERSION_CHECK(5,6,0))
            QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    //GlobalApplication a(argc, argv);
    QtSingleApplication a(argc, argv);

    QThread th;
    start_log_thread(th);

    if(a.isRunning())
    {
        return !a.sendMessage("已经有一个实例在运行中");
    }

    MainWindow w;
    w.show();
    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    //w.connect( &a, &GlobalApplication::keyOrMouseEventOccurred, &w, &MainWindow::onKeyOrMouseEventOccurred);
    w.connect( &a, &QtSingleApplication::keyOrMouseEventOccurred, &w, &MainWindow::onKeyOrMouseEventOccurred);
    ret = a.exec();

    if(MainWindow::MAIN_WINDOWN_NORMAL_CLOSE_AND_SHUTDOWN_SYS == ret)
    {
        DIY_LOG(LOG_INFO, "Now exit the program.");

        QString exit_comment_str
                = QString("%1 正常退出，系统即将关闭。").arg(a.applicationName());
        QString s_c = shutdown_system(exit_comment_str);
        DIY_LOG(LOG_INFO, QString("The shutdown command line is: %1").arg(s_c));
    }

    end_log_thread(th);
    return ret;
}
