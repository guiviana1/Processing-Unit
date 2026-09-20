#pragma once
#include <mutex>

const int NUM_RECURSOS = 5;

class Recursos {
private:
    std::mutex mtx[NUM_RECURSOS];

public:
    // Bloqueia os dois recursos sempre na ordem menor -> maior, o que evita deadlock
    void bloquear(int a, int b) {
        int menor = a;
        int maior = b;
        if (a > b) {
            menor = b;
            maior = a;
        }
        mtx[menor].lock();
        mtx[maior].lock();
    }

    // Libera os dois recursos na ordem inversa do bloqueio
    void liberar(int a, int b) {
        int menor = a;
        int maior = b;
        if (a > b) {
            menor = b;
            maior = a;
        }
        mtx[maior].unlock();
        mtx[menor].unlock();
    }
};
