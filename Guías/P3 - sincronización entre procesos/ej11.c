atomic bool libres = false;
atomic bool luz = false;

void PrisioneroCero() {
    int contador = 0;
    while (!libres) {
    // Esperar que la sala esté libre
        sala.wait()
        if (!luz) {
            luz = true;
            contador++;
            if (contador == N) libres = true;
        }
        sala.signal()
    }
}
    // Resto de los prisioneros
void RestoDeLosPrisionero(int i) {
    bool entreASala = false;
    while (!libres) {
        sala.wait()
        if (luz && !entreASala) {
            entreASala = true
            luz = false;
        }
        sala.signal()
    }
}