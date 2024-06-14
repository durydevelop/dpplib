#include "dpplib/qt/DQApp.h"

#ifdef QT_GUI_LIB

#include <dpplib/DPath.h>
#include <QFile>
#include <QApplication>

namespace DTools
{
namespace DApp {
    bool SetStyleSheetFromFile(fs::path QssFilename)
    {
        if (DPath::Exists(QssFilename)) {
            QFile QssFile(QssFilename.string().c_str());
            if (QssFile.open(QFile::ReadOnly)) {
                qApp->setStyleSheet(QString(QssFile.readAll()));
                //Log->d("Style Sheet "+Params->CurrStyleSheet+".qss caricato");
                return true;
            }
            else {
                //Log->e("Style Sheet "+Params->CurrStyleSheet+".qss ERRORE lettura");
                return false;
            }
        }
        else {
            //Log->e("Style Sheet "+Params->CurrStyleSheet+".qss NON ESISTE");
            return false;
        }
    }
}
}

#endif
