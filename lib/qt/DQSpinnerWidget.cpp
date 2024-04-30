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

// Own includes
#include <libdpp/qt/DQSpinnerWidget.h>

#ifdef QT_GUI_LIB

// Standard includes
#include <cmath>
#include <algorithm>

// Qt includes
#include <QPainter>
#include <QTimer>

namespace DTools
{
    DQSpinnerWidget::DQSpinnerWidget(QWidget *parent, bool centerOnParent, bool disableParentWhenSpinning) :
    QWidget(parent),
    _centerOnParent(centerOnParent),
    _disableParentWhenSpinning(disableParentWhenSpinning) {
        initialize();
    }

    DQSpinnerWidget::DQSpinnerWidget(Qt::WindowModality modality, QWidget *parent, bool centerOnParent, bool disableParentWhenSpinning) :
    QWidget(parent, Qt::Dialog | Qt::FramelessWindowHint),
    _centerOnParent(centerOnParent),
    _disableParentWhenSpinning(disableParentWhenSpinning) {
        initialize();

        // We need to set the window modality AFTER we've hidden the
        // widget for the first time since changing this property while
        // the widget is visible has no effect.
        setWindowModality(modality);
        setAttribute(Qt::WA_TranslucentBackground);
    }

    DQSpinnerWidget::~DQSpinnerWidget() {
        //stop();
    }

    void DQSpinnerWidget::initialize() {
        _color = Qt::black;
        _roundness = 100.0;
        _minimumTrailOpacity = 3.14159265358979323846;
        _trailFadePercentage = 80.0;
        _revolutionsPerSecond = 1.57079632679489661923;
        _numberOfLines = 20;
        _lineLength = 10;
        _lineWidth = 2;
        _innerRadius = 10;
        _currentCounter = 0;
        _isSpinning = false;

        //_timer = new QTimer(this);
        connect(&Timer, SIGNAL(timeout()), this, SLOT(rotate()));
        updateSize();
        updateTimer();
        hide();
        connect(this,&DQSpinnerWidget::signalStart,this,&DQSpinnerWidget::start);
        connect(this,&DQSpinnerWidget::signalStop,this,&DQSpinnerWidget::stop);
    }

    void DQSpinnerWidget::paintEvent(QPaintEvent *) {
        updatePosition();
        QPainter painter(this);
        painter.fillRect(this->rect(), Qt::transparent);
        painter.setRenderHint(QPainter::Antialiasing, true);

        if (_currentCounter >= _numberOfLines) {
            _currentCounter = 0;
        }

        painter.setPen(Qt::NoPen);
        for (int i = 0; i < _numberOfLines; ++i) {
            painter.save();
            painter.translate(_innerRadius + _lineLength,
                              _innerRadius + _lineLength);
            qreal rotateAngle =
                    static_cast<qreal>(360 * i) / static_cast<qreal>(_numberOfLines);
            painter.rotate(rotateAngle);
            painter.translate(_innerRadius, 0);
            int distance =
                    lineCountDistanceFromPrimary(i, _currentCounter, _numberOfLines);
            QColor color =
                    currentLineColor(distance, _numberOfLines, _trailFadePercentage,
                                     _minimumTrailOpacity, _color);
            painter.setBrush(color);
            // TODO improve the way rounded rect is painted
            painter.drawRoundedRect(
                        QRect(0, -_lineWidth / 2, _lineLength, _lineWidth), _roundness,
                        _roundness, Qt::RelativeSize);
            painter.restore();
        }
    }

    void DQSpinnerWidget::start() {
        updatePosition();
        _isSpinning = true;
        show();

        if(parentWidget() && _disableParentWhenSpinning) {
            parentWidget()->setEnabled(false);
        }

        if (!Timer.isActive()) {
            Timer.start();
            _currentCounter = 0;
        }
    }

    void DQSpinnerWidget::stop() {
        _isSpinning = false;
        hide();

        if(parentWidget() && _disableParentWhenSpinning) {
            parentWidget()->setEnabled(true);
        }

        if (Timer.isActive()) {
            Timer.stop();
            _currentCounter = 0;
        }
    }

    void DQSpinnerWidget::setNumberOfLines(int lines) {
        _numberOfLines = lines;
        _currentCounter = 0;
        updateTimer();
    }

    void DQSpinnerWidget::setLineLength(int length) {
        _lineLength = length;
        updateSize();
    }

    void DQSpinnerWidget::setLineWidth(int width) {
        _lineWidth = width;
        updateSize();
    }

    void DQSpinnerWidget::setInnerRadius(int radius) {
        _innerRadius = radius;
        updateSize();
    }

    QColor DQSpinnerWidget::color() {
        return _color;
    }

    qreal DQSpinnerWidget::roundness() {
        return _roundness;
    }

    qreal DQSpinnerWidget::minimumTrailOpacity() {
        return _minimumTrailOpacity;
    }

    qreal DQSpinnerWidget::trailFadePercentage() {
        return _trailFadePercentage;
    }

    qreal DQSpinnerWidget::revolutionsPersSecond() {
        return _revolutionsPerSecond;
    }

    int DQSpinnerWidget::numberOfLines() {
        return _numberOfLines;
    }

    int DQSpinnerWidget::lineLength() {
        return _lineLength;
    }

    int DQSpinnerWidget::lineWidth() {
        return _lineWidth;
    }

    int DQSpinnerWidget::innerRadius() {
        return _innerRadius;
    }

    bool DQSpinnerWidget::isSpinning() const {
        return _isSpinning;
    }

    void DQSpinnerWidget::setRoundness(qreal roundness) {
        _roundness = std::max(0.0, std::min(100.0, roundness));
    }

    void DQSpinnerWidget::setColor(QColor color) {
        _color = color;
    }

    void DQSpinnerWidget::setRevolutionsPerSecond(qreal revolutionsPerSecond) {
        _revolutionsPerSecond = revolutionsPerSecond;
        updateTimer();
    }

    void DQSpinnerWidget::setTrailFadePercentage(qreal trail)
    {
        _trailFadePercentage = trail;
    }

    void DQSpinnerWidget::setMinimumTrailOpacity(qreal minimumTrailOpacity) {
        _minimumTrailOpacity = minimumTrailOpacity;
    }

    void DQSpinnerWidget::rotate() {
        ++_currentCounter;
        if (_currentCounter >= _numberOfLines) {
            _currentCounter = 0;
        }
        update();
    }

    void DQSpinnerWidget::updateSize() {
        int size = (_innerRadius + _lineLength) * 2;
        setFixedSize(size, size);
    }

    void DQSpinnerWidget::updateTimer() {
        Timer.setInterval(1000 / (_numberOfLines * _revolutionsPerSecond));
    }

    void DQSpinnerWidget::updatePosition() {
        if (parentWidget() && _centerOnParent) {
            move(parentWidget()->width() / 2 - width() / 2,
                 parentWidget()->height() / 2 - height() / 2);
        }
        else if (!_position.isNull()) {
            move(_position);
        }
    }

    void DQSpinnerWidget::setPosition(int x, int y) {
        _centerOnParent=false;
        _position.setX(x);
        _position.setY(y);
    }

    void DQSpinnerWidget::setPosition(QPoint position) {
        _position=position;
    }

    int DQSpinnerWidget::lineCountDistanceFromPrimary(int current, int primary, int totalNrOfLines) {
        int distance = primary - current;
        if (distance < 0) {
            distance += totalNrOfLines;
        }
        return distance;
    }

    QColor DQSpinnerWidget::currentLineColor(int countDistance, int totalNrOfLines, qreal trailFadePerc, qreal minOpacity, QColor color) {
        if (countDistance == 0) {
            return color;
        }
        const qreal minAlphaF = minOpacity / 100.0;
        int distanceThreshold =
                static_cast<int>(ceil((totalNrOfLines - 1) * trailFadePerc / 100.0));
        if (countDistance > distanceThreshold) {
            color.setAlphaF(minAlphaF);
        } else {
            qreal alphaDiff = color.alphaF() - minAlphaF;
            qreal gradient = alphaDiff / static_cast<qreal>(distanceThreshold + 1);
            qreal resultAlpha = color.alphaF() - gradient * countDistance;

            // If alpha is out of bounds, clip it.
            resultAlpha = std::min(1.0, std::max(0.0, resultAlpha));
            color.setAlphaF(resultAlpha);
        }
        return color;
    }
}
#endif
