// se tienen N mesas de 4 jugadores 
semaforo mesas[N] 
semaforo jugar[N] 

for (int i ; i < N, i++) 
{
    mesas[i] = sem(0) 
    jugar[i] = sem(0)
}
semaforo mesas_libres = sem(N)
semaforo liberar_mesas = sem(0)
semaforo fin = sem(0)

void mesa(int i){
    // la mesa necesita recibir un llamado de un jugador
    int cant_jugadores = 0;
    // espera a llegar a 4 jugadores
    while (cant_jugadores < 4)
    {
        // si le mandan el signal es que se quieren sumar a esta mesa
        mesas[i].wait()
        // sumamos
        cant_jugadores ++
    }

    // el juego ya puede comenzar 
    jugar[i].signal(4)
    
    // fin del juego
    fin.signal(4)

    // chequeamos si están todas las mesas ocupadas
    if (mesas_libres.get() == 1)
    {
        // entonces tenemos que mandar un signal para las mesas que terminaron con estrategia turnlip
        liberar_mesas.signal()
    }

    // esperar a que se liberen todas las mesas (barrera) 
    liberar_mesas.wait()
    liberar_mesas.signal()
}


void jugador(int i){
    // espera mesa libre
    mesas_libres.wait()

    // si hay mesa => consigue una 
    int mesa = conseguirMesa()

    // le avisa que se quiere sumar a esta mesa
    mesas[mesa].signal()

    // espera a que arranque el juego
    jugar[i].wait()

    jugar()

    // esperar a que termine el juego
    fin.wait()

    // abandona la mesa
    abandonarMesa(mesa)
}