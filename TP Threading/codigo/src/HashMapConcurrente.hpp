#ifndef HMC_HPP
#define HMC_HPP

#include <mutex>
#include <array>
#include <atomic>
#include <string>
#include <vector>

#include "ListaAtomica.hpp"
using namespace std;

typedef std::pair<std::string, unsigned int> hashMapPair;

class HashMapConcurrente {
 public:
   static constexpr int cantLetras = 26;

   HashMapConcurrente();

   // ejercicio 2) implementar estos métodos
   void incrementar(std::string clave);
   std::vector<std::string> claves();
   unsigned int valor(std::string clave);

   // ejercicio 3a) implementar promedio sin que se ejecute concurrentemente con incrementar
   float promedio();

   // ejercicio 3b) agregamos como método al promedio por bucket como auxiliar para
   void promedio_por_intervalo_hash(std::vector<unsigned int> claves, float& ref_al_promedio);

   // es la función que dada una #threads, procesa de forma paralela por bucket para obtener el promedio
   float promedioParalelo(unsigned int cant_threads);


 private:
   ListaAtomica<hashMapPair> *tabla[HashMapConcurrente::cantLetras];

   static unsigned int hashIndex(std::string clave);

   // agregamos los mutex por cada hash de la tabla
   mutex mutex_indices_hashtable[HashMapConcurrente::cantLetras];
};

#endif  /* HMC_HPP */
