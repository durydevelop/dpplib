#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class DQLabel : public QLabel {
    Q_OBJECT 

    public:
        explicit DQLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(parent,f) {};
        explicit DQLabel(const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(text,parent,f) {};
        ~DQLabel(){};

    signals:
        void OnDblClick();

    protected:
        void mouseDoubleClickEvent(QMouseEvent* event) {
            emit OnDblClick();
        };

};

#endif
