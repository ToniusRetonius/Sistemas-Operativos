// se tiene una unidad de discos ópticos con 3 registros de escritura : 
    // DOR_IO : prendido/apagado (1/0)
    // ARM : número de pista seleccionada
    // SEEK_SECTOR : número de sector dentro de la pista
// y 3 registros de lectura :
    // DOR_STATUS : estado de la unidad. prendido/apagado (1/0) **que esté en 1 no quiere decir que realice con éxito una operación
    // ARM_STATUS : si el brazo está en movimiento / si está ubicado ya (0/1)
    // DATA_READY : dato enviado == 1
// contamos con 3 funciones :
    // int cantidad_sectores_por_pista()
    // void escribir_datos(void* src): escribe en el último sector seleccionado los datos apuntados por src
    // void sleep(int ms) : espera ciertos milisegs

semaphore mutex; 

// init del driver
int unidad_disco_init()
{
    // definimos el semaforo
    mutex = semaforo_create(1);
    return 0;
}

int disco_write(int sector, void* data)
{
    // pedimos acceso
    mutex.lock();
    // necesitamos esperar 200ms siempre antes de cualquier op
    sleep(200);
    
    // antes de escribir : nos aseguramos que esté prendido el motor 
    int prendido = IN(DOR_STATUS);

    if(!prendido) 
    {
        // prendemos y esperamos
        OUT(DOR_IO, 1);

    }
    // 50 dice que tarda en tener la velocidad adecuada
    sleep(50);

    // necesitamos la pista 
    int pista = sector / cantidad_sectores_por_pista();

    // nos movemos a la pista
    OUT(ARM, pista);

    // esperamos
    while (IN(ARM_STATUS) != 1)
    {
        // Busy waiting
    }
    
    // dentro de la pista nos movemos al sector adecuado
    OUT(SEEK_SECTOR, sector);


    // capturamos la data a escribirle al disco
    unsigned char* info;
    copy_from_user(info, data,sizeof(info));

    // escribimos
    escribir_datos(info);

    // esperamos a que esté todo escrito
    while (!IN(DATA_READY))
    {
        // busy wait
    }

    // apagamos por una cuestión energética
    OUT(DOR_STATUS, 1);

    // liberamos el mutex
    mutex.unlock();

    return 0;
}