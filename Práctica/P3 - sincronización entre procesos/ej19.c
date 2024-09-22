// restricciones : capacidad (la cuerda soporta a lo sumo 5 babuinos) y dirección (2 direcciones excluentes)

semaforo exclusividad_cuerda[2]
exclusividad_cuerda[0] = sem(5)
exclusividad_cuerda[1] = sem(0)

void babuino(int dire){
    // necesita saber si puede cruzar, el tema es q a lo sumo 5 pueden cruzar, si tenemos que esperar a que sean 5 sufren inanición los del otro lado
    exclusividad_cuerda[dire].wait()

    // en esta direccion podemos permitir que crucen a lo sumo 5
    cruzar()

    // llegamos y si soy el último le aviso que pueden empezar a cruzar los del otro lado
    if (exclusividad_cuerda[dire].get() == 1)
    {
        // soy el último babuino ==> aviso que puede cruzar los otros
        exclusividad_cuerda[dire + 1 % 2].signal(5)
    }
}

// este esquema sufre inanición porque si llega uno de un extremo y gana la exclusividad de la cuerda y hasta juntar 4 babuinos más se tarda más tiempo de lo que le cuesta sumar 5 al otro extremo, estarían esperando indefinidamente que se libere ese recurso
// para solucionar este problema : podríamos priorizar que cruce el extremo que mǽs babuinos tiene, entonces vamos almacenando una especie de cola donde cada vez que suma un babuino hace la comparación con la # babuinos del otro extremo, si de su lado es mayor, gana la exclusividad de la cuerda, y cruzan 