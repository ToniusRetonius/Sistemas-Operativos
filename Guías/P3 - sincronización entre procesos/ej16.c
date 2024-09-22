// la tribu de antropófagos comen de una olla si hay comida, sino el cocinero debe poner comida
semaforo olla = 0;
semaforo cocinero = 1;
atomic int porciones = 0;

void cocinero(){
    while (1)
    {    
        // espera que le avisen que tiene que cocinar
        cocinero.wait()
        // cocina M-porciones
        porciones.set(M)
        // olla ++ M-veces
        olla.signal(M)
    }
}

void canibal(){
    while (1)
    {
        // espera que haya comida en la olla
        olla.wait()
        // come 
        comer();
        // porciones--
        porciones.getAndDec();
        // si se morfó la última porción ==> avisa al cocinero
        if (olla.get() == 0)
        {
            cocinero.signal();
        }
    }
}
