#include "globalapplication.h"
#include <QDebug>

GlobalApplication::GlobalApplication(int &argc, char **argv):
    QApplication(argc,argv){

}

GlobalApplication::~GlobalApplication(){

}

bool GlobalApplication::notify(QObject *obj, QEvent *e){
    //const QMetaObject* objMeta = obj->metaObject();
    //QString clName = objMeta->className();
    if(e->type() == QEvent::KeyRelease){
        //qDebug()<<(clName);
        emit keyOrMouseEventOccurred();
    }else if( e->type() == QEvent::MouseButtonRelease){
        //qDebug()<<"cad----";
        emit keyOrMouseEventOccurred();
    }
    return QApplication::notify(obj,e);
}

void GlobalApplication::setWindowInstance(QWidget *wnd){
    widget = wnd;
}
