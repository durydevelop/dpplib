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
	}
}
