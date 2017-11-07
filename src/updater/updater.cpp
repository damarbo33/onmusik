#include "updater.h"

Updater::Updater(){
    ruta = "";
    utilHttp = new HttpUtil();
    aborted = false;
}

Updater::~Updater(){
    delete utilHttp;
}

/**
 * 
 * @return 
 */
uint32_t Updater::updates(){
    return 0;
}

/**
 * 
 * @param ruta
 * @return 
 */
bool Updater::needUpdate(string ruta){
    return false;
}

/**
 * 
 */
void Updater::abort(){
    aborted = true;
    utilHttp->abort();
}
