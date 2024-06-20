#include "dpplib/qt/DQProgressBarDelegate.h"

#ifdef QT_GUI_LIB

#include <QApplication>

namespace DTools
{
    DQProgressBarDelegate::DQProgressBarDelegate(QObject *parent) : QItemDelegate(parent) {
        columnIndex=-1;
    }

    void DQProgressBarDelegate::setColumnIndex(int index) {
        columnIndex=index;
    }

    void DQProgressBarDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
        if (index.column() != columnIndex) {
            QItemDelegate::paint(painter, option, index);
            return;
        }

        // Set up a QStyleOptionProgressBar to precisely mimic the
        // environment of a progress bar.
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.bottomToTop=false;
        progressBarOption.state=QStyle::State_Enabled;
        //progressBarOption.direction=QApplication::layoutDirection();
        progressBarOption.rect=option.rect;
        progressBarOption.fontMetrics=QFontMetrics(QApplication::font());
        progressBarOption.minimum=0;
        progressBarOption.maximum=100;
        progressBarOption.textAlignment=Qt::AlignCenter;
        progressBarOption.state = QStyle::StateFlag::State_Horizontal;
        progressBarOption.textVisible=true;

        // Set the progress and text values of the style option.
        int newValue=index.model()->data(index,Qt::DisplayRole).toInt();
        progressBarOption.progress = newValue < 0 ? progressBarOption.progress : newValue;
        progressBarOption.text = QString::asprintf("%d%%", newValue);

        // Draw the progress bar onto the view.
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,&progressBarOption,painter);
    }
}

#endif
