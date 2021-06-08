#ifndef DCheckBoxDelegateH
#define DCheckBoxDelegateH

#ifdef QT_GUI_LIB

#include <QStyledItemDelegate>

class DCheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        DCheckBoxDelegate(QObject *parent = nullptr);
        void setColumnIndex(int Index);

    protected:
        void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
        bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);

    private:
        int columnIndex;
};
#endif
#endif
