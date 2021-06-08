#include "libdpp/qt/DCheckBoxDelegate.h"

#ifdef QT_GUI_LIB

#include <QApplication>
#include <QCheckBox>
#include <QMouseEvent>

static QRect CheckBoxRect(const QStyleOptionViewItem &viewItemStyleOptions)
{
    QStyleOptionButton checkBoxStyleOption;
    QRect checkBoxRect=QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,&checkBoxStyleOption);
    QPoint checkBoxPoint(viewItemStyleOptions.rect.x() + viewItemStyleOptions.rect.width() / 2 - checkBoxRect.width() / 2,
                         viewItemStyleOptions.rect.y() + viewItemStyleOptions.rect.height() / 2 - checkBoxRect.height() / 2);
    return(QRect(checkBoxPoint,checkBoxRect.size()));
}

DCheckBoxDelegate::DCheckBoxDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    columnIndex=-1;
}

void DCheckBoxDelegate::setColumnIndex(int index)
{
    columnIndex=index;
}

void DCheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    if(index.column() == columnIndex){
        bool checked=index.model()->data(index,Qt::DisplayRole).toBool();
        QStyleOptionButton checkBoxStyleOption;
        checkBoxStyleOption.state|=QStyle::State_Enabled;
        checkBoxStyleOption.state|=checked? QStyle::State_On : QStyle::State_Off;
        checkBoxStyleOption.rect=CheckBoxRect(option);
        QApplication::style()->drawControl(QStyle::CE_CheckBox,&checkBoxStyleOption,painter);
    }else{
        QStyledItemDelegate::paint(painter,option,index);
    }
}

bool DCheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if(index.column() == columnIndex) {
        if((event->type() == QEvent::MouseButtonRelease) || (event->type() == QEvent::MouseButtonDblClick)) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(mouseEvent->button() != Qt::LeftButton || !CheckBoxRect(option).contains(mouseEvent->pos())) {
                return true;
            }
            if(event->type() == QEvent::MouseButtonDblClick) {
                return true;
            }
        }
        else if(event->type() == QEvent::KeyPress) {
            if(static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space && static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select) {
                return false;
            }
        }
        else {
            return false;
        }

        bool checked=index.model()->data(index,Qt::DisplayRole).toBool();
        int data=checked ? 0 : 1;
        return(model->setData(index, data, Qt::EditRole));
    }
    else {
        return(QStyledItemDelegate::editorEvent(event, model, option, index));
    }
}
#endif
