#include <libdpp/qt/DQWidget.h>

#ifdef QT_GUI_LIB

namespace DTools
{
namespace DWidget
{
    // Removes the contents of the given layout row.
    void QGridLayoutUtil::RemoveRow(QGridLayout *layout, int row, bool deleteWidgets) {
        Remove(layout, row, -1, deleteWidgets);
        layout->setRowMinimumHeight(row, 0);
        layout->setRowStretch(row, 0);
    }

    // Removes the contents of the given layout column.
    void QGridLayoutUtil::RemoveColumn(QGridLayout *layout, int column, bool deleteWidgets) {
        Remove(layout, -1, column, deleteWidgets);
        layout->setColumnMinimumWidth(column, 0);
        layout->setColumnStretch(column, 0);
    }

    // Removes the contents of the given layout cell.
    void QGridLayoutUtil::RemoveCell(QGridLayout *layout, int row, int column, bool deleteWidgets) {
        Remove(layout, row, column, deleteWidgets);
    }

    // Removes all
    void QGridLayoutUtil::RemoveAll(QGridLayout *layout, bool deleteWidgets) {
        Remove(layout, -1, -1, deleteWidgets);
    }

    // Removes all layout items which span the given row and column.
    void QGridLayoutUtil::Remove(QGridLayout *layout, int row, int column, bool deleteWidgets) {
        // We avoid usage of QGridLayout::itemAtPosition() here to improve performance.
        for (int i = layout->count() - 1; i >= 0; i--) {
            int r, c, rs, cs;
            layout->getItemPosition(i, &r, &c, &rs, &cs);
            if ((row == -1 || (r <= row && r + rs > row)) &&
                (column == -1 || (c <= column && c + cs > column))) {
                // This layout item is subject to deletion.
                QLayoutItem *item = layout->takeAt(i);
                if (deleteWidgets) {
                    DeleteChildWidgets(item);
                }
                delete item;
            }
        }
    }

    // Deletes all child widgets of the given layout item.
    void QGridLayoutUtil::DeleteChildWidgets(QLayoutItem *item) {
        QLayout *layout = item->layout();
        if (layout) {
            // Process all child items recursively.
            int itemCount = layout->count();
            for (int i = 0; i < itemCount; i++) {
                DeleteChildWidgets(layout->itemAt(i));
            }
        }
        delete item->widget();
    }

    /**
     * @brief Get index of tab with name TabName in QTabWidget.
     * @param TabName   ->  Tab name to find.
     * @param TabWidget ->  Pointer to QTabWidget.
     * @return Index (0 based) of tab with TabName or -1 if it is not found.
     */
    int QTabWidgetUtil::IndexOf(QString TabName, QTabWidget *TabWidget) {
        // Cerco il gruppo code corrente nel TabWidgetQueues
        for (int ixT=0; ixT<TabWidget->count(); ixT++) {
            if (TabWidget->tabText(ixT) == QString(TabName)) {
                return(ixT);
            }
        }
        return(-1);
    }
}
}

#endif
