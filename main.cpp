#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include "fila.h"
#include "recursos.h"

const int NUM_PRODUTORAS = 3;
const int NUM_CONSUMIDORAS = 5;
const int TAREFAS_POR_PRODUTORA = 5;
const int TEMPO_PROCESSAMENTO_MS = 400;

FilaTarefas fila;
Recursos recursos;
int processadas[NUM_CONSUMIDORAS] = {0};

// Produz as tarefas da produtora p e as insere na fila
void produtora(int p) {
    for (int k = 0; k < TAREFAS_POR_PRODUTORA; k++) {
        Tarefa t;
        t.id = p * TAREFAS_POR_PRODUTORA + k + 1;
        t.produtora = p;
        fila.inserir(t);
        std::this_thread::sleep_for(std::chrono::milliseconds(100 * (p + 1)));
    }
    imprimir("[Produtora " + std::to_string(p) + "] finalizou");
}

// Retira tarefas da fila e as processa usando dois recursos ate a fila encerrar
void consumidora(int c) {
    int r1 = c;
    int r2 = (c + 1) % NUM_RECURSOS;
    int menor = r1 < r2 ? r1 : r2;
    int maior = r1 < r2 ? r2 : r1;
    std::string recursosTexto = std::to_string(menor) + " e " + std::to_string(maior);
    Tarefa t;
    while (fila.remover(t, c)) {
        recursos.bloquear(r1, r2);
        imprimir("[Consumidora " + std::to_string(c) + "] processando tarefa " +
                 std::to_string(t.id) + " com recursos " + recursosTexto);
        std::this_thread::sleep_for(std::chrono::milliseconds(TEMPO_PROCESSAMENTO_MS));
        imprimir("[Consumidora " + std::to_string(c) + "] concluiu tarefa " +
                 std::to_string(t.id) + " e liberou recursos " + recursosTexto);
        recursos.liberar(r1, r2);
        processadas[c]++;
    }
    imprimir("[Consumidora " + std::to_string(c) + "] finalizou");
}

int main() {
    std::vector<std::thread> produtoras;
    std::vector<std::thread> consumidoras;
    produtoras.reserve(NUM_PRODUTORAS);
    consumidoras.reserve(NUM_CONSUMIDORAS);

    for (int c = 0; c < NUM_CONSUMIDORAS; c++) {
        consumidoras.emplace_back(consumidora, c);
    }
    for (int p = 0; p < NUM_PRODUTORAS; p++) {
        produtoras.emplace_back(produtora, p);
    }

    for (auto& produtoraThread : produtoras) {
        produtoraThread.join();
    }
    fila.encerrar();

    for (auto& consumidoraThread : consumidoras) {
        consumidoraThread.join();
    }

    std::string resumo = "Resumo: ";
    int total = 0;
    for (int c = 0; c < NUM_CONSUMIDORAS; c++) {
        resumo += "consumidora " + std::to_string(c) + " = " + std::to_string(processadas[c]);
        if (c < NUM_CONSUMIDORAS - 1) {
            resumo += ", ";
        }
        total += processadas[c];
    }
    resumo += " | total = " + std::to_string(total);
    imprimir(resumo);

    return 0;
}
