// se tienen N-heladeras
semaforo heladeras[N+1]     // pongo +1 porque como es secuencial el orden de llenado, queremos que todas le avisen a la que sigue. Todos en 0 menos el primero
heladeras[0].signal()

// tenemos que cumplir que : cada birra ingresa de a 1. Entonces, no puede haber CONTENCIÓN ( es decir, no debe suceder que ingresan más de una a la vez)
semaforo estoy_entrando = sem(0)
// se tiene que cumplir la secuencia específica en que la heladera se abre, ingresa la cerveza y luego se cierra y estas 3 cosas son excluyentes
semaforo puerta_abierta = sem(0)
semaforo puerta_cerrada = sem(0)

// tenemos que controlar cierta #botellas y #porrones ==> cada cerveza que conoce su TIPO tendrá que esperar si puede o no agregarse
// serán aumentadas a 15 botellas y 10 porrones por la heladera que le toque llenarse
semaforo lugar_para_mi[2]   // ambos en 0

void heladera(int i){
    // espera a que llegue su turno de llenarse
    heladeras[i].wait()
    // tiene que lanzar los signals avisando que hay lugar para cada tipo de cerveza
    lugar_para_mi[0].signal(15)   // botella
    lugar_para_mi[1].signal(10)   // porrón

    // qué cantidad de birra soporta ?
    int capacidad = 25;
    while (capacidad > 0)
    {
        // ya la birra que tiene permiso llega hasta acá ==> hay que dejarla entrar
        estoy_entrando.wait()
        // le abrimos la puerta
        abrir_puerta()
        puerta_abierta.signal()
        // sostenemos con una mano la puerta y metemos la birra y esperamos que haga meMetenEnLaHeladera()
        puerta_cerrada.wait()
        // ya tenemos una birra mǽs dentro de la heladera
        capacidad--
    }

    // una vez llena la heladera enfriamos a mǽxima velocidad
    enfriamientoSuperior()
    // tenemos que habilitar la heladera que sigue
    heladeras[i+1].signal()   
}

void cerveza(int i, int tipo_envase){
    // llego al bar
    llegar()
    // puedo entrar a alguna heladera ??
    lugar_para_mi[tipo_envase]
    // aviso a la heladera que voy a entrar así puede ...
    estoy_entrando.signal()
    // abrirme la heladera
    puerta_abierta.wait()
    // ya estoy dentro
    meMetenEnLaHeladera()
    // avisamos que ya puede cerrar la puerta
    puerta_cerrada.signal()
}
