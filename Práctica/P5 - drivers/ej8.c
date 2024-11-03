// impresora headaches persistent

// comenzar impresión implica ==>
    // 1 - poner en LOC_TEXT_POINTER de 32 bits la dire donde comieza el buffer que contiene el string a imprimir
    // 2 - poner en LOC_TEXT_SIZE de 32 bits la cantidad de chars que se deben leer del buffer
    // 3 - colocar la const START en LOC_CTRL

// si la impresora detecta que hay poca tinta ==> escribe LOW_INK en LOC_CTRL y READY en LOC_STATUS 

// si hay tinta ==> escribe PRINTING en LOC_CTRL y BUSY en LOC_STATUS

// al finalizar, la impresora escribe FINISHED en LOC_CTRL y READY en LOC_STATUS.

// detalles : hay que chequar 5 veces si hay tinta (problema de hw)
// interrupciones : HP_LOW_INK_INT y HP_FINISHED_INT

// variables
mutex impresora;
mutex terminado;

// handlers
void handler_terminado()
{
    terminado.unlock();
}

void handler_tinta(){}

// funciones
int driver_init()
{
    terminado = mutex_create();
    impresora = mutex_create();

    irq_register(HP_FINISHED_INT, handler_terminado);
    irq_register(HP_LOW_INK_INT, handler_tinta);
}

int driver_remove()
{    
    irq_unregister(HP_FINISHED_INT, handler_terminado);
    irq_unregister(HP_LOW_INK_INT, handler_tinta);
}

int driver_write(void* data, int size)
{
    // pedimos la impresora
    impresora.lock();

    // traemos la dire de memoria
    void* dire;
    kmalloc(dire, GFP_KERNEL);
    copy_from_user(dire, data, size);

    // ponemos la dire en el LOC_TEXT_POINTER
    OUT(LOC_TEXT_POINTER, dire);

    // ponemos la #chars
    OUT(LOC_TEXT_SIZE, size);


    // chequeamos la tinta rápido
    int chequeos = 5;
    while (chequeos != 0)
    {
        // ponemos READY
        OUT(LOC_CTRL, START);
        
        while (IN(LOC_CTRL) == START)
        {
            // dejamos que esté en START
        }
        
        // si cambia de estado tenemos que ver si es porque hay baja tinta o porque arrancó a laburar
        if (IN(LOC_CTRL) == PRINTING)
        {
            break;
        }
        chequeos--;
    }
    
    terminado.lock();

    impresora.unlock();
    
    kfree(dire);
    return size;
}