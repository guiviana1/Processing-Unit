#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>
#include <string>
#include <iostream>

const int CAPACIDADE = 5;

std::mutex mtxImpressao;

void imprimir(const std::string& msg) {
    std::lock_guard<std::mutex> lock(mtxImpressao);
    std::cout << msg << std::endl;
}

struct Tarefa {
    int id;
    int produtora;
};

class FilaTarefas {
private:
    std::queue<Tarefa> fila;
    std::mutex mtx;
    std::condition_variable cvNaoCheia;
    std::condition_variable cvNaoVazia;
    bool encerrada = false;

public:
    // Insere uma tarefa na fila, esperando enquanto ela estiver cheia
    void inserir(Tarefa t) {
        std::unique_lock<std::mutex> lock(mtx);
        if (fila.size() == CAPACIDADE) {
            imprimir("[Produtora " + std::to_string(t.produtora) + "] fila cheia, aguardando");
        }
        while (fila.size() == CAPACIDADE) {
            cvNaoCheia.wait(lock);
        }
        fila.push(t);
        imprimir("[Produtora " + std::to_string(t.produtora) + "] produziu tarefa " +
                 std::to_string(t.id) + " (fila: " + std::to_string(fila.size()) + "/" +
                 std::to_string(CAPACIDADE) + ")");
        cvNaoVazia.notify_one();
    }

    // Retira uma tarefa da fila; devolve false quando a fila esta vazia e encerrada
    bool remover(Tarefa& t, int consumidora) {
        std::unique_lock<std::mutex> lock(mtx);
        if (fila.empty() && !encerrada) {
            imprimir("[Consumidora " + std::to_string(consumidora) + "] fila vazia, aguardando");
        }
        while (fila.empty() && !encerrada) {
            cvNaoVazia.wait(lock);
        }
        if (fila.empty()) {
            return false;
        }
        t = fila.front();
        fila.pop();
        imprimir("[Consumidora " + std::to_string(consumidora) + "] retirou tarefa " +
                 std::to_string(t.id) + " (fila: " + std::to_string(fila.size()) + "/" +
                 std::to_string(CAPACIDADE) + ")");
        cvNaoCheia.notify_one();
        return true;
    }

    // Marca a fila como encerrada e acorda todas as consumidoras
    void encerrar() {
        std::lock_guard<std::mutex> lock(mtx);
        encerrada = true;
        cvNaoVazia.notify_all();
    }
};
