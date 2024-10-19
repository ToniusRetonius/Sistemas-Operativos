#ifndef CHM_CPP
#define CHM_CPP

#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <mutex>

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
    unsigned int hash = hashIndex(clave);
    
    // pedimos el mutex
    mutex_indices_hashtable[hash].lock();

    // recorremos el bucket en busca de la clave
    for (auto &producto : *tabla[hash])
    {
        // si la encuentro ==> INC
        if (producto.first == clave)
        {
            producto.second++;
        }
    }

    // si llegamos hasta acá es porque no estaba el par
    hashMapPair nuevo = hashMapPair(clave, 1);
    tabla[hash]->insertar(nuevo);

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

// definimos una función que dado un intervalo de buckets, calcule el promedio
void HashMapConcurrente::promedio_por_intervalo_hash(vector<unsigned int> claves, float& prom) {
    // promedios
    vector<float> promedios(claves.size());

    for(size_t i = 0; i < claves.size(); i++)
    {
        unsigned int hash = claves[i];
        // bloqueamos todo el bucket
        mutex_indices_hashtable[hash].lock();

        float sum = 0.0;
        unsigned int count = 0;

        for (const auto& p : *tabla[hash]) {
            sum += p.second;
            count++;
        }

        float promedio;
        if (count > 0) {
            promedio = sum / count;
        }else
        {
            promedio = 0;
        }

        // pusheamos el promedio de este bucket
        promedios[i] = promedio;

        // liberamos el mutex
        mutex_indices_hashtable[hash].unlock();
    }
    
    // procesamos todos los promedios y devolvemos un valor 
    int total = 0;
    for (size_t i = 0; i < promedios.size(); i++)
    {
        total += promedios[i];
    }
    prom = total/ (float)promedios.size();
}

float HashMapConcurrente::promedioParalelo(unsigned int cant_threads) {

    // vector de promedios
    vector<float> promedios_por_thread(cant_threads, 0.0);

    // vector de threads
    vector<thread> threads(cant_threads);

    // res
    float promedio = 0;

    // qué pasa si la cantidad de THREADS es menor que la CANT DE LETRAS ? Divido los buckets entre la #threads que tengo ?   
    // distribuimos los buckets
    int buckets_por_thread = cantLetras / cant_threads;
    int resto = cantLetras % cant_threads;

    // vector de claves
    vector<string> letras = {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z"};

    // lanzamos los threads
    for (unsigned int i = 0; i < cant_threads; i++)
    {
        // definimos en qué bucket inicia
        int inicio = i * buckets_por_thread;

        // definimos dónde deja de procesar
        int fin = inicio + buckets_por_thread;

        // si sobran valores, se los pasamos al primer thread
        if (resto > 0)
        {
            fin += resto;
            resto = 0;
        }    

        // le pasamos el vector de claves
        vector<unsigned int> vector_claves;
        for (int i = inicio; i < fin; i++)
        {
            vector_claves.push_back(hashIndex(letras[i]));
        }
        
        // lanzamos los threads
        // la función thread lanza unn thread y necesita saber qué va a ejecutar como puntero, en este caso de la clase HashMapConcurrente el método prom_p_intervalo
        // necesita además saber a qué instancia de la clase va a acceder, por eso 'this'
        // se le pasa el parámetro de la función
        // ref es una función onda wrapper que lo que hace es pasarle por referencia al hilo lo que queremos que modifique ( como no hay race condition, no metemos un mutex, sino habría que pasarle) 
        threads[i] = thread(&HashMapConcurrente::promedio_por_intervalo_hash, this, vector_claves, ref(promedios_por_thread[i]));
    }

    // bancamos que terminen
    for (auto &t: threads) {
        t.join();
    }

    // procesamos los promedios
    for (size_t i = 0; i < promedios_por_thread.size(); i++)
    {
        promedio += promedios_por_thread[i];
    }
    
    return promedio / (float)promedios_por_thread.size();
}

#endif
