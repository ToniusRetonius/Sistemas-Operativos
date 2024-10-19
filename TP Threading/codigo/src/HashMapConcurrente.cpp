#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

#include "HashMapConcurrente.hpp"
using namespace std;

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    // si no le ponés tolower se rompe todo (gracias cátedra por avisar)
    return (unsigned int)(tolower(clave[0]) - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // capturamos el índice de la hash table 
    int hash = hashIndex(clave);
    
    // pedimos el mutex
    mutex_indices_hashtable[hash].lock();

    // existencia
    bool existe = false;

    // recorremos el bucket en busca de la clave
    for (auto &producto : *tabla[hash])
    {
        // si la encuentro ==> INC
        if (producto.first == clave)
        {
            producto.second++;
            existe = true;   
        }
    }


    if (!existe)
    {
        // si llegamos hasta acá es porque no estaba el par
        hashMapPair nuevo = hashMapPair(clave, 1);
        tabla[hash]->insertar(nuevo);
    }

    // liberamos
    mutex_indices_hashtable[hash].unlock();
    
}

std::vector<std::string> HashMapConcurrente::claves() {
    // claves
    vector<string> claves;

    // iteramos sobre todos hashes para bloquear la i-ésima lista ( búsqueda por bucket )
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++)
    {
        // pedimos el mutex
        mutex_indices_hashtable[i].lock();

        // queremos movernos por la lista enlazada y capturar cada clave ( dado que cada elemento de la tabla es un puntero al bucket, desreferenciamos para acceder a la lista enlazada)
        for (auto &par : *tabla[i])
        {
            claves.push_back(par.first);
        }
        // soltamos el mutex 
        mutex_indices_hashtable[i].unlock();
    }

    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // capturamos el hash
    unsigned int hash = hashIndex(clave);
    
    // pedimos el mutex
    mutex_indices_hashtable[hash].lock();

    // res
    unsigned int res = 0;
    
    // recorremos la lista
    for (auto &par : *tabla[hash])
    {
        if (par.first == clave)
        {
            // liberamos 
            mutex_indices_hashtable[hash].unlock();
            // res es la cantidad de ese producto
            res = par.second;
        }
    }
    // si no está
    mutex_indices_hashtable[hash].unlock();
    return res;
}

float HashMapConcurrente::promedio() {

    // bloqueamos toda la tabla
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++)
    {
        mutex_indices_hashtable[i].lock();
    }

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
        // liberamos
        mutex_indices_hashtable[index].unlock();
    }

    float promedio;
    if (count > 0) {
        promedio = sum / count;
    }else
    {
        promedio = 0;
    }

    return promedio;        
}

// definimos una función que dado un bucket, devuelva la cantidad de productos de ese bucket y cuánto suma
void HashMapConcurrente::promedio_por_bucket(vector<pair<int,int>>& vector_resultados, atomic<int>& nro_fila) {
    // siempre chequeando una fila para hacerle el promedio
    while (true)
    {
        // condición : chequear el atomic<int> que define el nro de fila
        int fila = nro_fila.fetch_add(1);

        // si se indefine ==> break
        if(fila >= HashMapConcurrente::cantLetras) break;

        // pido el mutex
        mutex_indices_hashtable[fila].lock();

        // res < elementos diferentes, cantidad de elementos totales>
        pair<int, int> resultados = make_pair(0,0);

        //
        for (auto &producto : *tabla[fila])
        {
            resultados.first++;
            resultados.second += producto.second;
        }

        // libero
        mutex_indices_hashtable[fila].unlock();

        // modificamos el vector de resultados 
        
        vector_resultados[fila] = resultados;
    }      
}


float HashMapConcurrente::promedioParalelo(unsigned int cant_threads) {
    // declaramos el nro de fila como un atomic int
    atomic<int> nro_fila(0);

    // vector de resultados de la forma <cantidad de objetos, nro total de objetos por bucket>
    vector<pair<int,int>> vector_resultados(HashMapConcurrente::cantLetras, make_pair(0,0));

    // hilos
    vector<thread> threads;

    for (unsigned int i = 0; i < cant_threads; i++)
    {
        // lanzamos los threads : 
        // emplace_back en este contexto tomará como puntero al método de HashMapConcurrente promedio_po_bucket
        // recibirá la instancia actual de la tabla de hash, una referencia al vector de resultados y una referencia al atomic int
        threads.emplace_back(&HashMapConcurrente::promedio_por_bucket, this, std::ref(vector_resultados), std::ref(nro_fila));
    }
    
    // apropiadamente esperamos que terminen
    for(auto &th : threads) th.join();

    // calculamos promedio de la forma tradicional
    float sum = 0.0;
    unsigned int count = 0;

    for (auto &res : vector_resultados)
    {
        count += res.first;
        sum += res.second;
    }

    float promedio;
    if (count > 0) {
        promedio = sum / count;
    }else
    {
        promedio = 0;
    }

    return promedio; 
}

#endif
