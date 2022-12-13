#include "libdpp/qt/DSplashScreen.h"

#ifdef QT_GUI_LIB

#include <QStyleOptionProgressBar>

namespace DTools
{
DSplashScreen::DSplashScreen(QPixmap splashPixmap, bool showProgress) : DSplashScreen(nullptr,splashPixmap,showProgress)
{
}

#if QT_VERSION < QT_VERSION_CHECK(5,15,0)
DSplashScreen::DSplashScreen(QScreen *screen, QPixmap splashPixmap, bool showProgress) : QSplashScreen(splashPixmap)
#else
DSplashScreen::DSplashScreen(QScreen *screen, QPixmap splashPixmap, bool showProgress) : QSplashScreen(screen,splashPixmap)
#endif
{
    // Pixmap
    this->setPixmap(splashPixmap);
    this->setCursor(Qt::BusyCursor);

    // ProgressBar
    ShowProgress=showProgress;
    ProgressValue=0;
    ProgressPos=PROGRESS_POS_BOTTOM;
    connect(this,&DSplashScreen::SetProgress,this,&DSplashScreen::OnSetProgress);
    connect(this,&DSplashScreen::SetTitle,this,&DSplashScreen::OnSetTitle);
}

void DSplashScreen::SetProgressPos(DSplashScreen::DProgressPos progressPos, QRect customProgressRect)
{
    ProgressPos=progressPos;
    if (progressPos == PROGRESS_POS_CUSTOM) {
        ProgressRect=customProgressRect;
    }
    update();
}

void DSplashScreen::drawContents(QPainter *painter)
{
    QSplashScreen::drawContents(painter);

    if (ShowProgress) {
        QStyleOptionProgressBar progressBarOption;
        progressBarOption.state=QStyle::State_Enabled;
        progressBarOption.direction=QApplication::layoutDirection();
        progressBarOption.fontMetrics=QFontMetrics(QApplication::font());
        progressBarOption.minimum=0;
        progressBarOption.maximum=100;
        progressBarOption.textAlignment=Qt::AlignCenter;
        progressBarOption.textVisible=true;

        // Set the progress and text values of the style option.
        progressBarOption.progress=ProgressValue;
        progressBarOption.text=ProgressText;

        int ProgressHeight=progressBarOption.fontMetrics.height()+2;
        switch (ProgressPos) {
            case PROGRESS_POS_TOP:
                ProgressRect.setRect(0,0,width(),ProgressHeight);
                break;
            case PROGRESS_POS_BOTTOM:
                ProgressRect.setRect(0,height()-ProgressHeight-1,width(),ProgressHeight);
                break;
            case PROGRESS_POS_CENTER:
                ProgressRect.setRect(0,(height()/2)-(ProgressHeight/2),width(),ProgressHeight);
                break;
            case PROGRESS_POS_CUSTOM:
                break;
        }
        progressBarOption.rect=ProgressRect;

        // Draw the progress bar onto the view.
        QApplication::style()->drawControl(QStyle::CE_ProgressBar,&progressBarOption,painter);
        //style()->drawControl(QStyle::CE_ProgressBar, &pbstyle, painter, this);
    }
}

void DSplashScreen::OnSetProgress(size_t value, QString text) {
	ProgressValue = value;
	if (ProgressValue > 100) {
		ProgressValue = 100;
	}
	if (ProgressValue < 0) {
		ProgressValue = 0;
	}

	if (text.isEmpty()) {
		ProgressText=QString::asprintf("%zu%%",ProgressValue);
	}
	else {
		ProgressText=text;
	}
	update();
}

void DSplashScreen::OnSetTitle(QString Title)
{
	// TODO: add QTextBox
	setWindowTitle(Title);
}
}

#endif
