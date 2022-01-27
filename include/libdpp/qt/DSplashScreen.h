#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QApplication>

class DSplashScreen : public QSplashScreen
{
    Q_OBJECT
	public:
		enum DProgressPos { PROGRESS_POS_TOP, PROGRESS_POS_BOTTOM, PROGRESS_POS_CENTER, PROGRESS_POS_CUSTOM };
		explicit DSplashScreen(QPixmap splashPixmap, bool showProgress);
		explicit DSplashScreen(QScreen *screen, QPixmap splashPixmap, bool showProgress);
		void SetProgressPos(DSplashScreen::DProgressPos progressPos, QRect customProgressRect);

	signals:
		void SetProgress(size_t value, QString text);
		void SetTitle(QString Title);

	public slots:
		void OnSetProgress(size_t value, QString text);
		void OnSetTitle(QString Title);

	protected:
		void drawContents(QPainter *painter);

	private:
		void Init (QPixmap pixmap, QRect progressRect);
		bool ShowProgress;
		size_t ProgressValue;
		QRect ProgressRect;
		DProgressPos ProgressPos;
		QString ProgressText;
};

#endif // SPLASHSCREEN_H
