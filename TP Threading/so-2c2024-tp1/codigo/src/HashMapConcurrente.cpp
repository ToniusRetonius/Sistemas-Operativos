#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>

#include "HashMapConcurrente.hpp"

HashMapConcurrente::HashMapConcurrente() {
    for (unsigned int i = 0; i < HashMapConcurrente::cantLetras; i++) {
        tabla[i] = new ListaAtomica<hashMapPair>();
    }
}

unsigned int HashMapConcurrente::hashIndex(std::string clave) {
    return (unsigned int)(clave[0] - 'a');
}

void HashMapConcurrente::incrementar(std::string clave) {
    // capturamos el índice de la hash table 
    unsigned int hash = hashIndex(clave);

    // el elemento está en la lista atómica ?
    bool lo_encontre = false;
    
    // tenemos que controlar quién accede => mutex
    mutex_indices_hashtable[hash].lock();

    for (auto &producto : *tabla[hash])
    {
        // recorremos la lista atómica en busca de nuestro producto (producto, cantidad) 
        if (producto.first == clave)
        {
            // si ya hay un producto, le aumentamos la cantidad
            producto.second++;
            
            // lo encontramos
            found = true;
            
            // fin
            break;
        }
    }

    // en caso de no encontrarlo
    if (!lo_encontre)
    {
        // como no existe, lo creamos
        hashMapPair nuevo_par = make_pair(clave, 1);
        
        // ahora entonces en la tabla insertamos este par
        tabla[hash]->insertar(nuevo_par);
    }
    
    // liberamos el mutex
    mutex_indices_hashtable[hash].unlock();
}

std::vector<std::string> HashMapConcurrente::claves() {
    // la idea es ver si se puede capturar la info de la tabla y volcarla a un .txt para procesarla en otra función aparte
    // esto me garantiza, no bloquear, y evitar inanción

    // otra manera : full bloqueante pero consistente ==>

    // claves
    vector<int> claves;

    // iteramos sobre todos hashes para bloquear la i-ésima lista
    for (int i = 0; i < cantLetras; i++)
    {
        // pedimos el mutex
        mutex_indices_hashtable[i].lock();
    }

    // queremos movernos por la lista enlazada y capturar cada clave
    for (int j = 0; j < cantLetras; j++)
    {
        for (auto par : *tabla[j])
        {
            claves.push_back(par.first);
        }
        // soltamos el mutex 
        mutex_indices_hashtable[j].unlock();
    }

    return claves;
}

unsigned int HashMapConcurrente::valor(std::string clave) {
    // capturamos el hash
    unsigned int hash = hashIndex(clave);

    // recorremos la lista
    for ( auto par : *tabla[hash])
    {
        if (par.first == clave)
        {
            // retornamos la cantidad de ese producto
            return par.second;
        }
    }
    // si no está
    return 0;
}

float HashMapConcurrente::promedio() {
    // hay que modificar esta función para que no deje entrar a nadie desde principio a fin
    // esto garantizará consistencia de los datos

    for (int i = 0; i < cantLetras; i++)
    {
        // bloqueamos toda la tabla
        mutex_indices_hashtable[i].lock();
    }

    float sum = 0.0;
    unsigned int count = 0;
    for (unsigned int index = 0; index < HashMapConcurrente::cantLetras; index++) {
        for (const auto& p : *tabla[index]) {
            sum += p.second;
            count++;
        }
    }

    float promedio;
    if (count > 0) {
        promedio = sum / count;
    }else
    {
        promedio = 0;
    }

    // liberamos
    for (int i = 0; i < cantLetras; i++)
    {
        // desbloqueamos toda la tabla
        mutex_indices_hashtable[i].lock();
    }

    return promedio;        
}


#endif
