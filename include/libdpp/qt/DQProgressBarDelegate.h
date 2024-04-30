#ifndef DQProgressBarDelegateH
#define DQProgressBarDelegateH

#ifdef QT_GUI_LIB

#include <QItemDelegate>

namespace DTools
{
class DQProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
    public:
        DQProgressBarDelegate(QObject *parent = nullptr);
        void setColumnIndex(int index);
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    private:
        int columnIndex;
};
}

#endif
#endif
