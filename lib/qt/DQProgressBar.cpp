#include <libdpp/qt/DQProgressBar.h>

#ifdef QT_GUI_LIB
namespace DTools
{
    DQProgressBar::DQProgressBar(QWidget *parent) : QProgressBar(parent) {
        stepValue=1;
    }

    void DQProgressBar::setStepValue(int step) {
        stepValue=step;
    }

    //void DQProgressBar::step(int step) {
    //    setValue(value()+step);
    //}

    void DQProgressBar::step(void) {
        setValue(value()+stepValue);
    }

}
#endif
