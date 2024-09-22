int buffer[n];

// tenemos dos semǽforos uno que nos dice la #lugares_vacíos y otro la #lugares_llenos
semaforo llenos = 0;
semaforo vacíos = n;

// recodamos cómo se implementa wait y signal 
// si el valor del sem <= 0 duerme, sino resta 1
// signal suma y después despierta a alguno

// con esto en mente
void write(int mensaje){
    // esperamos que haya lugar para realizar la escritura
    vacíos.wait()
    // tenemos espacio y pusheamos el mensaje al buffer
    buffer.push(mensaje)
    // aumenta un mensaje por tanto llenos++
    llenos.signal()
}

int read(){
    // hay al menos un mensaje en el buffer ? while( llenos <= 0) sleep() ; llenos--; 
    llenos.wait()
    int lectura = buffer.pop()
    // como saco un mensaje, vacíos--
    vacíos.signal()
    return lectura
}