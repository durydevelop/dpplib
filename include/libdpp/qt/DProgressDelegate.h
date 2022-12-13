#ifndef DProgressDelegateH
#define DProgressDelegateH

#ifdef QT_GUI_LIB

#include <QItemDelegate>

namespace DTools
{
class DProgressDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
        DProgressDelegate(QObject *parent = nullptr);
        void setColumnIndex(int index);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    private:
        int columnIndex;
};
}

#endif
#endif
