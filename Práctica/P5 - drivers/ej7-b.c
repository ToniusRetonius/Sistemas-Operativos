#define TIEMPO_PRE_OP 200
#define TIEMPO_POST_ENCENDIDO 50

// variables de sincro
semaphore mutex;
semaphore tiempo;
semaphore arm_status;
semaphore data_ready;

// timer
static int timer = 0;

int unidad_disco_init()
{
    mutex = semaforo_create(1);
    tiempo = semaforo_create(0);
    arm_status = semaforo_create(0);
    data_ready = semaforo_create(0);
    irq_register(IRQ_6, handler_6);
    irq_register(IRQ_7, handler_7);
}

void handler_6()
{
    if (IN(ARM_STATUS) == 1){arm_status.unlock()}
    if (IN(DATA_READY) == 1){data_ready.unlock()}
}
void handler_7()
{
    // este lo que hace es verificar que los tiempos se cumplan
    while (timer > 0 )
    {
        timer--;
    }
    timer.unlock();
}

int disco_write(int sector, void* data)
{
    // pedimos acceso
    mutex.lock();
    // necesitamos esperar 200ms siempre antes de cualquier op
    timer = TIEMPO_PRE_OP;
    tiempo.lock();
    
    // antes de escribir : nos aseguramos que esté prendido el motor 
    int prendido = IN(DOR_STATUS);

    if(!prendido) 
    {
        // prendemos y esperamos
        OUT(DOR_IO, 1);

    }
    // 50 dice que tarda en tener la velocidad adecuada
    timer = TIEMPO_POST_ENCENDIDO;
    tiempo.lock();

    // necesitamos la pista 
    int pista = sector / cantidad_sectores_por_pista();

    // nos movemos a la pista
    OUT(ARM, pista);

    // esperamos
    arm_status.lock();
    
    // dentro de la pista nos movemos al sector adecuado
    OUT(SEEK_SECTOR, sector);

    // capturamos la data a escribirle al disco
    unsigned char* info;
    copy_from_user(info, data,sizeof(info));

    // escribimos
    escribir_datos(info);

    // esperamos a que esté todo escrito
    data_ready.lock();

    // apagamos por una cuestión energética
    OUT(DOR_STATUS, 1);

    // liberamos el mutex
    mutex.unlock();

    return 0;
}