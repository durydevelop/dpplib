#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

/**
 * @brief The DQLabel class is an overload of the QLabel that implements also double click event.
 */
class DQLabel : public QLabel {
    Q_OBJECT 

    public:
        explicit DQLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(parent,f) {};
        explicit DQLabel(const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags()) : QLabel(text,parent,f) {};
        ~DQLabel(){};

    signals:
        void OnDblClick(QMouseEvent* event);

    protected:
        void mouseDoubleClickEvent(QMouseEvent* event) {
            emit OnDblClick(event);
        };

};

#endif
