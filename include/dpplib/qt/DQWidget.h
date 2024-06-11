#ifndef DQWidgetH
#define DQWidgetH

#ifdef QT_GUI_LIB

#include <QGridLayout>
#include <QTabWidget>

namespace DTools
{
namespace DWidget {
    /**
     * Utility class to remove the contents of a QGridLayout row, column or
     * cell. If the deleteWidgets parameter is true, then the widgets become
     * not only removed from the layout, but also deleted. Note that we won't
     * actually remove any row or column itself from the layout, as this isn't
     * possible. So the rowCount() and columnCount() will always stay the same,
     * but the contents of the row, column or cell will be removed.
     *
     * https://stackoverflow.com/questions/5395266/removing-widgets-from-qgridlayout
     */
    class QGridLayoutUtil {
        public:
            // Removes the contents of the given layout row.
            static void RemoveRow(QGridLayout *layout, int row, bool deleteWidgets = true);
            // Removes the contents of the given layout column.
            static void RemoveColumn(QGridLayout *layout, int column, bool deleteWidgets = true);
            // Removes the contents of the given layout cell.
            static void RemoveCell(QGridLayout *layout, int row, int column, bool deleteWidgets = true);
            // Removes all.
            static void RemoveAll(QGridLayout *layout, bool deleteWidgets = true);

        private:
            // Removes all layout items which span the given row and column.
            static void Remove(QGridLayout *layout, int row, int column, bool deleteWidgets);
            // Deletes all child widgets of the given layout item.
            static void DeleteChildWidgets(QLayoutItem *item);
    };

    class QTabWidgetUtil {
        public:
            static int IndexOf(QString TabName, QTabWidget *TabWidget);
    };
}
}
#endif
#endif
