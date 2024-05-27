#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#ifdef QT_GUI_LIB

#include <QLabel>
#include <QWidget>

namespace DTools
{
/**
 * @brief The DQLabel class is an overload of the QLabel that implements also double click event.
 */
class DQLabel : public QLabel {
    Q_OBJECT 

    public:
        explicit DQLabel(QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());
        explicit DQLabel(const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags());

    signals:
        void OnDblClick(QMouseEvent* event);

    protected:
        void mouseDoubleClickEvent(QMouseEvent* event);

};
}

#endif
#endif
