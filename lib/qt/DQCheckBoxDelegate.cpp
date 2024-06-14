#include "dpplib/qt/DQCheckBoxDelegate.h"

#ifdef QT_GUI_LIB

/**
 @code
    CheckBoxDelegate=new DQCheckBoxDelegate(this);
    CheckBoxDelegate->setColumnIndex(QUEUE_COLUMN_ENABLE);
    connect(CheckBoxDelegate,&DQCheckBoxDelegate::statusChanged,this,&MainWindow::OnCheckBoxDelegateChange);
    ui->TreeWidgetQueues->setItemDelegateForColumn(QUEUE_COLUMN_ENABLE,CheckBoxDelegate);
 @code
*/
#include <QApplication>
#include <QCheckBox>
#include <QMouseEvent>

namespace DTools
{
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

DQCheckBoxDelegate::DQCheckBoxDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
    columnIndex=-1;
}

void DQCheckBoxDelegate::setColumnIndex(int index)
{
    columnIndex=index;
}

void DQCheckBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,const QModelIndex& index)const
{
    if(index.column() == columnIndex){
//        auto newSize=option.decorationSize*2;
//        option.decorationSize.expandedTo(newSize);

        QStyleOptionButton checkBoxStyleOption;
        checkBoxStyleOption.rect=CheckBoxRect(option);

        // Checked role
        bool checked=index.model()->data(index,Qt::EditRole).toBool();
        checkBoxStyleOption.state|=checked? QStyle::State_On : QStyle::State_Off;

        // Enabled role
        bool enabled=true; //default
        if (index.model()->data(index,ROLE_ENABLE).isValid()) {
            // Custom role for enable/disable. If it is set, use it
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

bool DQCheckBoxDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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
QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override {
    //return option.fontMetrics.boundingRect(option.rect, Qt::TextWordWrap, index.data().toString()).size();
    auto newSize=option.decorationSize*2;
    return option.decorationSize.expandedTo(newSize);
}
*/
}

/**
CustomStyle::CustomStyle()
{

}
int CustomStyle::pixelMetric(PixelMetric which, const QStyleOption *option, const QWidget *widget) const
{
    switch (which) {
        case PM_IndicatorWidth:
            return (QProxyStyle::pixelMetric(which, option, widget)*2);
        case PM_IndicatorHeight:
            return (QProxyStyle::pixelMetric(which, option, widget)*2);
        default:
            return QProxyStyle::pixelMetric(which, option, widget);
    }
}

void CustomStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                QPainter *painter, const QWidget *widget) const
{
    if (element == PE_IndicatorSpinUp || element == PE_IndicatorSpinDown) {
        QPolygon points(3);
        int x = option->rect.x();
        int y = option->rect.y();
        int w = option->rect.width() / 2;
        int h = option->rect.height() / 2;
        x += (option->rect.width() - w) / 2;
        y += (option->rect.height() - h) / 2;

        if (element == PE_IndicatorSpinUp) {
            points[0] = QPoint(x, y + h);
            points[1] = QPoint(x + w, y + h);
            points[2] = QPoint(x + w / 2, y);
        } else { // PE_SpinBoxDown
            points[0] = QPoint(x, y);
            points[1] = QPoint(x + w, y);
            points[2] = QPoint(x + w / 2, y + h);
        }

        if (option->state & State_Enabled) {
            painter->setPen(option->palette.mid().color());
            painter->setBrush(option->palette.buttonText());
        } else {
            painter->setPen(option->palette.buttonText().color());
            painter->setBrush(option->palette.mid());
        }
        painter->drawPolygon(points);
    } else {
    QProxyStyle::drawPrimitive(element, option, painter, widget);
    }
}
*/

/*
QWidget *DQCheckBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    QCheckBox *editor = new QCheckBox(parent);
editor->resize( option.rect.width(), option.rect.height() )
    return editor;
}

void DQCheckBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    //set if checked or not
    QCheckBox *cb = qobject_cast<QCheckBox *>(editor);
    cb->setChecked(index.data().toBool());
}

void DQCheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QCheckBox *cb = static_cast<QCheckBox *>(editor);
    int value = (cb->checkState()==Qt::Checked) ? 1 : 0;
    model->setData(index, value, Qt::EditRole);
}
*/

#endif
