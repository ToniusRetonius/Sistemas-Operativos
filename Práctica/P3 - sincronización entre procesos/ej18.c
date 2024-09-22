// lavarropas que sólo lava cuando estǽ lleno == 10 prendas
semaforo podes_ingresar = sem(0)
semaforo estoy_ingresando = sem(1)
semaforo hay_lugar = sem(10)
semaforo puedo_salir = sem(0)

void lavarropas(){
    while (1)
    {
        // tiene que cargarse
        hay_lugar.wait()
        estoyListo()
        // le permite ingresar
        podes_ingresar.signal()
        // ya entró .. chequeamos que no esté lleno
        if (hay_lugar.get() == 1)
        {
            // si está lleno ..
            // lavamos
            lavar()
            // retiramos la ropa toda junta
            puedo_salir.signal(10)
            // ya estǽ vacío 
            hay_lugar = sem(10)
        }
        else
        {
            // si puede alojar más ropa ...
            continue
        }
    }

}

void prenda(int i){
    // puede ingresar al lavarropa <==> el lavarropas le avisa que está_listo
    podes_ingresar.wait()
    // no pueden entrar muchas simultaneamente
    estoy_ingresando.wait()
    entroAlLavarropas()
    estoy_ingresando.signal()
    // mientras esté lavando deberǽ esperar la finalización del lavado
    puedo_salir.wait()
    saquenMeDeAqui()

    // fin
    exit(0)
}