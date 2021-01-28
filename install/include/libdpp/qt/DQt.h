#ifdef QT_GUI_LIB

#include <QWidget>
#include <QLayout>

namespace DTools {
	namespace DQt {
		/**
		 * @brief Enable/disable all widget inside a QLayout
		 * @param Layout	->	Pointer to layout container
		 * @param Enabled	->	Enable state
		 */
		void LayoutSetEnabled(QLayout *Layout, bool Enabled) {
			for (int ixW=0; ixW<Layout->count(); ++ixW) {
				QWidget *Widget = Layout->itemAt(ixW)->widget();
				if(Widget != nullptr) Widget->setEnabled(Enabled);
			}
		}

		/**
		 * @brief Show/hide all widget inside a QLayout
		 * @param Layout	->	Pointer to layout container
		 * @param Visible	->	Visible state
		 */
		void LayoutSetVisible(QLayout *Layout, bool Visible) {
			for (int ixW=0; ixW<Layout->count(); ++ixW) {
				QWidget *Widget = Layout->itemAt(ixW)->widget();
				if(Widget != nullptr) Widget->setVisible(Visible);
			}
		}


        bool SaveQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
        bool SaveQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
        QSettings::Status SaveQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
        QSettings::Status SaveQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());

        bool RestoreQWindowPosition(QMainWindow& qMainWindow, DTools::DPreferences *DestPrefs = nullptr);
        bool RestoreQWindowPosition(QDialog& qDialog, DTools::DPreferences *DestPrefs = nullptr);
        QSettings::Status RestoreQWindowGeometry(QMainWindow& qMainWindow, QString CompanyName = QString(), QString AppName = QString());
        QSettings::Status RestoreQWindowGeometry(QDialog& qDialog, QString CompanyName = QString(), QString AppName = QString());
	}
}
#endif
