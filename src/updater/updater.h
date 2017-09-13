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
        uint32_t updates();
        void setRuta(string var) {ruta = var;}
        bool needUpdate(string ruta);
        void abort();

    protected:
        string ruta;
        HttpUtil *utilHttp;
        bool aborted;

    private:
};

#endif // UPDATER_H
