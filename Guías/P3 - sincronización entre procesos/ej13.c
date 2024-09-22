// tenemos N mesas con capacidad : 4
// se llenan en orden a medida que llega la gente
// cuando el nro de personas en la mesa es 4 => se invoca JUGAR()
// si las mesas están llenas, los clientes deben esperar (FIFO)
// las mesas se liberan todas juntas

// # mesas
int cant_mesas = n;

// semáforo para modificar la disponibilidad de mesas
sem_t mesas;
int mesas_dispo = n;
int asiento_disponible[cant_mesas][4];

// semáforo para la lista fifo
sem_t fifo;
// a lo sumo puedo tener una ronda completa de personas esperando
int lista_espera[n * 4];

// 
typedef struct {
    int mesa;
    int lugar;
} Asiento;

// definimos la variable que modela estar jugando
bool jueguen;


// sólo se puede ejecutar si hay mesas disponibles
Asiento conseguirMesa(){
    for (int i = 0; i < asiento_disponible; i++)
    {
        // la mesa dispo 
        for (int j = 0; j < asiento_disponible[i]; j++)
        {
            // busco asiento
            if (asiento_disponible[i][j] == 0)
            {
                // lugar libre
                asiento_disponible[i][j] = 1;

                Asiento asiento = {-1,-1};
                asiento.mesa = i;
                asiento.lugar = j;
                return asiento;
            }
        }
    }  
}

void cliente(){
    // esperamos al semǽforo
    wait(mesas);
    // si tengo mesas puedo manejar las variables compartidas
    if (mesas_dispo == 0)
    {
        // libero las variables relacionadas a las mesas
        signal(mesas);
        // me pongo en espera
        wait(fifo);
        lista_espera.push();
        signal(fifo);
    }
    else
    {
        // si hay mesa dispo :
        Asiento asiento = conseguirMesa();
        // actualizamos mesa disponible
        mesas_dispo--;
        // liberamos la variable
        signal(mesas);
        // si el lugar que ocupó es el último
        if (asiento.lugar == 3)
        {
            // jugar
            jugar();
        }

    // mientras el juego continue 
    // la variable depende de la mesa ... bueno ..  detalles
    // asumimos que cumple todo lo que queremos que cumpla 
    while (jueguen)
    {
        // están acá
    }

    wait(mesas);
    // si el juego termina
    abandonarMesa(asiento.mesa);
    signal(mesas);
    }
}

