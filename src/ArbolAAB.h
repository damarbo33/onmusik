/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ArbolAAB.h
 * Author: dmarcobo
 *
 * Created on 9 de noviembre de 2017, 16:13
 */

#ifndef ARBOLAAB_H
#define ARBOLAAB_H

#include <iostream>
#include <cstring>
using namespace std;

template<class DATO>
class ABB {
  private:
   //// Clase local de Lista para Nodo de ArbolBinario:
   template<class DATON>
   class Nodo {
     public:
      // Constructor:
      Nodo(const DATON dat, Nodo<DATON> *izq=NULL, Nodo<DATON> *der=NULL) :
        dato(dat), izquierdo(izq), derecho(der) {}
      // Miembros:
      DATON dato;
      Nodo<DATON> *izquierdo;
      Nodo<DATON> *derecho;
   };

   // Punteros de la lista, para cabeza y nodo actual:
   Nodo<DATO> *raiz;
   Nodo<DATO> *actual;
   int contador;
   int altura;

  public:
   // Constructor y destructor básicos:
   ABB() : raiz(NULL), actual(NULL) {}
   ~ABB() { Podar(raiz); }
   // Insertar en árbol ordenado:
   void Insertar(const DATO dat);
   // Borrar un elemento del árbol:
   void Borrar(const DATO dat);
   // Función de búsqueda:
   bool Buscar(const DATO dat);
   // Comprobar si el árbol está vacío:
   bool Vacio(Nodo<DATO> *r) { return r==NULL; }
   // Comprobar si es un nodo hoja:
   bool EsHoja(Nodo<DATO> *r) { return !r->derecho && !r->izquierdo; }
   // Contar número de nodos:
   const int NumeroNodos();
   const int AlturaArbol();
   // Calcular altura de un dato:
   int Altura(const DATO dat);
   // Devolver referencia al dato del nodo actual:
   DATO &ValorActual() { return actual->dato; }
   // Moverse al nodo raiz:
   void Raiz() { actual = raiz; }
   // Aplicar una función a cada elemento del árbol:
   void InOrden(void (*func)(DATO&) , Nodo<DATO> *nodo=NULL, bool r=true);
   void PreOrden(void (*func)(DATO&) , Nodo<DATO> *nodo=NULL, bool r=true);
   void PostOrden(void (*func)(DATO&) , Nodo<DATO> *nodo=NULL, bool r=true);
  private:
   // Funciones auxiliares
   void Podar(Nodo<DATO>* &);
   void auxContador(Nodo<DATO>*);
   void auxAltura(Nodo<DATO>*, int);
};


// Poda: borrar todos los nodos a partir de uno, incluido
template<class DATO>
void ABB<DATO>::Podar(Nodo<DATO>* &nodo)
{
   // Algoritmo recursivo, recorrido en postorden
   if(nodo) {
      Podar(nodo->izquierdo); // Podar izquierdo
      Podar(nodo->derecho);   // Podar derecho
      delete nodo;            // Eliminar nodo
      nodo = NULL;
   }
}

// Insertar un dato en el árbol ABB
template<class DATO>
void ABB<DATO>::Insertar(const DATO dat)
{
   Nodo<DATO> *padre = NULL;

   actual = raiz;
   // Buscar el dato en el árbol, manteniendo un puntero al nodo padre
   while(!Vacio(actual) && dat != actual->dato) {
      padre = actual;
      if(dat > actual->dato) actual = actual->derecho;
      else if(dat < actual->dato) actual = actual->izquierdo;
   }

   // Si se ha encontrado el elemento, regresar sin insertar
   if(!Vacio(actual)) return;
   // Si padre es NULL, entonces el árbol estaba vacío, el nuevo nodo será
   // el nodo raiz
   if(Vacio(padre)) raiz = new Nodo<DATO>(dat);
   // Si el dato es menor que el que contiene el nodo padre, lo insertamos
   // en la rama izquierda
   else if(dat < padre->dato) padre->izquierdo = new Nodo<DATO>(dat);
   // Si el dato es mayor que el que contiene el nodo padre, lo insertamos
   // en la rama derecha
   else if(dat > padre->dato) padre->derecho = new Nodo<DATO>(dat);
}

// Eliminar un elemento de un árbol ABB
template<class DATO>
void ABB<DATO>::Borrar(const DATO dat)
{
   Nodo<DATO> *padre = NULL;
   Nodo<DATO> *nodo;
   DATO aux;

   actual = raiz;
   // Mientras sea posible que el valor esté en el árbol
   while(!Vacio(actual)) {
      if(dat == actual->dato) { // Si el valor está en el nodo actual
         if(EsHoja(actual)) { // Y si además es un nodo hoja: lo borramos
            if(padre) // Si tiene padre (no es el nodo raiz)
               // Anulamos el puntero que le hace referencia
               if(padre->derecho == actual) padre->derecho = NULL;
               else if(padre->izquierdo == actual) padre->izquierdo = NULL;
            delete actual; // Borrar el nodo
            actual = NULL;
            return;
         }
         else { // Si el valor está en el nodo actual, pero no es hoja
            // Buscar nodo
            padre = actual;
            // Buscar nodo más izquierdo de rama derecha
            if(actual->derecho) {
               nodo = actual->derecho;
               while(nodo->izquierdo) {
                  padre = nodo;
                  nodo = nodo->izquierdo;
               }
            }
            // O buscar nodo más derecho de rama izquierda
            else {
               nodo = actual->izquierdo;
               while(nodo->derecho) {
                  padre = nodo;
                  nodo = nodo->derecho;
               }
            }
            // Intercambiar valores de no a borrar u nodo encontrado
            // y continuar, cerrando el bucle. El nodo encontrado no tiene
            // por qué ser un nodo hoja, cerrando el bucle nos aseguramos
            // de que sólo se eliminan nodos hoja.
            aux = actual->dato;
            actual->dato = nodo->dato;
            nodo->dato = aux;
            actual = nodo;
         }
      }
      else { // Todavía no hemos encontrado el valor, seguir buscándolo
         padre = actual;
         if(dat > actual->dato) actual = actual->derecho;
         else if(dat < actual->dato) actual = actual->izquierdo;
      }
   }
}

// Recorrido de árbol en inorden, aplicamos la función func, que tiene
// el prototipo:
// template<class DATO> void func(DATO&);
template<class DATO>
void ABB<DATO>::InOrden(void (*func)(DATO&) , Nodo<DATO> *nodo, bool r)
{
   if(r) nodo = raiz;
   if(nodo->izquierdo) InOrden(func, nodo->izquierdo, false);
   func(nodo->dato);
   if(nodo->derecho) InOrden(func, nodo->derecho, false);
}

// Recorrido de árbol en preorden, aplicamos la función func, que tiene
// el prototipo:
// template<class DATO> void func(DATO&);
template<class DATO>
void ABB<DATO>::PreOrden(void (*func)(DATO&), Nodo<DATO> *nodo, bool r)
{
   if(r) nodo = raiz;
   func(nodo->dato);
   if(nodo->izquierdo) PreOrden(func, nodo->izquierdo, false);
   if(nodo->derecho) PreOrden(func, nodo->derecho, false);
}

// Recorrido de árbol en postorden, aplicamos la función func, que tiene
// el prototipo:
// template<class DATO> void func(DATO&);
template<class DATO>
void ABB<DATO>::PostOrden(void (*func)(DATO&), Nodo<DATO> *nodo, bool r)
{
   if(r) nodo = raiz;
   if(nodo->izquierdo) PostOrden(func, nodo->izquierdo, false);
   if(nodo->derecho) PostOrden(func, nodo->derecho, false);
   func(nodo->dato);
}

// Buscar un valor en el árbol
template<class DATO>
bool ABB<DATO>::Buscar(const DATO dat)
{
   actual = raiz;

   // Todavía puede aparecer, ya que quedan nodos por mirar
   while(!Vacio(actual)) {
      if(dat == actual->dato) return true; // dato encontrado
      else if(dat > actual->dato) actual = actual->derecho; // Seguir
      else if(dat < actual->dato) actual = actual->izquierdo;
   }
   return false; // No está en árbol
}

// Calcular la altura del nodo que contiene el dato dat
template<class DATO>
int ABB<DATO>::Altura(const DATO dat)
{
   int altura = 0;
   actual = raiz;

   // Todavía puede aparecer, ya que quedan nodos por mirar
   while(!Vacio(actual)) {
      if(dat == actual->dato) return altura; // dato encontrado
      else {
         altura++; // Incrementamos la altura, seguimos buscando
         if(dat > actual->dato) actual = actual->derecho;
         else if(dat < actual->dato) actual = actual->izquierdo;
      }
   }
   return -1; // No está en árbol
}

// Contar el número de nodos
template<class DATO>
const int ABB<DATO>::NumeroNodos()
{
   contador = 0;

   auxContador(raiz); // FUnción auxiliar
   return contador;
}

// Función auxiliar para contar nodos. Función recursiva de recorrido en
//   preorden, el proceso es aumentar el contador
template<class DATO>
void ABB<DATO>::auxContador(Nodo<DATO> *nodo)
{
   contador++;  // Otro nodo
   // Continuar recorrido
   if(nodo->izquierdo) auxContador(nodo->izquierdo);
   if(nodo->derecho)   auxContador(nodo->derecho);
}

// Calcular la altura del árbol, que es la altura del nodo de mayor altura.
template<class DATO>
const int ABB<DATO>::AlturaArbol()
{
   altura = 0;

   auxAltura(raiz, 0); // Función auxiliar
   return altura;
}

// Función auxiliar para calcular altura. Función recursiva de recorrido en
// postorden, el proceso es actualizar la altura sólo en nodos hojas de mayor
// altura de la máxima actual
template<class DATO>
void ABB<DATO>::auxAltura(Nodo<DATO> *nodo, int a)
{
   // Recorrido postorden
   if(nodo->izquierdo) auxAltura(nodo->izquierdo, a+1);
   if(nodo->derecho)   auxAltura(nodo->derecho, a+1);
   // Proceso, si es un nodo hoja, y su altura es mayor que la actual del
   // árbol, actualizamos la altura actual del árbol
   if(EsHoja(nodo) && a > altura) altura = a;
}


#endif