#ifndef CHM_CPP
#define CHM_CPP

#include <vector>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>

#include "CargarArchivos.hpp"

using namespace std;

int cargarArchivo(HashMapConcurrente &hashMap, std::string filePath) 
{
    std::fstream file;
    int cant = 0;
    std::string palabraActual;

    // Abro el archivo.
    file.open(filePath, file.in);
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo '" << filePath << "'" << std::endl;
        return -1;
    }
    while (file >> palabraActual) {
        // Completar (Ejercicio 4)
        // si leemos una palabra es porque hay que agregarla al HashMapConcurrente
        hashMap.incrementar(palabraActual);
        cant++;
    }
    // Cierro el archivo.
    if (!file.eof()) {
        std::cerr << "Error al leer el archivo" << std::endl;
        file.close();
        return -1;
    }
    file.close();
    return cant;
}


void cargar(HashMapConcurrente& hashmap, atomic<int>& archivo, std::vector<std::string>& paths)
{
    while (true)
    {
        // condición
        int file = archivo.fetch_add(1);

        // si se indefine ==> break
        if ((size_t)file >= paths.size()) break;

        // sino
        cargarArchivo(hashmap, paths[file]);
    }
    
}

void cargarMultiplesArchivos(HashMapConcurrente &hashMap, unsigned int cantThreads, std::vector<std::string> filePaths) 
{
    // Completar (Ejercicio 4)
    // nos dan un montón de threads y files para cargar info al hashmap
    
    // qué archivo cargamos
    std::atomic<int> archivo(0);

    // threads
    std::vector<thread> threads;

    // lanzamos
    for (unsigned int i = 0; i < cantThreads; i++)
    {
        threads.emplace_back(cargar, std::ref(hashMap), std::ref(archivo), std::ref(filePaths));
    }

    // terminamos a que terminen
    for (auto &th : threads)
    {
        th.join();
    }

    // fin
}

#endif
