int cant_especies = n;

// una puerta por especie, dos colas por el sexo
sem_t puertas[cant_especies][2];

for (int j = 0; j < n; j++)
{
    puertas[j][0] = 0;
    puertas[j][1] = 0;
}


void animal(int especie, int sexo){
    int sexo_opuesto; 
    if (sexo == 0)
    {
        sexo_opuesto = 1;
    }
    else
    {
        sexo_opuesto = 0;
    }
    
    signal(puertas[especie][sexo_opuesto]);
    wait(puertas[especie][sexo]);
    entrar(especie);
}


