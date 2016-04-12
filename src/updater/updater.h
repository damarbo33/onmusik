#ifndef UPDATER_H
#define UPDATER_H

#include "Constant.h"
#include "Dirutil.h"
#include "unzip/unziptool.h"
#include "httputil.h"

class Updater
{
    public:
        Updater();
        virtual ~Updater();
        void updateFFmpeg(string ruta);
        DWORD updates();
        setRuta(string var) {ruta = var;}
        bool needUpdate(string ruta);

    protected:
        string ruta;

    private:
};

#endif // UPDATER_H
