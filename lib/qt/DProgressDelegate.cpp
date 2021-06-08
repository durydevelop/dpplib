#include "libdpp/qt/DProgressDelegate.h"

#ifdef QT_GUI_LIB

#include <QApplication>

DProgressDelegate::DProgressDelegate(QObject *parent) : QItemDelegate(parent)
{
    columnIndex=-1;
}

void DProgressDelegate::setColumnIndex(int index)
{
    columnIndex=index;
}

void DProgressDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.column() != columnIndex) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    // Set up a QStyleOptionProgressBar to precisely mimic the
    // environment of a progress bar.
    QStyleOptionProgressBar progressBarOption;
    progressBarOption.state=QStyle::State_Enabled;
    progressBarOption.direction=QApplication::layoutDirection();
    progressBarOption.rect=option.rect;
    progressBarOption.fontMetrics=QFontMetrics(QApplication::font());
    progressBarOption.minimum=0;
    progressBarOption.maximum=100;
    progressBarOption.textAlignment=Qt::AlignCenter;
    progressBarOption.textVisible=true;

    // Set the progress and text values of the style option.
    /* TODO
    int progress = qobject_cast<MainWindow *>(parent())->clientForRow(index.row())->progress();
    progressBarOption.progress = progress < 0 ? 0 : progress;
    progressBarOption.text = QString::asprintf("%d%%", progressBarOption.progress);
    */

    // Draw the progress bar onto the view.
    QApplication::style()->drawControl(QStyle::CE_ProgressBar,&progressBarOption,painter);
}
#endif
