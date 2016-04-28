#include "updater.h"

Updater::Updater(){
    ruta = "";
    utilHttp = new HttpUtil();
    aborted = false;
}

Updater::~Updater(){
    delete utilHttp;
}

DWORD Updater::updates(){
    if (!ruta.empty())
        updateFFmpeg(ruta);

    return 0;
}

/**
*
*/
bool Updater::needUpdate(string ruta){
    Dirutil dir;
    string downDir = ruta + Constant::getFileSep();
    string file1 = downDir + Constant::getFileSep() + "ffmpeg.exe";
    string file2 = downDir + Constant::getFileSep() + "ffprobe.exe";
    return (!dir.existe(file1) || !dir.existe(file2));
}

/**
*
*/
void Updater::updateFFmpeg(string ruta){
    Dirutil dir;
    aborted = false;

    string downDir = ruta + Constant::getFileSep();
    string file1 = downDir + Constant::getFileSep() + "ffmpeg.exe";
    string file2 = downDir + Constant::getFileSep() + "ffprobe.exe";
    string strFileName1 = downDir + Constant::getFileSep() + "ffmpeg.zip";
    string strFileName2 = downDir + Constant::getFileSep() + "ffprobe.zip";

    string url1 = "https://github.com/damarbo33/onmusik/raw/master/Release/ffmpeg.zip";
    string url2 = "https://github.com/damarbo33/onmusik/raw/master/Release/ffprobe.zip";

    if (!dir.existe(file1) && !aborted){
        utilHttp->download(url1, strFileName1);
        if (dir.existe(strFileName1)){
            UnzipTool *unzipTool = new UnzipTool();
            unzipTool->descomprimirZip(strFileName1.c_str());
            delete unzipTool;
            dir.borrarArchivo(strFileName1);
        }
    }

    if (!dir.existe(file2) && !aborted){
        utilHttp->download(url2, strFileName2);
        if (dir.existe(strFileName2)){
            UnzipTool *unzipTool = new UnzipTool();
            unzipTool->descomprimirZip(strFileName2.c_str());
            delete unzipTool;
            dir.borrarArchivo(strFileName2);
        }

    }
}

void Updater::abort(){
    aborted = true;
    utilHttp->abort();
}
