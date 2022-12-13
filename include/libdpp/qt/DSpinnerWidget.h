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

#ifndef DSpinnerWidgetH
#define DSpinnerWidgetH
#ifdef QT_GUI_LIB

// Qt includes
#include <QWidget>
#include <QTimer>
#include <QColor>

namespace DTools
{
class DSpinnerWidget : public QWidget {
    Q_OBJECT
    public:
        /**
        * @brief Constructor for "standard" widget behaviour - use this
        * constructor if you wish to, e.g. embed your widget in another.
        * @param parent
        * @param centerOnParent
        * @param disableParentWhenSpinning
        */
        DSpinnerWidget(QWidget *parent = 0, bool centerOnParent = true, bool disableParentWhenSpinning = true);

        /**
        * @brief Constructor - use this constructor to automatically create a modal
        * ("blocking") spinner on top of the calling widget/window.  If a valid
        * parent widget is provided, "centreOnParent" will ensure that
        * QtWaitingSpinner automatically centres itself on it, if not,
        * "centreOnParent" is ignored.
        * @param modality
        * @param parent
        * @param centerOnParent
        * @param disableParentWhenSpinning
        */
        DSpinnerWidget(Qt::WindowModality modality, QWidget *parent = 0, bool centerOnParent = true, bool disableParentWhenSpinning = true);

        ~DSpinnerWidget();

    signals:
        void signalStart();
        void signalStop();

    public slots:
        void start();
        void stop();

    public:
        void setColor(QColor color);
        void setRoundness(qreal roundness);
        void setMinimumTrailOpacity(qreal minimumTrailOpacity);
        void setTrailFadePercentage(qreal trail);
        void setRevolutionsPerSecond(qreal revolutionsPerSecond);
        void setNumberOfLines(int lines);
        void setLineLength(int length);
        void setLineWidth(int width);
        void setInnerRadius(int radius);
        void setText(QString text);
        void setPosition(int x, int y);
        void setPosition(QPoint position);

        QColor color();
        qreal roundness();
        qreal minimumTrailOpacity();
        qreal trailFadePercentage();
        qreal revolutionsPersSecond();
        int numberOfLines();
        int lineLength();
        int lineWidth();
        int innerRadius();

        bool isSpinning() const;

    private slots:
        void rotate();

    protected:
        void paintEvent(QPaintEvent *paintEvent);

    private:
        static int lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines);
        static QColor currentLineColor(int distance, int totalNrOfLines, qreal trailFadePerc, qreal minOpacity, QColor color);

        DSpinnerWidget(const DSpinnerWidget&);
        DSpinnerWidget& operator=(const DSpinnerWidget&);

        void initialize();
        void updateSize();
        void updateTimer();
        void updatePosition();

        QColor  _color;
        qreal   _roundness; // 0..100
        qreal   _minimumTrailOpacity;
        qreal   _trailFadePercentage;
        qreal   _revolutionsPerSecond;
        int     _numberOfLines;
        int     _lineLength;
        int     _lineWidth;
        int     _innerRadius;
        //QTimer *_timer;
        QTimer  Timer;
        bool    _centerOnParent;
        QPoint  _position;
        bool    _disableParentWhenSpinning;
        int     _currentCounter;
        bool    _isSpinning;
};
}

#endif
#endif
