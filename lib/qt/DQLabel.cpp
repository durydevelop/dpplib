#include "libdpp/qt/DQLabel.h"

#ifdef QT_GUI_LIB

namespace DTools
{
DQLabel::DQLabel(QWidget *parent, Qt::WindowFlags flags) : QLabel(parent,flags)
{

}

DQLabel::DQLabel(const QString &text, QWidget *parent, Qt::WindowFlags flags) : QLabel(text,parent,flags) {

}

void DQLabel::mouseDoubleClickEvent(QMouseEvent* event) {
    emit OnDblClick(event);
};
}

#endif
