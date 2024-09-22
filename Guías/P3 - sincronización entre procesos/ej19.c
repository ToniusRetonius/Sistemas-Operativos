// dos babuinos no se pueden encontrar yendo en direcciones opuestas
// la cuerda soporta 5 nomás
// se garantiza que si comenzó a caminar, llega al otro lado

#define BABUINOS 100

// definimos un semáforo de cada lado
sem_t extremo_1 = 0;
sem_t extremo_2 = 0;

// adelantandome un poco al punto b) podemos implementar colas fifo en cada extremo
// donde el primer elemento guarda el timestamp de ingreso a la cola
// con esta implementación sabemos quién de los dos llego primero y evitamos inanición
int fifo_extremo_1[BABUINOS];
int fifo_extremo_2[BABUINOS];

sem_t sem_fifo_extremo_1 = 1;
sem_t sem_fifo_extremo_2 = 1;

sem_t filas_completas;

void init(){
    for (int i = 0; i < BABUINOS; i++)
    {
        // aleatoriamente asignamos a los babuinos a los extremos
        int extremo = rand() % 2;
        // spawn babuino(extremo, i);
    }

    // esperamos a que cada monkey se asigne
    wait(filas_completas);

    // realizamos la comparación y lanzamos la mǽxima # de babuinos al otro extremo
    wait(fifo_extremo_1);
    wait(fifo_extremo_2);

    time_t x1 = fifo_extremo_1[0];
    time_t x2 = fifo_extremo_2[0];


    if (x1 < x2)
    {
        // dejamos pasar a x1
        signal(extremo_1);
    }
    else
    {
        // dejamos que pase x2
        signal(extremo_2);
    }
    
    // liberamos
    signal(fifo_extremo_1);
    signal(fifo_extremo_2);
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

void cruzar(int extremo){
    // primero vemos qué extremo es
    if (extremo == 1)
    {
        wait(fifo_extremo_1);  
        // pasan a lo sumo 5
        for (int i = 0; i < min(5, sizeof(fifo_extremo_1)/sizeof(time_t)); i++)
        {
            // pop()
            fifo_extremo_1.pop();
        }
        // liberamos
        signal(fifo_extremo_1);

        // le avisamos al otro extremo que puede cruzar
        signal(extremo_2);
    }
    else
    {
        wait(fifo_extremo_2);  
        // pasan a lo sumo 5
        for (int i = 0; i < min(5, sizeof(fifo_extremo_2)/sizeof(time_t)); i++)
        {
            // pop()
            fifo_extremo_2.pop();
        }
        // liberamos
        signal(fifo_extremo_2);   
        // le avisamos al otro extremo que puede cruzar
        signal(extremo_1);
    }
}

void babuino(int extremo_asignado, int i){
    // se agregan a la fifo con el timestamp
    time_t t;
    time(&t);
        
    if (extremo_asignado == 1)
    {
        // se agrega al extremo 1
        wait(fifo_extremo_1);
        // pusheamos el timestamp de llegada
        fifo_extremo_1.push(t);
        //liberamos
        signal(fifo_extremo_1);
    }
    else
    {
        // se agrega al extremo 2
        wait(fifo_extremo_2);
        // pusheamos el timestamp de llegada
        fifo_extremo_2.push(t);
        //liberamos
        signal(fifo_extremo_2);
    }

    // -------------------- barrera --------------
    // quiero esperar a que todos se asignen bien a sus respectivas fifo
    if (i == BABUINOS)
    {
        for (int i = 0; i < BABUINOS; i++)
        {
            signal(filas_completas);
        }
    }
    // frenate acá
    else
    {
        wait(filas_completas);
    }
    // -------------------- barrera --------------
    
    // ahora decisión
    if (extremo_asignado == 1)
    {
        // esperamos que nos habiliten
        wait(extremo_1);
        // cruza
        cruzar(extremo_asignado);
    }
    else
    {
        // esperamos que nos habiliten
        wait(extremo_2);
        cruzar(extremo_asignado);
    }

}
