#include "libdpp/qt/DCheckBoxDelegate.h"

#ifdef QT_GUI_LIB

/**
 @code
    CheckBoxDelegate=new DCheckBoxDelegate(this);
    CheckBoxDelegate->setColumnIndex(QUEUE_COLUMN_ENABLE);
    connect(CheckBoxDelegate,&DCheckBoxDelegate::statusChanged,this,&MainWindow::OnCheckBoxDelegateChange);
    ui->TreeWidgetQueues->setItemDelegateForColumn(QUEUE_COLUMN_ENABLE,CheckBoxDelegate);
 @code
*/
#include <QApplication>
#include <QCheckBox>
#include <QMouseEvent>

/**
 * @brief CheckBoxRect
 * @param viewItemStyleOptions
 * @return
 */
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
        QStyleOptionButton checkBoxStyleOption;
        checkBoxStyleOption.rect=CheckBoxRect(option);

        // Checked role
        bool checked=index.model()->data(index,Qt::EditRole).toBool();
        checkBoxStyleOption.state|=checked? QStyle::State_On : QStyle::State_Off;

        // Enabled role
        bool enabled=true; //default
        if (index.model()->data(index,ROLE_ENABLE).isValid()) {
            // Custom role for enable/disable. If is set use it
            enabled=index.model()->data(index,ROLE_ENABLE).toBool();
        }

        if (enabled) {
            checkBoxStyleOption.state|=QStyle::State_Enabled;
        }

        // Visible role
        bool visible=true; //default
        if (index.model()->data(index,ROLE_VISIBLE).isValid()) {
            // Custom role for enable/disable. If is set use it
            visible=index.model()->data(index,ROLE_VISIBLE).toBool();
        }

        // BackgroundBrush color
        if (index.model()->data(index, Qt::BackgroundRole).isValid()) {
            // Role set
            QColor color = index.model()->data(index, Qt::BackgroundRole).value<QColor>();
            QStyleOptionViewItem opt(option);
            opt.backgroundBrush = QBrush(QColor(color));
            // Draw item background
            option.widget->style()->drawPrimitive(QStyle::PE_PanelItemViewItem,&opt,painter);
        }

        // Draw control
        if (visible) {
            QApplication::style()->drawControl(QStyle::CE_CheckBox,&checkBoxStyleOption,painter);
        }
    }
    else {
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
        emit statusChanged(index,!checked);
        return(model->setData(index, data, Qt::EditRole));
    }
    else {
        return(QStyledItemDelegate::editorEvent(event, model, option, index));
    }
}

/*
QWidget *DCheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QCheckBox *editor = new QCheckBox(parent);
    return editor;
}

void DCheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //set if checked or not
    QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
    cb->setChecked(index.data().toBool());
}

void DCheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *cb = static_cast<QCheckBox *>(editor);
    int value = (cb->checkState()==Qt::Checked) ? 1 : 0;
    model->setData(index, value, Qt::EditRole);
}
*/

#endif
