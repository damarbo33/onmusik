#include "Iofrontend.h"
#include "uilistgroup.h"
#include "beans/listgroupcol.h"
#include "uilistcommon.h"

//Estos son los campos necesarios para identificar la aplicacion
//de dropbox que he dado de alta mediante oauth.
//No deben ser de dominio publico
const string cliendid="";
const string secret="";
bool Iofrontend::finishedDownload;
const int MAXDBGAIN = 20;

/**
* Constructor
* HEREDA DE Ioutil
*
*/
Iofrontend::Iofrontend(){
    Traza::print("Constructor de IoFrontend", W_DEBUG);
    convColor = new Colorutil();
    imgGestor = new ImagenGestor();

    Traza::print("Creando objetos de cada menu", W_PARANOIC);
    for (int i=0; i < MAXMENU; i++){
        ObjectsMenu[i] = new tmenu_gestor_objects(this->getWidth(), this->getHeight()); //Inicializo el puntero asignado a cada menu. En el constructor de esta clase,                                                    //se crean tantos objetos como se defina en la constante MAXOBJECTS
    }

    for (int i=0; i < MAXMENU*MAXOBJECTS; i++){
        pt2Func[i] = NULL;
    }
    propertiesPt2Func.size = 0;
    Traza::print("Asignando elementos y acciones", W_PARANOIC);
    initUIObjs();

    //Despues de inicializar todos los objetos, debemos mostrar la pantalla del reproductor
    //y cargar la playlist, lo que tarda un poco. Por eso refrescamos la pantalla sin nada
    //, refrescamos la pantalla para que se pinte todo vacio y finalmente volvemos a refrescar
    //para que se vea todo correctamente
    posAlbumSelected = 0;
    posSongSelected = 0;

    juke = new Jukebox();
    player = new AudioPlayer();
    finishedDownload = false;
    threadPlayer = NULL;
    threadDownloader = NULL;
    setSelMenu(PANTALLAREPRODUCTOR);
    tEvento evento;
    drawMenu(evento);
    refreshAlbumAndPlaylist();
    drawMenu(evento);
    bienvenida();
    Traza::print("Fin Constructor de IoFrontend", W_PARANOIC);
}



/**
* Destructor
*/
Iofrontend::~Iofrontend(){
    Traza::print("Destructor de IoFrontend", W_DEBUG);
    delete convColor;

//  A los objetos no los podemos eliminar porque no fueron creados dinamicamente
    Traza::print("Eliminando objetos de cada Menu", W_DEBUG);
    for (int i=0; i < MAXMENU; i++){
        Traza::print("Eliminando menu: ", i, W_PARANOIC);
        delete ObjectsMenu[i];
    }
    delete juke;
    delete player;
    Traza::print("Destructor de IoFrontend FIN", W_DEBUG);
}


/**
* Inicializa los objetos que se pintaran por pantalla para cada pantalla
* Debe llamarse a este metodo despues de haber inicializado SDL para que se puedan centrar los componentes correctamente
**/

void Iofrontend::initUIObjs(){

    ObjectsMenu[PANTALLAPREGUNTA]->add("valor", GUIINPUTWIDE, 0, -20 * zoomText, INPUTW, Constant::getINPUTH(), "Dato:", true);
    ObjectsMenu[PANTALLAPREGUNTA]->add("btnAceptarPregunta", GUIBUTTON, -(BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLAPREGUNTA]->add("btnCancelarPregunta", GUIBUTTON, (BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    ObjectsMenu[PANTALLAPREGUNTA]->add("borde", GUIPANELBORDER,0,0,0,0, "Introduzca el dato", false);
    ObjectsMenu[PANTALLAPREGUNTA]->getObjByName("valor")->setColor(cBlanco);


    ObjectsMenu[PANTALLACONFIRMAR]->add("borde", GUIPANELBORDER,0,0,0,0, "Seleccione una opcion", false);
    ObjectsMenu[PANTALLACONFIRMAR]->add("textosBox", GUITEXTELEMENTSAREA, 20, -40 * zoomText, getWidth()-50, 70, "", true)->setVerContenedor(false);
    ObjectsMenu[PANTALLACONFIRMAR]->add("btnSiConfirma", GUIBUTTON, -(BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLACONFIRMAR]->add("btnNoConfirma", GUIBUTTON, (BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);

    UITextElementsArea *infoTextRom = (UITextElementsArea *)ObjectsMenu[PANTALLACONFIRMAR]->getObjByName("textosBox");
    t_posicion pos = {0,0,0,0};
    infoTextRom->addField("labelDetalle","","",pos, true);
    infoTextRom->setTextColor(cBlanco);

    //2, 2, FAMFAMICONW, FAMFAMICONH
    ObjectsMenu[PANTALLABIENVENIDA]->add("borde", GUIPANEL, 2 + FAMFAMICONW, 2 + FAMFAMICONH,getWidth(),getHeight(), "Bienvenido!", false)->setEnabled(false);;
    ObjectsMenu[PANTALLABIENVENIDA]->add("textosBox", GUITEXTELEMENTSAREA, 80,50,getWidth() - 80,150, "", false)->setVerContenedor(false);
    ObjectsMenu[PANTALLABIENVENIDA]->add("btnSiConfirma", GUIBUTTON, 80, 200, BUTTONW,BUTTONH, "Entendido!", false)->setIcon(tick);
    ObjectsMenu[PANTALLABIENVENIDA]->add("ImgFlecha", GUIBUTTON, 2 + FAMFAMICONW, 0, 50,50, "Ajustar volumen", false)->setIcon(FlechaEsquinaSupIzq)->setVerContenedor(false);

    UITextElementsArea *textLabel = (UITextElementsArea *)ObjectsMenu[PANTALLABIENVENIDA]->getObjByName("textosBox");
    textLabel->addField("labelDetalle","","",pos, true);
    textLabel->setTextColor(cBlanco);

    UIPanel *panel = (UIPanel *)ObjectsMenu[PANTALLABIENVENIDA]->getObjByName("borde");
    panel->setColor(cNegro);
    panel->setAlpha(200);


    ObjectsMenu[PANTALLABROWSER2]->add("ImgFondo", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondo", true)->setEnabled(false);
    ObjectsMenu[PANTALLABROWSER2]->getObjByName("ImgFondo")->setAlpha(150);
    ObjectsMenu[PANTALLABROWSER2]->add(OBJLISTABROWSER2, GUILISTBOX, 0, 0, 0, 0, "LISTADODIR", false)->setVerContenedor(false)->setShadow(false);
    ObjectsMenu[PANTALLABROWSER2]->add(BTNACEPTARBROWSER, GUIBUTTON, -(BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLABROWSER2]->add(BTNCANCELARBROWSER, GUIBUTTON, (BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    ObjectsMenu[PANTALLABROWSER2]->add(ARTDIRBROWSER, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Direccion Browser", false)->setEnabled(false);
    ObjectsMenu[PANTALLABROWSER2]->add("comboBrowser", GUICOMBOBOX, 0, 0, 0, 0, "", false);

    ObjectsMenu[PANTALLAREPRODUCTOR]->add("panelMedia", GUIPANEL, 0,0,0,0, "", true)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("statusMessage",  GUILABEL,  0,0,0,0, "", false)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("albumList", GUILISTBOX, 0,0,0,0, "", true)->setEnabled(true);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("playLists", GUILISTGROUPBOX, 0,0,0,0, "", true)->setEnabled(true);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("spectrum", GUISPECTRUM, 0,0,0,0, "", true)->setEnabled(true);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("labelVol",  GUILABEL,  0,0,0,0, "100%", false)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnAddContent",  GUIBUTTON, 0,0,0,0, "Subir nuevo disco", true)->setIcon(add)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("mediaTimerTotal",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("mediaTimer",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("progressBarMedia", GUIPROGRESSBAR, 0,0,0,0, "", true)->setShadow(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("progressBarVolumen", GUIPROGRESSBAR, 0,0,0,0, "", true)->setShadow(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("ImgVol", GUIBUTTON, 0,0,0,0, "Ajustar volumen", true)->setIcon(sound)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnBackward", GUIBUTTON, 0,0,0,0, "Saltar a canción anterior", true)->setIcon(control_rewind)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnPlay",     GUIBUTTON, 0,0,0,0, "Reproducir", true)->setIcon(control_play)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnStop",     GUIBUTTON, 0,0,0,0, "Parar", true)->setIcon(control_stop)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnForward",  GUIBUTTON, 0,0,0,0, "Saltar a canción siguiente", true)->setIcon(control_fastforward)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnRepeat", GUIBUTTON, 0,0,0,0, "Repetir disco", true)->setIcon(btn_repeat_off)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnRandom", GUIBUTTON, 0,0,0,0, "Aleatorio", true)->setIcon(btn_random_off)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnEqualizer", GUIBUTTON, 0,0,0,0, "Mostrar Ecualizador", true)->setIcon(control_equalizer)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnSwitchEq", GUIBUTTON, 0,0,0,0, "Ecualizador On/Off", true)->setIcon(btn_on)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnResetEq", GUIBUTTON, 0,0,0,0, "Resetear Ecualizador", true)->setIcon(btn_reset_eq)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setVisible(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnResetEq")->setVisible(false);


    for(int i=NBIQUADFILTERS - 1; i >= 0; i--){
        //Se anyade el slider
        ObjectsMenu[PANTALLAREPRODUCTOR]->add("filtroAudio" + Constant::TipoToStr(i), GUISLIDER, 0,0,0,0, frecsEQStr[i], true)->setShadow(false);
        UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i));
        objfilterGraves->setVisible(false);
        objfilterGraves->setProgressMax(MAXDBGAIN);
        objfilterGraves->setProgressPos(MAXDBGAIN/2);
        objfilterGraves->setShowHint(false);
        objfilterGraves->setTextColor(cBlanco);
    }


    //ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("panelMedia")->setAlpha(150);
    ((UIPanel *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("panelMedia"))->setColor(cGrisOscuro);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("statusMessage")->setTextColor(cBlanco);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimerTotal")->setTextColor(cBlanco);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimer")->setTextColor(cBlanco);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("labelVol")->setTextColor(cBlanco);
    ((UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarMedia"))->setTypeHint(HINT_TIME);
    UISpectrum *objSpectrum = (UISpectrum *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum");
    objSpectrum->setColor(cGrisOscuro);
    objSpectrum->setColorFondo(cGrisClaro);

    UIProgressBar *objProg = (UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarVolumen");
    objProg->setProgressMax(SDL_MIX_MAXVOLUME);
    objProg->setProgressPos(SDL_MIX_MAXVOLUME);


    UIList *albumList = ((UIList *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList"));
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
    albumList->setColor(cGrisOscuro);
    albumList->setTextColor(cBlanco);
    albumList->setEnableLastSelected(true);
    playList->setEnableLastSelected(true);

    UIPopupMenu * popup1 = addPopup(PANTALLAREPRODUCTOR, "popupAlbum", "albumList");
    if (popup1 != NULL){
        popup1->addElemLista("Eliminar Álbum", "delete", controller);
    }

    vector <ListGroupCol *> miCabecera;
    miCabecera.push_back(new ListGroupCol("Canción", ""));
    miCabecera.push_back(new ListGroupCol("Artista", ""));
    miCabecera.push_back(new ListGroupCol("Album", ""));
    miCabecera.push_back(new ListGroupCol("Duración", ""));
    playList->setHeaderLista(miCabecera);
    playList->adjustToHeader(false);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->addHeaderWith(100);
    playList->setColor(cNegroClaro);
    playList->setTextColor(cBlanco);


    //Establecemos los elementos que se redimensionan
    setDinamicSizeObjects();
    //Botones para la pantalla de los directorios
    addEvent(BTNACEPTARBROWSER, &Iofrontend::marcarBotonSeleccionado);
    addEvent(BTNCANCELARBROWSER, &Iofrontend::marcarBotonSeleccionado);
    addEvent(OBJLISTABROWSER2, &Iofrontend::accionesListaExplorador);
    addEvent("btnAceptarPregunta", &Iofrontend::simularIntro);
    addEvent("btnCancelarPregunta", &Iofrontend::simularEscape);
    //Botones para la pantalla de confirmacion
    addEvent("btnSiConfirma", &Iofrontend::marcarBotonSeleccionado);
    addEvent("btnNoConfirma", &Iofrontend::marcarBotonSeleccionado);

    //Botones para la pantalla de video
    addEvent("btnPlay",  &Iofrontend::accionesMediaPause);
    addEvent("btnStop",  &Iofrontend::accionesMediaStop);
    addEvent("btnForward",  &Iofrontend::accionesMediaAvanzar);
    addEvent("btnBackward",  &Iofrontend::accionesMediaRetroceder);
    addEvent("progressBarMedia", &Iofrontend::mediaClicked);
    addEvent("btnAddContent", &Iofrontend::uploadDiscToDropbox);
    addEvent("albumList", &Iofrontend::selectAlbum);
    addEvent("playLists", &Iofrontend::accionesPlaylist);
    addEvent("popupAlbum", &Iofrontend::accionAlbumPopup);
    addEvent("progressBarVolumen", &Iofrontend::accionVolumen);
    addEvent("ImgVol", &Iofrontend::accionVolumenMute);
    addEvent("btnRepeat", &Iofrontend::accionRepeat);
    addEvent("btnRandom", &Iofrontend::accionRandom);
    addEvent("comboBrowser", &Iofrontend::accionCombo);
    addEvent("btnEqualizer", &Iofrontend::accionesEqualizer);
    addEvent("filtroAudio0", &Iofrontend::accionesfiltroAudio0);
    addEvent("filtroAudio1", &Iofrontend::accionesfiltroAudio1);
    addEvent("filtroAudio2", &Iofrontend::accionesfiltroAudio2);
    addEvent("filtroAudio3", &Iofrontend::accionesfiltroAudio3);
    addEvent("filtroAudio4", &Iofrontend::accionesfiltroAudio4);
    addEvent("filtroAudio5", &Iofrontend::accionesfiltroAudio5);
    addEvent("filtroAudio6", &Iofrontend::accionesfiltroAudio6);
    addEvent("filtroAudio7", &Iofrontend::accionesfiltroAudio7);
    addEvent("filtroAudio8", &Iofrontend::accionesfiltroAudio8);

    addEvent("btnResetEq", &Iofrontend::accionesResetFiltros);
    addEvent("btnSwitchEq", &Iofrontend::accionesSwitchFiltros);
}

/**
* Con el menu pasado por parametro lo dibujamos entero
*/
bool Iofrontend::drawMenu(tEvento evento){
    Traza::print("Iofrontend::drawMenu Inicio", W_PARANOIC);
    bool salir = false;
    this->clearScr(cGrisOscuro);
    Traza::print("Iofrontend::clearScr Fin", W_PARANOIC);
    //Realiza las acciones de cada elemento de pantalla
    salir = casoDEFAULT(evento);
    //Muestra un mensaje durante un tiempo determinado
    showAutoMessage();
    //Muestra el contador de cuadros por pantalla
    if (SHOWFPS) fps();
    Traza::print("Iofrontend::drawMenu Fin", W_PARANOIC);
    this->flipScr();
    return salir;
}

/**
*
*/
int Iofrontend::casoDEFAULT(tEvento evento){
    tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
    bool salir = procesarControles(objMenu, &evento, NULL);
    return salir;
}



/**
* Se encarga de procesar las introducciones de teclas o de joystick para cambiar el contenido
* de los botones, inputs, checks, ... y almacenar en ellos los datos correpondientes.
*/
bool Iofrontend::procesarControles(tmenu_gestor_objects *objMenu, tEvento *evento, tscreenobj *screenEvents){
    Traza::print("procesarControles: Inicio", W_PARANOIC);

    bool execFunc = true;
    bool drawComp = true;
    if (screenEvents != NULL){
        execFunc = screenEvents->execFunctions;
        drawComp = screenEvents->drawComponents;
    }

    //Se procesan los eventos de cada uno de los objetos de un menu
    Object *object;

    if (execFunc){
        //Se llama al action del objeto que esta seleccionado en este momento.
        //Cada objeto tiene el suyo propio o heredado de Object
        objMenu->procEvent(*evento);
        //objMenu->procAllEvent(*evento);
        //Mostramos popups si es necesario
        procesarPopups(objMenu, evento);
        //Hacemos llamada para hacer otros procesados. Por ahora solo en la pantalla de seleccion
        //de roms
        procesarMenuActual(objMenu, evento);
    }

    int posBoton = 0;
    bool salir = false;
    bool botonPulsado = false;
    int estado = 0;
    vector<Object *> objPostProcesado;
    int cursorPrincipal = -1;
    bool updateCursor = false;

    //Recorremos todos los objetos para dibujarlos por pantalla
    try{
         //Procesando el redimensionado de ventana
        if (evento->resize)
            resizeMenu();

        //PINTAMOS ANTES DE PROCESAR LAS ACCIONES. NO SE SI ESTO ES BUENA IDEA
        for (int i=0;i<objMenu->getSize();i++){
            object = objMenu->getObjByPos(i);
            //Finalmente dibujamos el objeto
            if (drawComp && object != NULL){
                if (object->getObjectType() == GUICOMBOBOX){
                    objPostProcesado.push_back(object);
                } else {
                    drawObject(object, evento);
                }
            }
        }
        //Para los objetos que son prioritarios de pintar, lo hacemos en ultimo lugar
        //para que se dibujen sobre el resto
        for(vector<Object *>::iterator it = objPostProcesado.begin(); it < objPostProcesado.end(); ++it){
            Object * obj = *it;
            drawObject(obj, evento);
        }
        objPostProcesado.clear();

        //Procesamos las acciones
        for (int i=0;i<objMenu->getSize();i++){
            object = objMenu->getObjByPos(i);

            if (execFunc && object != NULL){
                if (!object->isPopup()){
                    switch(object->getObjectType()){
                        case GUIBUTTON:
                            //En el caso de los botones tambien procesamos sus eventos
                            botonPulsado = procesarBoton(object, objMenu);
                            estado = evento->mouse_state;
                            if (botonPulsado && ( (evento->isMouse && estado == SDL_RELEASED) || evento->isKey || evento->isJoy)){ //Comprobamos si se ha pulsado el elemento
                                posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
                                if (posBoton >= 0){ //Si hemos encontrado una funcion
                                    if (this->pt2Func[posBoton] != NULL){
                                        //Forzamos a que se actualicen todos los elementos
                                        objMenu->resetElements();
                                        /**Los botones no pueden hacer que se salga de la aplicacion. Solo ejecutamos la funcion*/
                                        //salir = (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        Traza::print("procesarControles: Evento lanzado para " + object->getName(), W_DEBUG);
                                    }
                                }
                            }
                            break;
                        case GUILISTBOX:
                        case GUIPROGRESSBAR:
                        case GUISLIDER:
                        case GUIPOPUPMENU:
                        case GUILISTGROUPBOX:
                        case GUICOMBOBOX:
                            if (procesarBoton(object, objMenu)){ //Comprobamos si se ha pulsado el elemento
                                posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
                                if (posBoton >= 0){ //Si hemos encontrado una funcion
                                    if (this->pt2Func[posBoton] != NULL){
                                        //Forzamos a que se actualicen todos los elementos
                                        objMenu->resetElements();
                                        salir = (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        Traza::print("procesarControles: Evento lanzado para " + object->getName(), W_DEBUG);
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    } // FIN CASE
                }
            } // FIN IF
//            //Finalmente dibujamos el objeto
            /**Aqui iba el dibujado del objeto para aprovechar el bucle pero esto
            * daba problemas. Se corrige para que tenga su propio bucle al inicio
            * de las acciones
            */
            //Comprobamos si el objeto esta lanzando un evento para cambiar el mouse.
            //Solo aceptamos el primer objeto que pida cambiar el mouse
            if (object->getCursor() >= 0){
                updateCursor = true;
                if (cursorPrincipal < 0){
                    cursorPrincipal = object->getCursor();
                }
            }
        }
        if (!updateCursor){
            //Reseteamos el cursor al que hay por defecto
            cursorPrincipal = cursor_arrow;
        }
        //Dibujamos el cursor solo si procede
        this->pintarCursor(evento->mouse_x, evento->mouse_y, cursorPrincipal);

    } catch (Excepcion &e) {
         Traza::print("Excepcion procesarControles: " + string(e.getMessage()), W_ERROR);
    }
    return salir;
}



/**
*
*/
int Iofrontend::accionesMenu(tEvento *evento){
    bool salir = false;

    try{
        int menu = this->getSelMenu();
        tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
        Object *object = objsMenu->getObjByPos(objsMenu->getFocus());

        if (object != NULL){
            if (object->getObjectType() == GUILISTBOX ||
                object->getObjectType() == GUIPOPUPMENU ||
                object->getObjectType() == GUILISTGROUPBOX){

                UIListCommon *objList = (UIListCommon *)object;
                unsigned int pos = objList->getPosActualLista();
                string valorSelec = objList->getValue(pos);
                int destino = objList->getDestino(pos);
                objList->setImgDrawed(false);

                if (valorSelec.compare("salir") == 0){
                    salir = true;
                } else if (destino != -1){
                    this->cargaMenuFromLista(objList, evento);
                }
            }
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion accionesMenu" + string(e.getMessage()), W_ERROR);
    }
    return salir;
}

/**
*
*/
void Iofrontend::cargaMenuFromLista(UIListCommon *obj, tEvento *evento){

    if (obj->getPosActualLista() < 0){
        obj->setPosActualLista(0);
    } else {
        int menucarga = obj->getDestino(obj->getPosActualLista());
        string valorSelec = obj->getValue(obj->getPosActualLista());
        //Si hemos pulsado el boton de volver (que es el ultimo de la lista)
        //reiniciamos la posicion por si volvemos a entrar
        if (obj->getPosActualLista() >= obj->getSize() - 1){
            obj->setPosActualLista(0);
            obj->calcularScrPos();
        }
        //Ahora cargamos el siguiente menu
        cargaMenu(menucarga, valorSelec, evento);
    }
}

/**
*
*/
void Iofrontend::cargaMenu(int menucarga, string valorSelec, tEvento *evento){
    comprobarUnicode(menucarga);
    if (menucarga >= 0 && menucarga <= MAXMENU){
        this->setSelMenu(menucarga);
        //Damos el foco al primer elemento que haya en el menu
        this->ObjectsMenu[menucarga]->setFirstFocus();
    }

    tmenu_gestor_objects *objsMenu = ObjectsMenu[menucarga];
    UIListCommon *objTemp = NULL;

    switch (menucarga){
        default:
            break;
    }
}

/**
*
*/
long Iofrontend::waitMedia(){
    long max_ = 0;
    return max_;
}

/**
*
*/
void Iofrontend::playMedia(tEvento *evento){
}

/**
*
*/
int Iofrontend::accionesCargaPantalla(tEvento *evento){
    int posMenu = accionesGotoPantalla(evento);
    if (posMenu >= 0){
        cargaMenu(posMenu, "", evento);
        return true;
    }
    return false;
}



/**
* El campo checked del control se da valor desde la llamada al procEvent del respectivo menu
* En esta funcion se comprueba el valor checked para saber si se ha pulsado el elemento
*/
bool Iofrontend::procesarBoton(Object * obj, tmenu_gestor_objects *gestorMenu){
    try{
        //Traza::print("Checkeando name: " + string(name) + " valor: " + string(objMenu->getObjByName(name)->isChecked()?"S":"N"), W_ERROR);
        if (obj->getObjectType() == GUICOMBOBOX){
            if (((UIComboBox *)obj)->isValueChanged()){
                ((UIComboBox *)obj)->setValueChanged(false);
                ((UIComboBox *)obj)->setChecked(false);
                ((UIComboBox *)obj)->setFocus(false);
                gestorMenu->findNextFocus();
                return true;
            }
        } else if (obj->isChecked()){
            obj->setChecked(false);
            return true;
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion procesarBoton" + string(e.getMessage()), W_ERROR);
    }
    return false;
}

/**
* Se simula que se ha pulsado la tecla Intro o el boton aceptar del joystick
*/
int Iofrontend::simularEscape(tEvento *evento){
    evento->isKey = true;
    evento->key = SDLK_ESCAPE;
    evento->isJoy = true;
    evento->joy = JOY_BUTTON_START;
    return true;
}

/**
* Se simula que se ha pulsado la tecla Intro o el boton aceptar del joystick
*/
int Iofrontend::simularIntro(tEvento *evento){
    evento->isKey = true;
    evento->key = SDLK_RETURN;
    evento->isJoy = true;
    evento->joy = JOY_BUTTON_A;
    return true;
}

/**
*
*/
int Iofrontend::marcarBotonSeleccionado(tEvento *evento){

    int menu = getSelMenu();
    tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
    int pos = objMenu->getFocus();

    Traza::print("marcarBotonSeleccionado: " + objMenu->getObjByPos(pos)->getName(), W_DEBUG);
    if (pos >= 0){
        objMenu->getObjByPos(pos)->setTag("selected");
        return true;
    }
    return false;
}

/**
* Se asigna la funcion especificada al boton con el nombre especificado.
* No deberian haber botones del mismo nombre
*/
void Iofrontend::addEvent(string nombre, typept2Func funcion){
    addEvent(nombre, funcion, -1);
}

/**
*
*/
void Iofrontend::addEvent(string nombre, typept2Func funcion, int parms){
    if (propertiesPt2Func.size < MAXMENU*MAXOBJECTS){
        propertiesPt2Func.name.push_back(nombre);
        propertiesPt2Func.parms.push_back(Constant::TipoToStr(parms));
        pt2Func[propertiesPt2Func.size] = funcion;
        propertiesPt2Func.size++;
    }
}

/**
* Se busca la posicion del puntero a funcion asociado con el nombre del boton
*/
int Iofrontend::findEventPos(string nombre){

    int i=0;

    while (i < propertiesPt2Func.size){
        if (nombre.compare(propertiesPt2Func.name[i]) == 0)
            return i;
        i++;
    }
    return -1;
}

/**
* Se busca la posicion del puntero a funcion asociado con el nombre del boton y se modifica
*/
void Iofrontend::setEvent(string nombre, typept2Func funcion){
    int pos = findEventPos(nombre);
    if (pos != -1){
        pt2Func[pos] = funcion;
    }
}

void Iofrontend::setEvent(string nombre, typept2Func funcion, int parms){
    int pos = findEventPos(nombre);
    if (pos != -1){
        pt2Func[pos] = funcion;
        propertiesPt2Func.parms[pos] = Constant::TipoToStr(parms);
    }
}


/**
*
*/
void Iofrontend::popUpMenu(tEvento evento){
}

/**
* Con el menu pasado por parametro lo dibujamos entero
*/
void Iofrontend::resizeMenu(){
    for (int i=0; i< MAXMENU; i++){
        ObjectsMenu[i]->setAreaObjMenu(getWidth(),getHeight());
    }
    setDinamicSizeObjects();
}

/**
*
*/
void Iofrontend::setDinamicSizeObjects(){
    try{
        //Calculamos el tamanyo del titulo de los elementos que lo tengan, y redimensionamos el elemento
        //lista que tenga ese menu con el total de la ventana que queda
        for (int i=0; i<MAXMENU; i++){
            try{
                ObjectsMenu[i]->getObjByName(TITLESCREEN)->setTam( 0, 0, this->getWidth(),Constant::getINPUTH());
                int j = 0;
                Object *posibleObj = NULL;

                while (j < ObjectsMenu[i]->getSize()){
                    posibleObj = ObjectsMenu[i]->getObjByPos(j);
                    if(posibleObj != NULL){
                        if (posibleObj->getObjectType() == GUILISTBOX || ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUIPICTURE
                            || posibleObj->getObjectType() == GUILISTGROUPBOX){
                            posibleObj->setTam(0,Constant::getINPUTH(), this->getWidth(), this->getHeight()-Constant::getINPUTH());
                        }

                        if (ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUILISTBOX){
                            ((UIList *)posibleObj)->calcularScrPos();
                        } else if (ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUILISTGROUPBOX){
                            ((UIListGroup *)posibleObj)->calcularScrPos();
                        }
                    }
                    j++;
                }
            } catch (Excepcion &e){}
        }

        //Redimension para el browser de directorios2

        ObjectsMenu[PANTALLABROWSER2]->getObjByName(OBJLISTABROWSER2)->setTam(0, Constant::getINPUTH() + COMBOHEIGHT + 4,this->getWidth(), this->getHeight() - BUTTONH - Constant::getINPUTH() - COMBOHEIGHT - 10 - 4);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName("comboBrowser")->setTam(1, Constant::getINPUTH() + 4, 100, 150);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(BTNACEPTARBROWSER)->setTam( (this->getWidth() / 2) -(BUTTONW + 5), this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(BTNCANCELARBROWSER)->setTam( (this->getWidth() / 2) + 5, this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(ARTDIRBROWSER)->setTam( 0, 0, this->getWidth(), Constant::getINPUTH());


        //Redimension para la pantalla de videos multimedia
        int desp = (this->getWidth() / 2) - FAMFAMICONW*2 - BUTTONW/2;
        int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("panelMedia")->setTam(0, calculaPosPanelMedia(), this->getWidth(), this->getHeight() - calculaPosPanelMedia());
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnBackward")->setTam(desp += (BUTTONW/2), bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnStop")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnForward")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRepeat")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRandom")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("statusMessage")->setTam(desp += FAMFAMICONW*2, bottom, this->getWidth() - desp, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("ImgVol")->setTam(TIMEW- FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarVolumen")->setTam(TIMEW + SEPTIMER, bottom + 2, TIMEW, PROGRESSHEIGHT);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("labelVol")->setTam(2*(TIMEW + SEPTIMER), bottom +1 , TIMEW, PROGRESSHEIGHT);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnAddContent")->setTam(2, 2, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnEqualizer")->setTam(FAMFAMICONW + 4, 2, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarMedia")->setTam( TIMEW + SEPTIMER, bottom - PROGRESSSEPBOTTOM, this->getWidth() - TIMEW*2 - SEPTIMER*2, PROGRESSHEIGHT);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimer")->setTam(SEPTIMER, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimerTotal")->setTam(this->getWidth() - TIMEW, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);

        int albumWith = 200;
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList")->setTam(0, FAMFAMICONH + 2, albumWith, calculaPosPanelMedia() - FAMFAMICONH - 2 - albumWith);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum")->setTam(0, FAMFAMICONH + 2 + calculaPosPanelMedia() - FAMFAMICONH - 2 - albumWith, albumWith, albumWith);

        int yFiltros = FAMFAMICONH + 2 + calculaPosPanelMedia() - FAMFAMICONH - 2 - albumWith;

        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setTam(2, yFiltros + 2, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnResetEq")->setTam(4 + FAMFAMICONW, yFiltros + 2, FAMFAMICONW, FAMFAMICONH);

        int xFilter = 0;
        for(int i=0; i < NBIQUADFILTERS; i++){
            UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i));
            xFilter = (albumWith - 10) / 2 + (i - 4) * 21;
            objfilterGraves->setTam(xFilter, yFiltros + 25, 10, albumWith - 50);
        }

        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists")->setTam(albumWith, 0, this->getWidth() - albumWith, calculaPosPanelMedia());
    } catch (Excepcion &e){
        Traza::print("setDinamicSizeObjects: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
bool Iofrontend::casoPANTALLACONFIRMAR(string titulo, string txtDetalle){
    ignoreButtonRepeats = true;
    Traza::print("casoPANTALLACONFIRMAR: Inicio", W_PARANOIC);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    bool salida = false;
    int menuInicial = getSelMenu();

    //Procesamos el menu antes de continuar para que obtengamos la captura
    //de pantalla que usaremos de fondo
    procesarControles(ObjectsMenu[menuInicial], &askEvento, NULL);
    SDL_Rect iconRectFondo = {0, 0, this->getWidth(), this->getHeight()};
    SDL_Surface *mySurface = NULL;
    takeScreenShot(&mySurface, iconRectFondo);

    //Seguidamente cambiamos la pantalla a la de la confirmacion
    setSelMenu(PANTALLACONFIRMAR);
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLACONFIRMAR];
    objMenu->getObjByName("borde")->setLabel(titulo);

    UITextElementsArea *textElems = (UITextElementsArea *)objMenu->getObjByName("textosBox");
    textElems->setImgDrawed(false);
    textElems->setFieldText("labelDetalle", txtDetalle);

    long delay = 0;
    unsigned long before = 0;
    objMenu->setFocus(0);

    do{
        before = SDL_GetTicks();
        askEvento = WaitForKey();
//        clearScr(cBgScreen);
        printScreenShot(&mySurface, iconRectFondo);
        drawRectAlpha(iconRectFondo.x, iconRectFondo.y, iconRectFondo.w, iconRectFondo.h , cNegro, 200);

        procesarControles(objMenu, &askEvento, NULL);

        flipScr();
        salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
        (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

        if (objMenu->getObjByName("btnSiConfirma")->getTag().compare("selected") == 0){
            salir = true;
            salida = true;
            objMenu->getObjByName("btnSiConfirma")->setTag("");
            Traza::print("Detectado SI pulsado", W_DEBUG);
        } else if (objMenu->getObjByName("btnNoConfirma")->getTag().compare("selected") == 0){
            salir = true;
            salida = false;
            objMenu->getObjByName("btnNoConfirma")->setTag("");
            Traza::print("Detectado NO pulsado", W_DEBUG);
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);

    setSelMenu(menuInicial);
    return salida;
}

/**
*
*/
string Iofrontend::casoPANTALLAPREGUNTA(string titulo, string label){
    ignoreButtonRepeats = true;
    Traza::print("casoPANTALLAPREGUNTA: Inicio", W_PARANOIC);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    string salida = "";
    int menuAnt = getSelMenu();
    setSelMenu(PANTALLAPREGUNTA);

    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAPREGUNTA];
    objMenu->getObjByName("valor")->setLabel(label);
    objMenu->getObjByName("borde")->setLabel(titulo);


    long delay = 0;
    unsigned long before = 0;

    do{
        before = SDL_GetTicks();
        askEvento = WaitForKey();
        clearScr(cBgScreen);
        if (askEvento.isKey){
            Traza::print("letra", askEvento.key, W_DEBUG);
        }

        procesarControles(objMenu, &askEvento, NULL);

        flipScr();
        salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
        (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

        UIInput *input = (UIInput *)objMenu->getObjByName("valor");

        if (input->getSize() > 0 &&
            ( (askEvento.isKey && askEvento.key == SDLK_RETURN)
              || (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_A))) ){
            salida = input->getText();
            salir = true;
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);
    setSelMenu(menuAnt);

    return salida;
}


/**
*
*/
int Iofrontend::accionesGotoPantalla(tEvento *evento){
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *object = objsMenu->getObjByPos(objsMenu->getFocus());

    int posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
    if (posBoton >= 0){ //Si hemos encontrado una funcion
        int posMenu = Constant::strToTipo<int>(propertiesPt2Func.parms[posBoton]);
        if (posMenu >= 0){
            this->setSelMenu(posMenu);
            this->ObjectsMenu[posMenu]->findNextFocus();
            comprobarUnicode(posMenu);
            return posMenu;
        }
    }
    return false;
}

/**
* En este metodo se comprueba si tenemos que activar unicode para que
* las teclas del teclado se traduzcan sin problemas de dependencia de layouts
*/
void Iofrontend::comprobarUnicode(int menu){

    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    int i=0;
    bool found = false;

    while (i < objsMenu->getSize() && !found){
        if (objsMenu->getObjByPos(i) != NULL)
            if (objsMenu->getObjByPos(i)->getObjectType() == GUIINPUTWIDE)
                found = true;
        i++;
    }

    Traza::print("comprobarUnicode: " + Constant::TipoToStr(menu) + ((found == true) ? " UNICODE=S":" UNICODE=N"), W_DEBUG);
    SDL_EnableUNICODE(found);
}

/**
* Establece el nombre del fichero o directorio seleccionado por el explorador de
* ficheros, en el contenido un campo especificado por parámetro
*/
void Iofrontend::setTextFromExplorador(tEvento *evento, UIInput *objCampoEdit){
    try{
        Dirutil dir;
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        //Si el objeto ya tiene datos, comprobamos si existe el directorio que supuestamente contiene
        string uri = dir.getFolder(objCampoEdit->getText());
        if (dir.existe(uri)){
            //Si resulta que existe, hacemos un cambio de directorio para que se muestre
            //el contenido del directorio nada mas pulsar en el boton
            dir.changeDirAbsolute(uri.c_str());
        }
        //Abrimos el explorador de archivos y esperamos a que el usuario seleccione un fichero
        //o directorio
        string fichName = showExplorador(evento);
        //Si se ha seleccionado algo, establecemos el texto en el objeto que hemos recibido por parametro
        if (!fichName.empty()){
            objCampoEdit->setText(fichName);
            objMenu->setFocus(objCampoEdit->getName());
        }
    } catch (Excepcion &e){
        Traza::print("setTextFromExplorador: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
string Iofrontend::showExplorador(tEvento *evento){
    Traza::print("showExplorador: Inicio", W_PARANOIC);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    int menuInicio = this->getSelMenu();
    this->setSelMenu(PANTALLABROWSER2);
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLABROWSER2];
    UIList *obj = NULL;
    ignoreButtonRepeats = true;
    string fileUri = "";
    string fileTempSelec = "";
    Dirutil dir;

    try{
        loadComboUnidades();
        obj = (UIList *)objMenu->getObjByName(OBJLISTABROWSER2);
        obj->setFocus(true);
        obj->setTag("");
        obj->setPosActualLista(-1);
        //Forzamos a que se actualicen todos los elementos
        objMenu->resetElements();
        //Seleccionamos a la lista que esta en primer lugar
        objMenu->findNextFocus();


        long delay = 0;
        unsigned long before = 0;
        this->accionesListaExplorador(evento);

        do{
            before = SDL_GetTicks();
            askEvento = WaitForKey();
            clearScr(cBlanco);
            int pos = obj->getPosActualLista();
            //Carga de imagenes de fondo en la pantalla del explorador de ficheros
            if (pos >= 0){
                fileTempSelec = obj->getListNames()->get(pos);
                string ruta = dir.getDirActual() +  Constant::getFileSep() + fileTempSelec;
                UIPicture *objPict = (UIPicture *)objMenu->getObjByName("ImgFondo");

                if (objPict->getImgGestor()->getRuta().compare(ruta) != 0){
                    if (dir.findIcon(fileTempSelec.c_str()) == page_white_picture){
                        objPict->loadImgFromFile(ruta);
                        objPict->getImgGestor()->setBestfit(false);
                    } else {
                        if (objPict->getImgGestor()->clearFile())
                            objPict->setImgDrawed(false);
                    }
                    obj->setImgDrawed(false);
                    objMenu->getObjByName(BTNACEPTARBROWSER)->setImgDrawed(false);
                    objMenu->getObjByName(BTNCANCELARBROWSER)->setImgDrawed(false);
                    objPict->getImgGestor()->setRuta(ruta);
                }
            }

            procesarControles(objMenu, &askEvento, NULL);
            if (objMenu->getFocus() < 0){
                objMenu->findNextFocus();
            }

            //fps();
            flipScr();
            salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
            ( ((askEvento.isKey && askEvento.key == SDLK_ESCAPE) || !obj->getTag().empty())
             || objMenu->getObjByName(BTNACEPTARBROWSER)->getTag().compare("selected") == 0
             || objMenu->getObjByName(BTNCANCELARBROWSER)->getTag().compare("selected") == 0);


            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            if(delay > 0) SDL_Delay(delay);
        } while (!salir);

    } catch (Excepcion &e){
        Traza::print("Error en showExplorador: " + string(e.getMessage()), W_ERROR);
    }

    this->setSelMenu(menuInicio);
    string fileSelec;
    string diractual = dir.getDirActual();

    if (obj != NULL){
        // En el caso de que se haya pulsado el boton aceptar, obtenemos el elemento seleccionado
        if (objMenu->getObjByName(BTNACEPTARBROWSER)->getTag().compare("selected") == 0){
            int pos = obj->getPosActualLista();
            if (pos >= 0){
                fileSelec = obj->getListNames()->get(pos);
                obj->setTag(diractual + tempFileSep + fileSelec);
            }
        }
        fileUri = obj->getTag();
    }

    try{
        objMenu->getObjByName(BTNCANCELARBROWSER)->setTag("");
        objMenu->getObjByName(BTNACEPTARBROWSER)->setTag("");
    } catch (Excepcion &e){
        Traza::print("showExplorador: reseteando botones: " + string(e.getMessage()), W_ERROR);
    }

    //No queremos que se seleccionen directorios incorrectos
    if (fileSelec.compare("..") == 0){
//        obj->setTag("");
//        showMessage("Directorio no valido.", 2000);
//        fileUri = showExplorador(evento);
        fileUri = fileUri.substr(0, fileUri.find_last_of(tempFileSep));
    }


    return fileUri;
}

/**
*
*/
int Iofrontend::accionesListaExplorador(tEvento *evento){

    string fileSelec = "";

    try{
        tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLABROWSER2];
        UIList * obj = (UIList *)objMenu->getObjByName(OBJLISTABROWSER2);
        Dirutil dir;
        bool dirChanged = false;
        string diractual;
        int pos = obj->getPosActualLista();

        if (evento == NULL){
            pos = 0;
            dirChanged = true;
        } else if (pos >= 0){
            string fileSelec = obj->getListNames()->get(pos);
            string valorSelec = obj->getListValues()->get(pos);
            Traza::print("cambiando al directorio: " + fileSelec, W_DEBUG);
            if (Constant::strToTipo<int>(valorSelec) == TIPODIRECTORIO){
                dirChanged = dir.changeDirRelative(fileSelec.c_str());
            } else {
                diractual = dir.getDirActual();
                obj->setTag(diractual + tempFileSep + fileSelec);
            }
        }

        //Reseteamos la posicion del explorador para el siguiente directorio
        obj->setPosActualLista(0);
        obj->calcularScrPos();
        //Actualizamos la barra principal con la ruta actual
        diractual = dir.getDirActual();

        ((UIArt *)objMenu->getObjByName(ARTDIRBROWSER))->setLabel(diractual);

        if (dirChanged || pos < 0){
            //Obtenemos el directorio actual y sus elementos
            listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
            unsigned int numFiles = dir.listarDir(diractual.c_str(), filelist);
            Traza::print("Ficheros: " + Constant::TipoToStr(numFiles), W_DEBUG);

            if (filelist != NULL && numFiles > 0){
                //Hacemos espacio en la lista para que la asignacion sea rapida
                obj->resizeLista(numFiles);
                //Recorremos la lista de ficheros y lo incluimos en el objeto de lista para mostrar los datos
                for (unsigned int i = 0; i < numFiles; i++){
                    obj->addElemLista(filelist->get(i).filename , Constant::TipoToStr(filelist->get(i).filetype), filelist->get(i).ico );
                }
            } else {
               obj->resizeLista(1);
               obj->addElemLista(".." , Constant::TipoToStr(TIPODIRECTORIO), folder);
            }
            delete filelist;
        }


    }catch (Excepcion &e){
        Traza::print("accionesListaExplorador: " + string (e.getMessage()), W_ERROR);
    }


    return true;
}

/**
*
*/
void Iofrontend::loadComboUnidades(){
    UIComboBox *combo = (UIComboBox *)ObjectsMenu[PANTALLABROWSER2]->getObjByName("comboBrowser");
    combo->clearLista();
    combo->setPosActualLista(0);
    vector<string> drives;

    Dirutil dir;
    dir.getDrives(&drives);
    int actualDrive = 0;
    string actualDir = dir.getDirActual();

    for (int i=0; i < drives.size(); i++){
        combo->addElemLista(drives.at(i),drives.at(i));
        if (actualDir.find(drives.at(i)) != string::npos){
            actualDrive = i;
        }
    }
    combo->setPosActualLista(actualDrive);
    combo->calcularScrPos();
}

/**
*
*/
int Iofrontend::accionCombo(tEvento *evento){
    Traza::print("Iofrontend::accionCombo", W_DEBUG);
    UIComboBox *combo = (UIComboBox *)ObjectsMenu[PANTALLABROWSER2]->getObjByName("comboBrowser");
    string unidad = combo->getValue(combo->getPosActualLista());
    Traza::print("Iofrontend::accionCombo. Drive: " + unidad, W_DEBUG);
    Dirutil dir;
    bool cambioDir = dir.changeDirAbsolute(unidad.c_str());

    if (cambioDir){
        Traza::print("Iofrontend::accionCombo. EXITO Drive: " + unidad, W_DEBUG);
    } else {
        Traza::print("Iofrontend::accionCombo. ERROR Drive: " + unidad, W_ERROR);
    }

    clearEvento(evento);
    this->accionesListaExplorador(NULL);
    return 0;
}

/**
* Carga el resultado de la seleccion del explorador de archivos en un campo de texto
* - El objeto boton que llama a este metodo debe tener el campo tag rellenado con el campo de destino
*   En el que se quiere que se guarde el resultado
*/
int Iofrontend::loadDirFromExplorer(tEvento *evento){

    try{
        //Obtenemos los objetos del menu actual
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        //Obtenemos el objeto que ha sido seleccionado y que tiene el foco
        Object *obj = objMenu->getObjByPos(objMenu->getFocus());
        //Obtenemos el tag del elemento que indica en que campo deberemos dar valor a la seleccion
        //que hagamos del explorador de archivos
        string tag = obj->getTag();
        if (!tag.empty()){
            setTextFromExplorador(evento, (UIInput *)objMenu->getObjByName(tag));
        } else {
            Traza::print("loadDirFromExplorer. El objeto: " + obj->getLabel() + " no tiene tag asociado " +
                         "que indique donde cargar el texto del explorador", W_ERROR);
        }
    } catch (Excepcion &e){
        Traza::print("loadDirFromExplorer: " + string(e.getMessage()), W_ERROR);
    }
    return 0;
}

/**
*
*/
UIPopupMenu * Iofrontend::addPopup(int pantalla, string popupName, string callerName){
    UIPopupMenu * popup1 = NULL;

    try{
        tmenu_gestor_objects *objMenu = ObjectsMenu[pantalla];
        objMenu->getObjByName(callerName)->setPopupName(popupName);
        ObjectsMenu[pantalla]->add(popupName, GUIPOPUPMENU, 0, 0, 100, 100, popupName, false)->setVisible(false);
        popup1 = (UIPopupMenu *) objMenu->getObjByName(popupName);
        popup1->setFont(getFont());
        popup1->setAutosize(true);
    } catch (Excepcion &e){
        Traza::print("addPopup: " + string(e.getMessage()), W_ERROR);
    }

    return popup1;
}

/**
* Desde este menu podremos mostrar por pantalla la informacion de las roms de cada emulador
*/
bool Iofrontend::procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento){
    return true;
}

/**
* Muestra el menu pasado por parametro como si fuera emergente, dejando el menu anterior
* difuminado sobre el fondo
*/
void Iofrontend::showMenuEmergente(int menu, string objImagenFondo){
    try{
        //Procesamos el menu actual para que se vuelva a repintar
        procesarControles(ObjectsMenu[getSelMenu()], new tEvento(), NULL);
        //Seleccionamos el menu que queremos mostrar como si fuese emergente
        tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
        //Realizamos una captura de pantalla en el objeto imagen de fondo del menu de destino
        UIPicture * fondoImg = (UIPicture *) objMenu->getObjByName(objImagenFondo);
        //Le damos un toque mas oscuro a la imagen
        fondoImg->setAlpha(150);
        //Obtenemos una captura de la pantalla entera
        takeScreenShot(&fondoImg->getImgGestor()->surface);
        //Seleccionamos el menu de destino y procesamos sus controles para que se pinten
        setSelMenu(menu);
        tEvento askEvento;
        clearEvento(&askEvento);
        procesarControles(objMenu, &askEvento, NULL);
    } catch (Excepcion &e) {
         Traza::print("Excepcion showMenuEmergente: " + string(e.getMessage()), W_ERROR);
    }

}

/**
*
*/
bool Iofrontend::procesarPopups(tmenu_gestor_objects *objMenu, tEvento *evento){
        Object *object = objMenu->getObjByPos(objMenu->getFocus());
        if (object != NULL){
            try{
                //Comprobamos si el elemento que estamos pintando deberia mostrar su menu de popup
                if (object->isPopup() && object->isFocus()){
                    Traza::print("procesarPopups", W_DEBUG);
                    //Obtenemos el objeto popup
                    UIPopupMenu *objPopup = (UIPopupMenu *)objMenu->getObjByName(object->getPopupName());
                    //Mostramos el popup
                    objPopup->setVisible(true);
                    //Seteamos la posicion del menu popup
                    if (evento->mouse_x > 0 && evento->mouse_y > 0){
                        //Si no tenemos espacio horizontal por la derecha, intentamos mostrar el popup por
                        //la izquierda del lugar clickado
                        if (evento->mouse_x + objPopup->getW() > this->getWidth()){
                            objPopup->setX(evento->mouse_x - objPopup->getW());
                        } else {
                            objPopup->setX(evento->mouse_x);
                        }
                        //Si no tenemos espacio vertical por debajo, intentamos mostrar el popup por
                        //encima del lugar clickado
                        if (evento->mouse_y + objPopup->getH() > this->getHeight()){
                            objPopup->setY(evento->mouse_y - objPopup->getH());
                        } else {
                            objPopup->setY(evento->mouse_y);
                        }
                    }
                    //Asignamos el elemento que ha llamado al popup
                    objPopup->setCallerPopup(objMenu->getObjByPos(objMenu->getFocus()));
                    //Damos el foco al popup
                    objMenu->setFocus(object->getPopupName());
                    //Evitamos que el elemento procese cualquier evento mientras aparezca el popup
                    object->setEnabled(false);
                    //Forzamos a que se refresque el elemento padre que lanzo el popup
                    object->setImgDrawed(false);
                }
            } catch (Excepcion &e) {
                Traza::print("Excepcion en popup: " + object->getPopupName() + ". " + string(e.getMessage()), W_ERROR);
            }
        }

    return true;
}



/**
* Copia el texto seleccionado desde un popup al elemento que lo llama. Por ahora solo lo hace
* en campos input.
*/
int Iofrontend::accionCopiarTextoPopup(tEvento *evento){
    Traza::print("accionCopiarTextoPopup", W_DEBUG);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        if (objPopup->getCallerPopup() != NULL){
            //Obtenemos el objeto llamador
            if (objPopup->getCallerPopup()->getObjectType() == GUIINPUTWIDE){
                UIInput *objInput = (UIInput *)objPopup->getCallerPopup();
                //Anyadimos el texto al input
                objInput->setText(objInput->getText() + selected);
                //Devolvemos el foco al elemento que llamo al popup
                objsMenu->setFocus(objPopup->getCallerPopup()->getName());
            }
        }
    }
    return 0;
}

/**
*
*/
SongsReproduced::SongsReproduced(){
    clear();
}
/**
*
*/
SongsReproduced::~SongsReproduced(){
}
/**
*
*/
void SongsReproduced::clear(){
    for (int i=0; i < MAXSONG_REPEAT_AVOID; i++){
        arraySongReproduced[i] = false;
    }
    nSongsReproduced = 0;
}
/**
*
*/
void SongsReproduced::addSongReproduced(unsigned int pos, unsigned int max_){
    if (pos < max_){
        arraySongReproduced[pos] = true;
        nSongsReproduced++;
    }

    if (nSongsReproduced == MAXSONG_REPEAT_AVOID || nSongsReproduced == max_){
        clear();
    }
}
/**
*
*/
bool SongsReproduced::isReproduced(unsigned int pos){
    if (pos < MAXSONG_REPEAT_AVOID){
        return arraySongReproduced[pos];
    } else
        return false;
}


/***************************************************************************************************************/
/**                                ACCIONES DE LOS CAMPOS DE LA APLICACION                                     */
/***************************************************************************************************************/

/**
*
*/
string Iofrontend::casoJOYBUTTONS(tEvento *evento){
    ignoreButtonRepeats = true;
    configButtonsJOY(evento);
    return "";
}

/**
*
*/
int Iofrontend::accionesMediaAvanzar(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaAvanzar", W_DEBUG);
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
    if (player->getStatus() != STOPED && playList->getPosActualLista() < playList->getSize() - 1){
        playList->nextSelectedPos();
        startSongPlaylist(evento);
        return 0;
    } else {
        player->setStatus(FINISHEDALBUM);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("statusMessage")->setLabel("");
        return 1;
    }

}

/**
*
*/
int Iofrontend::accionesMediaRetroceder(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaRetroceder", W_DEBUG);
    if (player->getStatus() != STOPED){
        UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
        playList->prevSelectedPos();
        startSongPlaylist(evento);
        return 0;
    } else return 1;

}

/**
*
*/
int Iofrontend::accionesMediaPause(tEvento *evento){
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
    if (player->getStatus() == PLAYING){
        player->pause();
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("statusMessage")->setLabel("Pausado");
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setIcon(control_play);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setImgDrawed(false);
    } else if (player->getStatus() == PAUSED){
        player->pause();
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setIcon(control_pause);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setImgDrawed(false);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("statusMessage")->setLabel(playList->getCol(playList->getLastSelectedPos(), 0)->getTexto());
    } else {
        accionesPlaylist(evento);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setIcon(control_pause);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setImgDrawed(false);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio0(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio0");
    player->setFilter(0, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio1(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio1");
    player->setFilter(1, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio2(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio2");
    player->setFilter(2, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio3(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio3");
    player->setFilter(3, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio4(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio4");
    player->setFilter(4, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio5(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio5");
    player->setFilter(5, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio6(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio6");
    player->setFilter(6, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio7(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio7");
    player->setFilter(7, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}

/**
*
*/
int Iofrontend::accionesfiltroAudio8(tEvento *evento){
    UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio8");
    player->setFilter(8, objfilterGraves->getProgressPos() - MAXDBGAIN / 2);
    return 0;
}
/**
*
*/
int Iofrontend::accionesResetFiltros(tEvento *evento){
    for(int i=0; i < NBIQUADFILTERS; i++){
        UISlider *objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i));
        objfilterGraves->setProgressPos(MAXDBGAIN/2);
    }
    accionesfiltroAudio0(evento);
    accionesfiltroAudio1(evento);
    accionesfiltroAudio2(evento);
    accionesfiltroAudio3(evento);
    accionesfiltroAudio4(evento);
    accionesfiltroAudio5(evento);
    accionesfiltroAudio6(evento);
    accionesfiltroAudio7(evento);
    accionesfiltroAudio8(evento);
    return 0;
}

/**
*
*/
int Iofrontend::accionesSwitchFiltros(tEvento *evento){
    player->setEqualizerOn(!player->isEqualizerOn());
    if (!player->isEqualizerOn()){
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setIcon(btn_off);
    } else {
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setIcon(btn_on);
    }

    UISlider *objfilterGraves;
    for(int i=0; i < NBIQUADFILTERS; i++){
        objfilterGraves = (UISlider *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i));
        objfilterGraves->setEnabled(player->isEqualizerOn());
    }

    return 0;
}
/**
*
*/
int Iofrontend::accionesEqualizer(tEvento *evento){
    player->setEqualizerVisible(!player->isEqualizerVisible());
    UISpectrum *objSpectrum = (UISpectrum *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum");

    Object *btnEq = ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnEqualizer");


    if (!player->isEqualizerVisible()){
        btnEq->setIcon(control_equalizer);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setVisible(false);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnResetEq")->setVisible(false);

        for(int i=0; i < NBIQUADFILTERS; i++){
            ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i))->setVisible(false);
        }
        objSpectrum->setEnabled(true);
        player->setViewSpectrum(true);
    } else {
        //Debemos dibujar el equalizador
        player->setViewSpectrum(false);
        objSpectrum->setEnabled(false);
        objSpectrum->setImgDrawed(false);
        btnEq->setIcon(control_equalizer_blue);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnSwitchEq")->setVisible(true);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnResetEq")->setVisible(true);

        for(int i=0; i < NBIQUADFILTERS; i++){
            ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("filtroAudio" + Constant::TipoToStr(i))->setVisible(true);
        }
    }

    return 0;
}
/**
*
*/
int Iofrontend::accionesMediaStop(tEvento *evento){
    Traza::print("Iofrontend::accionesMediaStop", W_DEBUG);
    player->stop();
    Traza::print("Iofrontend::accionesMediaStop. Terminando thread...", W_DEBUG);
    if (threadPlayer != NULL)
        threadPlayer->join();

    Traza::print("Iofrontend::accionesMediaStop. Thread terminado. Actualizando pantalla", W_DEBUG);
    tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
    UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName("progressBarMedia");
    obj->getObjByName("mediaTimerTotal")->setLabel(Constant::timeFormat(0));
    obj->getObjByName("mediaTimer")->setLabel(Constant::timeFormat(0));
    objProg->setProgressMax(0);
    objProg->setProgressPos(0);
    ((UISpectrum *) ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum"))->setImgDrawed(false);
    obj->getObjByName("statusMessage")->setLabel("");
    obj->getObjByName("spectrum")->setImgDrawed(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setIcon(control_play);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setImgDrawed(false);
    return 0;
}

/**
*
*/
int Iofrontend::accionesPlaylist(tEvento *evento){
    Traza::print("Iofrontend::accionesPlaylist", W_DEBUG);
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
    playList->refreshLastSelectedPos();
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setIcon(control_pause);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setImgDrawed(false);
    startSongPlaylist(evento);
    return 0;
}

/**
*
*/
int Iofrontend::uploadDiscToDropbox(tEvento *evento){
    Traza::print("Iofrontend::uploadDiscToDropbox", W_DEBUG);
    long delay = 0;
    unsigned long before = 0;

    try{
        Dirutil dir;
        //Abrimos el explorador de archivos y esperamos a que el usuario seleccione un fichero
        //o directorio
        string fichName = showExplorador(evento);
        if (!dir.isDir(fichName)){
            fichName = dir.getFolder(fichName);
        }
        //Si se ha seleccionado algo, establecemos el texto en el objeto que hemos recibido por parametro
        if (!fichName.empty()){
                Dropbox dropbox;
                Traza::print("Comprobando autorizacion...", W_DEBUG);
                string accessToken = autenticarDropbox();

                if (accessToken.empty()){
                    showMessage("No se ha podido conectar a dropbox. Se aborta la subida", 2000);
                } else {
                    juke->setObjectsMenu(ObjectsMenu[PANTALLAREPRODUCTOR]);
                    juke->setDirToUpload(fichName);
                    juke->setAccessToken(accessToken);
                    Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::convertir);
                    if (thread->start())
                        Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
                }
        }
    } catch (Excepcion &e){
        Traza::print("uploadDiscToDropbox: " + string(e.getMessage()), W_ERROR);
    }

    return 0;
}

/**
*
*/
int Iofrontend::mediaClicked(tEvento *evento){
    Traza::print("Iofrontend::mediaClicked", W_DEBUG);
    //Comprobamos si se ha terminado la descarga y recargamos en ese caso
    reloadSong(posAlbumSelected, posSongSelected);

    UIProgressBar * objProg = (UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarMedia");
    Traza::print("Pos pulsada barra de tiempo", objProg->getProgressPos(), W_DEBUG);
    float pos = 0;
    if (finishedDownload && objProg->getProgressMax() > 0 && player->getStatus() == PLAYING){
        player->setPosicionCancion(objProg->getProgressPos() * 1000);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionVolumen(tEvento *evento){
    Traza::print("Iofrontend::accionVolumen", W_DEBUG);
    UIProgressBar * objProg = (UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarVolumen");
    UIButton * objMute = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("ImgVol");
    Traza::print("Pos pulsada volumen: ", objProg->getProgressPos(), W_DEBUG);

    int percent = (objProg->getProgressPos() / (float)objProg->getProgressMax()) * 100;

    player->setVol(objProg->getProgressPos());
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("labelVol")->setLabel(Constant::TipoToStr(percent) + "%");

    if (objMute->getIcon() == sound_mute){
        objMute->setIcon(sound);
    }

    return 0;
}

int Iofrontend::accionRepeat(tEvento *evento){
    Traza::print("Iofrontend::accionRepeat", W_DEBUG);
    UIButton * objButton = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRepeat");
    if (objButton->getIcon() == btn_repeat_off){
        objButton->setIcon(btn_repeat);
    } else if (objButton->getIcon() == btn_repeat){
        objButton->setIcon(btn_repeat_off);
    }

    return 0;
}

int Iofrontend::accionRandom(tEvento *evento){
    Traza::print("Iofrontend::accionRandom", W_DEBUG);
    UIButton * objButton = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRandom");
    if (objButton->getIcon() == btn_random_off){
        objButton->setIcon(btn_random);
    } else if (objButton->getIcon() == btn_random){
        objButton->setIcon(btn_random_off);
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionVolumenMute(tEvento *evento){
    Traza::print("Iofrontend::accionVolumenMute", W_DEBUG);
    UIProgressBar * objProg = (UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarVolumen");
    UIButton * objMute = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("ImgVol");
    static int volAnt = 0;

    if (objMute->getIcon() == sound){
        volAnt = objProg->getProgressPos();
        objMute->setIcon(sound_mute);
        objProg->setProgressPos(0);
    } else if (objMute->getIcon() == sound_mute){
        objMute->setIcon(sound);
        objProg->setProgressPos(volAnt);
    }
    player->setVol(objProg->getProgressPos());
    int percent = (objProg->getProgressPos() / (float)objProg->getProgressMax()) * 100;
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("labelVol")->setLabel(Constant::TipoToStr(percent) + "%");

    objMute->setImgDrawed(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("labelVol")->setImgDrawed(false);
    return 0;
}

/**
*
*/
void Iofrontend::setPanelMediaVisible(bool var){
    Traza::print("Iofrontend::setPanelMediaVisible", W_DEBUG);
    try{
        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        obj->getObjByName("panelMedia")->setVisible(var);
        obj->getObjByName("btnBackward")->setVisible(var);
        obj->getObjByName("btnPlay")->setVisible(var);
        obj->getObjByName("btnStop")->setVisible(var);
        obj->getObjByName("btnForward")->setVisible(var);
        obj->getObjByName("progressBarMedia")->setVisible(var);
        obj->getObjByName("mediaTimerTotal")->setVisible(var);
        obj->getObjByName("mediaTimer")->setVisible(var);
        if (!var) {
            t_region regionPantalla = {0 , calculaPosPanelMedia(), getWidth(), getHeight() - calculaPosPanelMedia()};
            clearScr(cGrisOscuro, regionPantalla);
        }

    } catch (Excepcion &e){
        Traza::print("setPanelMediaVisible: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
int Iofrontend::startSongPlaylist(tEvento *evento){
    Traza::print("Iofrontend::startSongPlaylist", W_DEBUG);
    UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
    do{
        string cancion = playList->getValue(playList->getLastSelectedPos());
        playList->setImgDrawed(false);
        string file = Constant::getAppDir() + tempFileSep + "temp.ogg";
        //Intentamos parar el thread del reproductor
        if (threadPlayer != NULL){
            if (threadPlayer->isRunning()){
                Traza::print("Terminando Thread de reproduccion...", W_DEBUG);;
                player->stop();
                while(threadPlayer->isRunning()){
                    Constant::waitms(50);
                }
                Traza::print("Reproduccion terminada.", W_DEBUG);
            }
            delete threadPlayer;
            threadPlayer = NULL;
        }


        //Comprobamos que no haya ninguna descarga activa
        if (threadDownloader != NULL){
            if (threadDownloader->isRunning()){
                juke->abortDownload();
                while(threadDownloader->isRunning()){
                    Constant::waitms(50);
                }
            }
            delete threadDownloader;
            threadDownloader = NULL;
        }


        juke->setFileToDownload(cancion);
        //Borramos el archivo antes de descargarlo
        Dirutil dir;
        dir.borrarArchivo(file);

        //Creamos el thread
        threadDownloader = new Thread<Jukebox>(juke, &Jukebox::downloadFile);
        //Lanzamos el thread
        if (threadDownloader->start()){
            Traza::print("Thread started with id: ",threadDownloader->getThreadID(), W_DEBUG);
            //esperamos a que se carguen al menos 100KB
            std::ifstream::pos_type tam = 0;
            while ((tam = dir.filesize(file.c_str())) < 50000){
                Constant::waitms(50);
            }
            bucleReproductor();
        }

        UIButton * objButtonRandom = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRandom");
        UIButton * objButtonRepeat = (UIButton *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnRepeat");

        if (playList->getPosActualLista() >= playList->getSize() - 1 && objButtonRandom->getIcon() != btn_random){
            //Caso del fin de la lista de reproduccion. Comprobamos si esta activado el repeat,
            //y en ese caso volvemos al inicio de la lista
            if (objButtonRepeat->getIcon() == btn_repeat){
                playList->setPosActualLista(0);
                playList->setLastSelectedPos(0);
            } else {
                return 0;
            }
        } else if (player->getStatus() == FINISHEDSONG){
            //Caso en el que termina la cancion. Comprobamos si esta activado el orden aleatorio,
            //y en ese caso vamos a una cancion aleatoria
            if (objButtonRandom->getIcon() == btn_random && playList->getSize() > 0){
                static unsigned int latestRandomValue = 0;
                unsigned int next = playList->getSize()-1;

//                if (objButtonRepeat->getIcon() == btn_repeat_off
//                    && mySongsReproduced.getNSongsReproduced() == playList->getSize())
//                {
//                    //Ya hemos reproducido todas las canciones de forma aleatoria. Si no
//                    //Tenemos activado el boton Repeat, no seguimos reproduciendo
//                    return 0;
//                } else {
                    Traza::print("playList->getSize() - 1", playList->getSize() - 1, W_DEBUG);
                    //Se limita que no saldra la misma cancion hasta despues de haber reproducido
                    //un maximo de MAXSONG_REPEAT_AVOID veces con la llamada a mySongsReproduced.isReproduced
                    int contadorBucle = 0;
                    do{
                        next = Constant::getRandomRange(0, playList->getSize() - 1);
                        Traza::print("next", next, W_DEBUG);
                        contadorBucle++;
                    } while ( (next == latestRandomValue || mySongsReproduced.isReproduced(next))
                             && playList->getSize() > 1 && contadorBucle < MAXSONG_REPEAT_AVOID * 2
                             );

                    playList->setPosActualLista(next);
                    playList->setLastSelectedPos(next);
                    latestRandomValue = next;
                    mySongsReproduced.addSongReproduced(next, playList->getSize());
//                }
            } else {
                playList->nextSelectedPos();
            }
        }
    } while (player->getStatus() != STOPED && playList->getPosActualLista() < playList->getSize());

    return 0;
}
/**
*
*/
bool Iofrontend::bucleReproductor(){
    Traza::print("Iofrontend::bucleReproductor", W_DEBUG);
    bool salir = false;
    long delay = 0;
    unsigned long before = 0;
    unsigned long timer1s = 0;
    unsigned long timerPanelMedia = 0;
    ignoreButtonRepeats = true;
    tEvento askEvento;
    long lenSongSec = 0;
    tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
    bool panelMediaVisible = true;
    finishedDownload = false;

    try {
        //Obtenemos de la lista de reproduccion, el tiempo, el nombre y la ruta de la cancion.
        UIListGroup *playList = ((UIListGroup *)obj->getObjByName("playLists"));
        string cancion = playList->getValue(playList->getLastSelectedPos());
        string time = playList->getCol(playList->getLastSelectedPos(), 3)->getValor();
        string file = Constant::getAppDir() + tempFileSep + "temp.ogg";
        Traza::print("cancion: " + cancion + " time: " + time, W_DEBUG);

        //Obtenemos el codigo del album y la cancion que esta reproduciendose actualmente
        UIList *albumList = ((UIList *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList"));
        posAlbumSelected = albumList->getLastSelectedPos();
        posSongSelected = playList->getLastSelectedPos();

        //Asignamos el tiempo total de la cancion
        lenSongSec = time.empty() ? 0 : ceil(Constant::strToTipo<double>(time));
        //Una vez sabemos el maximo de tiempo, damos valor a la barra de progreso con el maximo de segundos
        //y al label para mostrar el total de tiempo de la pelicula
        UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName("progressBarMedia");
        objProg->setProgressMax(lenSongSec);
        obj->getObjByName("mediaTimerTotal")->setLabel(Constant::timeFormat(lenSongSec));

        clearEvento(&askEvento);
        timer1s = SDL_GetTicks();
        timerPanelMedia = timer1s;
        setPanelMediaVisible(true);
        tscreenobj screenEvents;
        screenEvents.drawComponents = true;
        screenEvents.execFunctions = true;
        //Inicializamos el reproductor
        player->setScreen(screen);
        player->initAudio();
        player->setFilename(file);
        player->setStatus(PLAYING);
        player->setViewSpectrum(ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum")->isEnabled());
        player->setSongDownloaded(false);
        player->setObjectsMenu(ObjectsMenu[PANTALLAREPRODUCTOR]);

        if (threadPlayer != NULL){
            delete threadPlayer;
            threadPlayer = NULL;
        }

        obj->getObjByName("statusMessage")->setLabel(playList->getCol(playList->getLastSelectedPos(), 0)->getTexto());
        //Lanzamos el thread del reproductor
        threadPlayer = new Thread<AudioPlayer>(player, &AudioPlayer::loadFile);
        if (threadPlayer->start())
            Traza::print("Thread reproductor started with id: ", threadPlayer->getThreadID(), W_DEBUG);

        do{
                clearScr(cGrisOscuro);
                //Procesamos los controles de la aplicacion
                askEvento = WaitForKey();
                before = SDL_GetTicks();

                //Actualizamos el indicador de la barra de progreso y del tiempo actual
                if (before - timer1s > 500 && panelMediaVisible){
                    obj->getObjByName("mediaTimer")->setLabel(Constant::timeFormat(player->getActualPlayTime()/1000));
                    objProg->setProgressPos(player->getActualPlayTime()/1000);
                    timer1s = before;
                }
                //Procesamos los eventos para cada elemento que pintamos por pantalla
                procesarControles(obj, &askEvento, &screenEvents);
                //Si pulsamos escape, paramos la ejecucion
                salir = (askEvento.isKey && askEvento.key == SDLK_ESCAPE) || player->getStatus() == STOPED
                        || player->getStatus() == FINISHEDSONG;

                if (salir && (player->getStatus() == STOPED
                        || player->getStatus() == FINISHEDSONG))
                    Traza::print("Saliendo por fin de cancion", W_DEBUG);

                if ((askEvento.isKey && askEvento.key == SDLK_SPACE) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_START)){
                    player->pause();
                    timerPanelMedia = SDL_GetTicks();
                } else if ((askEvento.isKey && askEvento.key == SDLK_RIGHT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_RIGHT)){
                    if (finishedDownload && objProg->getProgressPos() + 10 < objProg->getProgressMax()){
                        reloadSong(posAlbumSelected, posSongSelected);
                        player->forward(10000);
                        timerPanelMedia = SDL_GetTicks();
                    }
                } else if ((askEvento.isKey && askEvento.key == SDLK_LEFT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_LEFT)){
                    if (finishedDownload){
                        reloadSong(posAlbumSelected, posSongSelected);
                        player->rewind(10000);
                        timerPanelMedia = SDL_GetTicks();
                    }
                } else if (askEvento.resize){
                    clearScr(cGrisOscuro);
                    setDinamicSizeObjects();
                } else if (askEvento.isMouseMove){
                    if (askEvento.mouse_y > calculaPosPanelMedia()){
                        timerPanelMedia = SDL_GetTicks();
                    }
                } else if (askEvento.quit){
                    player->stop();
                    while(threadPlayer->isRunning()){};
                    salir = true;
                    exit(0);
                }

                if(player->getNeed_refresh()){
                    refreshSpectrum(player);
                }

                //Recargamos la cancion si se ha terminado la descarga de la misma
                //y no se habia obtenido informacion del tiempo total de la cancion
                if (lenSongSec == 0){
                    reloadSong(posAlbumSelected, posSongSelected);
                //Si no se ha descargado, comprobamos si se ha terminado la descarga
                } else if (!threadDownloader->isRunning() && !finishedDownload && player->getStatus() == PLAYING &&
                            posAlbumSelected == albumList->getLastSelectedPos() && !player->isSongDownloaded()){
                    player->setSongDownloaded(true);
                }

                flipScr();
                delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
                if(delay > 0) SDL_Delay(delay);
            } while (!salir);

            //Reseteamos la barra de progreso
            obj->getObjByName("mediaTimerTotal")->setLabel(Constant::timeFormat(0));
            obj->getObjByName("mediaTimer")->setLabel(Constant::timeFormat(0));
            objProg->setProgressMax(0);
            objProg->setProgressPos(0);
            player->setSongDownloaded(true);
            Traza::print("Fin del bucle de reproductor", W_DEBUG);
    } catch (Excepcion &e){
        Traza::print("Excepcion en bucle de reproductor: " + string(e.getMessage()), W_ERROR);
    }

    return true;
}

/**
*
*/
void Iofrontend::reloadSong(int posAlbumSelected, int posSongSelected){
    if (threadDownloader != NULL && player != NULL){
        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        UIListGroup *playList = ((UIListGroup *)obj->getObjByName("playLists"));
        UIList *albumList = ((UIList *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList"));

        //Cuando detectamos que se ha descargado el fichero totalmente, recargamos
        //el thread para que se pueda avanzar o retroceder. Esto es debido a limitaciones
        //en la libreria de SDL para Mix_LoadMUS_RW con un fichero a medio descargar
        //por streaming. Solo lo hacemos si se ha terminado el thread de descarga, se esta
        //reproduciendo una cancion y no se ha seleccionado otro album que haya cambiado la lista
        if (!threadDownloader->isRunning() && !finishedDownload && player->getStatus() == PLAYING &&
            posAlbumSelected == albumList->getLastSelectedPos()){
            Traza::print("Descarga completada correctamente", W_DEBUG);
            //Indicamos que la cancion se ha terminado de descargar
            player->setSongDownloaded(true);
            //Realizamos la peticion de stop y esperamos a que termine el thread
            player->stop();
            threadPlayer->join();
            //Obtenemos la posicion en la que terminamos la reproduccion. Debe ser justo en un segundo
            //especifico, no nos vale en milesimas puesto que no tenemos tanta precision con SDL_MUSIC.
            //Esto se controla en el propio bucle del AudioPlayer
            long posActual = player->getActualPlayTime();
            Traza::print("Thread detenido correctamente", W_DEBUG);
            player->setPosicionInicial(posActual);
            delete threadPlayer;
            threadPlayer = new Thread<AudioPlayer>(player, &AudioPlayer::loadFile);
            if (threadPlayer->start())
                Traza::print("Thread reproductor started with id: ", threadPlayer->getThreadID(), W_DEBUG);
            finishedDownload = true;

            //Una vez que el fichero esta descargado, ya podemos obtener los tags id3 que contienen
            //mayor informacion sobre la cancion que se esta reproduciendo
            //Nos aseguramos que modificamos la posicion correcta con el campo posSongSelected
            playList->setLastSelectedPos(posSongSelected);
            juke->refreshPlayListMetadata();
        }
    }
}

/**
*
*/
int Iofrontend::calculaPosPanelMedia(){
    int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
    return (bottom - PROGRESSSEPBOTTOM - SEPPANELMEDIA);
}

/**
*
*/
void Iofrontend::refreshAlbumAndPlaylist(){
    Traza::print("Iofrontend::refreshAlbumAndPlaylist", W_DEBUG);

    string accessToken = autenticarDropbox();
    if (accessToken.empty()){
        showMessage("No se ha podido conectar a dropbox", 2000);
    } else {
        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        juke->setObjectsMenu(obj);
        juke->setAccessToken(accessToken);

        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshAlbumAndPlaylist);
        tEvento evento;
        pintarIconoProcesando(true);
        thread->start();
        while (thread->isRunning()){
            evento = WaitForKey();
            procesarControles(obj, &evento, NULL);
            pintarIconoProcesando(false);
        }
//        procesarControles(obj, &evento, NULL);
//        flipScr();
        delete thread;

        ((UIListGroup *)obj->getObjByName("playLists"))->setImgDrawed(false);
        ((UIListGroup *)obj->getObjByName("albumList"))->setImgDrawed(false);
    }
}

/**
*
*/
//void Iofrontend::refreshAlbum(){
//    Traza::print("Comprobando autorizacion refreshAlbum...", W_DEBUG);
//    string accessToken = autenticarDropbox();
//
//    if (accessToken.empty()){
//        showMessage("No se ha podido conectar a dropbox. Se aborta la subida", 2000);
//    } else {
//        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
//        juke->setObjectsMenu(obj);
//        juke->setAccessToken(accessToken);
//
////        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshAlbum);
////        thread->start();
////        pintarIconoProcesando(thread);
////        clearScr();
//        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshAlbum);
//        tEvento evento;
//        pintarIconoProcesando(true);
//        thread->start();
//        while (thread->isRunning()){
//            evento = WaitForKey();
//            procesarControles(obj, &evento, NULL);
//            pintarIconoProcesando(false);
//        }
//        procesarControles(obj, &evento, NULL);
//        flipScr();
//        delete thread;
//
//        ((UIListGroup *)obj->getObjByName("albumList"))->setImgDrawed(false);
//    }
//}

/**
* Se llama cuando se hace doble click o se selecciona un nuevo album de la lista
*/
int Iofrontend::selectAlbum(tEvento *evento){
    Traza::print("Iofrontend::selectAlbum", W_DEBUG);
    UIList *albumList = ((UIList *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList"));
    Traza::print("Comprobando autorizacion selectAlbum...", W_DEBUG);
    string accessToken = autenticarDropbox();

    if (accessToken.empty()){
        showMessage("No se ha podido conectar a dropbox. Se aborta la subida", 2000);
    } else {
        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        juke->setObjectsMenu(obj);
        juke->setAccessToken(accessToken);
//        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlaylist);
//        if (thread->start())
//            Traza::print("Thread started with id: ",thread->getThreadID(), W_DEBUG);
//        pintarIconoProcesando(thread);
//        clearScr();

        Thread<Jukebox> *thread = new Thread<Jukebox>(juke, &Jukebox::refreshPlaylist);
        tEvento evento;
        procesarControles(obj, &evento, NULL);
        pintarIconoProcesando(true);
        thread->start();
        while (thread->isRunning()){
            evento = WaitForKey();
            procesarControles(obj, &evento, NULL);
            pintarIconoProcesando(false);
        }

        UIListGroup *playList = ((UIListGroup *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("playLists"));
        playList->setPosActualLista(0);
        playList->refreshLastSelectedPos();
        procesarControles(obj, &evento, NULL);
        flipScr();
        delete thread;
    }
    return 0;
}

/**
*
*/
string Iofrontend::autenticarDropbox(){
    Traza::print("Iofrontend::autenticarDropbox", W_DEBUG);
    Dropbox *dropbox = new Dropbox();
    if (this->accessToken.empty()){
        Traza::print("Comprobando autorizacion autenticarDropbox...", W_DEBUG);
        Constant::setExecMethod(launch_create_process);

        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        Thread<Dropbox> *thread = new Thread<Dropbox>(dropbox, &Dropbox::authenticate);
        tEvento evento;
        pintarIconoProcesando(true);
        thread->start();
        while (thread->isRunning()){
            evento = WaitForKey();
            procesarControles(obj, &evento, NULL);
            pintarIconoProcesando(false);
        }
        procesarControles(obj, &evento, NULL);
        flipScr();

        string mensaje = "Para usar la aplicación debes dar permisos desde tu cuenta de dropbox. ";
        mensaje.append("A continuación se abrirá un explorador. Debes logarte en Dropbox y pulsar el botón de \"PERMITIR\".");
        mensaje.append("Seguidamente deberás copiar el código obtenido y pegarlo en la ventana de Onmusik que aparecerá a continuación.");

        if (thread->getExitCode() != ERRORCONNECT){
            this->accessToken = dropbox->getAccessToken();
            //Si despues de autenticarse, no se ha podido obtener el access token, lo obtenemos manualmente
            if (this->accessToken.empty()){
                bool permiso = casoPANTALLACONFIRMAR("Autorizar aplicación", mensaje);
                if (permiso){
                    dropbox->launchAuthorize(cliendid);
                    string code = casoPANTALLAPREGUNTA("Autorizar aplicación", "Introduce el campo obtenido de la página de dropbox (CTRL+V)");
                    if (!code.empty()){
                        clearScr(cGrisOscuro);
                        procesarControles(obj, &evento, NULL);
                        flipScr();
                        this->accessToken = dropbox->storeAccessToken(cliendid, secret, code);
                    }
                }
            }
        } else {
            showMessage("No se ha podido autenticar en dropbox. Revise su conexión o especifique datos de proxy", 4000);
        }
        delete thread;
        delete dropbox;
    }
    return this->accessToken;
}



/**
* Copia el texto seleccionado desde un popup al elemento que lo llama. Por ahora solo lo hace
* en campos input.
*/
int Iofrontend::accionAlbumPopup(tEvento *evento){
    Traza::print("Iofrontend::accionAlbumPopup", W_DEBUG);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        if (objPopup->getCallerPopup() != NULL){
            //Obtenemos el objeto llamador
            if (objPopup->getCallerPopup()->getObjectType() == GUILISTBOX){
                UIList *objList = (UIList *)objPopup->getCallerPopup();
                string borrar = objList->getValue(objList->getPosActualLista());
                cout << "Borramos: " << borrar << endl;
                bool confirm = casoPANTALLACONFIRMAR("Borrar Álbum", "¿Está seguro de que desea eliminar: " + borrar + "?");
                if (confirm){
                    string accessToken = autenticarDropbox();
                    Dropbox dropbox;
                    bool res = dropbox.deleteFiles(Constant::uencodeUTF8(borrar), accessToken);
                    if (res){
                        showMessage("Álbum eliminado correctamente", 2000);
                        tEvento askEvento;
                        this->clearScr(cGrisOscuro);
                        clearEvento(&askEvento);
                        procesarControles(ObjectsMenu[menu], &askEvento, NULL);
                        flipScr();
                        refreshAlbumAndPlaylist();
                    }
                    else
                        showMessage("Error al eliminar el álbum", 2000);
                }
            }
        }
    }
    return 0;
}

/**
*
*/
void Iofrontend::refreshSpectrum(AudioPlayer *player){
    UISpectrum *obj = ((UISpectrum *) ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("spectrum"));
    if (obj->isEnabled()){
        AudioPlayer::TStreamMusicPlaying streamData = player->getStreamMusicPlaying();
        obj->buf = streamData.stream[streamData.which];
        player->setNeed_refresh(0);
        obj->setImgDrawed(false);
    }
}

void Iofrontend::bienvenida(){
    Traza::print("bienvenida: Inicio", W_PARANOIC);
    UIList *albumList = ((UIList *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("albumList"));
    if (!this->accessToken.empty() && albumList->getSize() == 0){
        ignoreButtonRepeats = true;
        bool salir = false;
        tEvento askEvento;
        clearEvento(&askEvento);
        bool salida = false;
        int menuInicial = getSelMenu();

        //Procesamos el menu antes de continuar para que obtengamos la captura
        //de pantalla que usaremos de fondo
        procesarControles(ObjectsMenu[menuInicial], &askEvento, NULL);
        SDL_Rect iconRectFondo = {0, 0, this->getWidth(), this->getHeight()};
        SDL_Surface *mySurface = NULL;
        takeScreenShot(&mySurface, iconRectFondo);

        //Seguidamente cambiamos la pantalla a la de la confirmacion
        setSelMenu(PANTALLABIENVENIDA);
        tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLABIENVENIDA];
        //objMenu->getObjByName("borde")->setLabel(titulo);

        string txtDetalle = "Que emocionante! Parece que es la primera vez que accedes a Onmusik!\n";
        txtDetalle.append("Empieza subiendo tus canciones haciendo click en el botón indicado con la flecha\n\n");
        txtDetalle.append("Para subir cada disco, puedes seleccionar la carpeta que contiene todas las canciones");
        txtDetalle.append("o alguna de las canciones contenidas en la carpeta\n");

        UITextElementsArea *textElems = (UITextElementsArea *)objMenu->getObjByName("textosBox");
        textElems->setImgDrawed(false);
        textElems->setFieldText("labelDetalle", txtDetalle);

        long delay = 0;
        unsigned long before = 0;
        objMenu->setFocus(0);

        do{
            before = SDL_GetTicks();
            askEvento = WaitForKey();
            printScreenShot(&mySurface, iconRectFondo);

            procesarControles(objMenu, &askEvento, NULL);

            flipScr();
            salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
            (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

            if (objMenu->getObjByName("btnSiConfirma")->getTag().compare("selected") == 0){
                salir = true;
                salida = true;
                objMenu->getObjByName("btnSiConfirma")->setTag("");
                Traza::print("Detectado SI pulsado", W_DEBUG);
            }

            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            if(delay > 0) SDL_Delay(delay);
        } while (!salir);

        setSelMenu(menuInicial);
    }
}


