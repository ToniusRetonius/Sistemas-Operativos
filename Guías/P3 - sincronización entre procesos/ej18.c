#define N 100

// cantidad de prendas en el lavarropas
// no pueden entrar en simultaneo las prendas al lavarropas
int prendas = 0;
sem_t sem_prendas = 1;

// defino el arranque sii prendas == 10
sem_t arranque = 0;

// estarListo es una condición que se cumple sii no está lavando
sem_t estoyVacio = 1;

void init(){
    for (int i = 0; i < N; i++)
    {
        // spawn proceso(i);
    }
}

void lavarropas(){
    while (1)
    {
        // si está lleno, lava
        wait(arranque);
        lavar();

        // termina de lavar y se vacía para dar lugar a un nuevo lavado
        wait(sem_prendas);
        prendas = 0;
        signal(sem_prendas);

        // listo
        // estoyListo();
        
        for (int i = 0; i < N; i++)
        {
            // si no hago n de estos se trabaría
            signal(estoyVacio);
        }
        
    }
    
}

void proceso(int i){
    // se fija si puede cargarleropa
    wait(sem_prendas);
    if (prendas < 10)
    {
        prendas++;
        if (prendas == 10)
        {
            // avisamos que puede arrancar a lavar
            signal(arranque);
        }
        // liberamos
        signal(sem_prendas);
    }
    // si el lavarropas está lleno
    else
    {
        // liberamos el recurso para que no se trabe
        signal(sem_prendas);

        // esperamos a que se vacíe
        wait(estoyVacio);

        // ahora sí
        wait(sem_prendas);
        if (prendas < 10)
        {
            prendas++;
            if (prendas == 10)
            {
                // avisamos que puede arrancar a lavar
                signal(arranque);
            }
            // liberamos
            signal(sem_prendas);
        }
    }

    // ahora es hora de sacar la ropa
    saquenmedeaqui();
}