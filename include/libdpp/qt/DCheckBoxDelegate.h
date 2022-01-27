#ifndef DCheckBoxDelegateH
#define DCheckBoxDelegateH

#ifdef QT_GUI_LIB

#include <QStyledItemDelegate>

class DCheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    public:
        enum DRoles {ROLE_ENABLE=Qt::UserRole, ROLE_VISIBLE};
        DCheckBoxDelegate(QObject *parent = nullptr);
        void setColumnIndex(int Index);

    signals:
        void statusChanged(const QModelIndex&, bool checked);

    public slots:

    protected:
        void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
        bool editorEvent(QEvent *event,QAbstractItemModel *model,const QStyleOptionViewItem &option,const QModelIndex &index);
/*
        QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        void setEditorData(QWidget *editor, const QModelIndex &index) const override;
        void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
*/

    private:
        int columnIndex;
};
#endif
#endif
