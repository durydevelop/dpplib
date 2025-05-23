/* Original Work Copyright (c) 2012-2014 Alexander Turkin
   Modified 2014 by William Hallatt
   Modified 2015 by Jacob Dawid
   Modified 2021 by Fabio Durigon

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef DQSpinnerWidgetDelegateH
#define DQSpinnerWidgetDelegateH

#ifdef QT_GUI_LIB

// Qt includes
#include <QItemDelegate>
#include "DQSpinnerWidget.h"

namespace DTools
{
    class DQSpinnerWidgetDelegate : public QItemDelegate {
        Q_OBJECT
        public:
            DQSpinnerWidgetDelegate(QObject *parent = nullptr);
            void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
        private:
            DQSpinnerWidget *SpinnerDelegate;

    };
}

#endif
#endif
