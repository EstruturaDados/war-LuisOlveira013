/* Exemplo completo demonstrando alocação dinâmica de Territórios e Missões,
 * validação simples de ataques e a função liberarMemoria que libera tudo.
 *
 * Compile: gcc -Wall -Wextra -std=c11 -o war_example war_example.c
 * Execute: ./war_example
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Estrutura que representa um território no jogo */
typedef struct Territory {
    char *name;                 // nome alocado dinamicamente
    int owner;                  // id do jogador dono (0 = neutro / 1..n = jogadores)
    int armies;                 // número de exércitos no território
    int nNeighbors;             // número de vizinhos
    struct Territory **neighbors; // array de ponteiros para territórios vizinhos
} Territory;

/* Estrutura que representa uma missão estratégica */
typedef struct Mission {
    char *description; // descrição alocada dinamicamente
    int targetOwner;   // exemplo: missão relacionada a um dono específico
} Mission;

/* Cria e retorna um território com nome e dono fornecidos */
Territory *criarTerritorio(const char *name, int owner, int armies) {
    Territory *t = malloc(sizeof(Territory));
    if (!t) {
        perror("malloc Territory");
        exit(EXIT_FAILURE);
    }
    t->name = malloc(strlen(name) + 1);
    if (!t->name) {
        perror("malloc name");
        free(t);
        exit(EXIT_FAILURE);
    }
    strcpy(t->name, name);
    t->owner = owner;
    t->armies = armies;
    t->nNeighbors = 0;
    t->neighbors = NULL;
    return t;
}

/* Adiciona um vizinho a um território (cresce dinamicamente o array) */
void adicionarVizinho(Territory *t, Territory *vizinho) {
    t->neighbors = realloc(t->neighbors, sizeof(Territory *) * (t->nNeighbors + 1));
    if (!t->neighbors) {
        perror("realloc neighbors");
        exit(EXIT_FAILURE);
    }
    t->neighbors[t->nNeighbors++] = vizinho;
}

/* Cria e retorna uma missão */
Mission *criarMissao(const char *desc, int targetOwner) {
    Mission *m = malloc(sizeof(Mission));
    if (!m) {
        perror("malloc Mission");
        exit(EXIT_FAILURE);
    }
    m->description = malloc(strlen(desc) + 1);
    if (!m->description) {
        perror("malloc desc");
        free(m);
        exit(EXIT_FAILURE);
    }
    strcpy(m->description, desc);
    m->targetOwner = targetOwner;
    return m;
}

/* Valida se um ataque é permitido:
 * - jogador só pode atacar territórios que NÃO são dele
 * - o território atacante deve ter pelo menos 2 exércitos (ex.: 1 fica para defesa)
 */
int validarAtaque(Territory *from, Territory *to, int playerId) {
    if (!from || !to) return 0;
    if (from->owner != playerId) {
        // só pode atacar se for dono do território atacante
        return 0;
    }
    if (to->owner == playerId) {
        // não pode atacar um próprio território
        return 0;
    }
    if (from->armies < 2) {
        // precisa de ao menos 2 exércitos para realizar ataque (um fica defendendo)
        return 0;
    }
    // opcional: validar se 'to' é vizinho de 'from'
    int found = 0;
    for (int i = 0; i < from->nNeighbors; ++i) {
        if (from->neighbors[i] == to) { found = 1; break; }
    }
    if (!found) return 0;
    return 1; // ataque válido
}

/* Exemplo simples de resolução de combate (aleatório) */
void resolverAtaque(Territory *from, Territory *to) {
    // números aleatórios para exemplo (usar srand(time(NULL)) no main)
    int attackRoll = rand() % 6 + 1; // 1..6
    int defendRoll = rand() % 6 + 1; // 1..6

    printf("Rolagem atacante: %d | defensor: %d\n", attackRoll, defendRoll);
    if (attackRoll > defendRoll) {
        // atacante vence: reduz defender, possivelmente conquista
        to->armies -= 1;
        if (to->armies <= 0) {
            printf("Território %s conquistado!\n", to->name);
            to->owner = from->owner;
            // mover pelo menos 1 exército do atacante para o território conquistado
            from->armies -= 1;
            to->armies = 1;
        } else {
            printf("%s perde 1 exército (restam %d)\n", to->name, to->armies);
        }
    } else {
        // defensor vence
        from->armies -= 1;
        printf("%s perde 1 exército (restam %d)\n", from->name, from->armies);
    }
}

/* Função pedida: libera toda a memória alocada para territórios e missões.
 *
 * territories: array de ponteiros para Territory (size = nTerritories)
 * missions: array de ponteiros para Mission (size = nMissions)
 *
 * Importante: a função assume que todos os ponteiros são válidos ou NULL.
 */
void liberarMemoria(Territory **territories, int nTerritories, Mission **missions, int nMissions) {
    // Liberar cada território
    if (territories) {
        for (int i = 0; i < nTerritories; ++i) {
            Territory *t = territories[i];
            if (!t) continue;
            // liberar string do nome
            if (t->name) {
                free(t->name);
                t->name = NULL;
            }
            // liberar array de vizinhos (somente o array, não os territórios apontados,
            // pois eles são liberados pela lista 'territories' principal)
            if (t->neighbors) {
                free(t->neighbors);
                t->neighbors = NULL;
            }
            // finalmente liberar o struct Territory
            free(t);
            territories[i] = NULL;
        }
        // liberar o array que continha os ponteiros para Territory
        free(territories);
        territories = NULL;
    }

    // Liberar cada missão
    if (missions) {
        for (int j = 0; j < nMissions; ++j) {
            Mission *m = missions[j];
            if (!m) continue;
            if (m->description) {
                free(m->description);
                m->description = NULL;
            }
            free(m);
            missions[j] = NULL;
        }
        // liberar o array que continha os ponteiros para Mission
        free(missions);
        missions = NULL;
    }

    // Observação: não há retorno. Após chamar liberarMemoria, todos os dados
    // alocados dinamicamente serão liberados.
}

/* Exemplo de uso */
int main(void) {
    srand((unsigned)time(NULL)); // gerar números aleatórios (boa prática)

    // --- Criar alguns territórios dinamicamente ---
    int nTerritories = 3;
    Territory **territories = malloc(sizeof(Territory *) * nTerritories);
    if (!territories) { perror("malloc territories"); exit(EXIT_FAILURE); }

    territories[0] = criarTerritorio("Amazônia", 1, 5);
    territories[1] = criarTerritorio("Sertão", 2, 3);
    territories[2] = criarTerritorio("Litoral", 0, 2);

    // criar vizinhanças (grafo simples)
    adicionarVizinho(territories[0], territories[1]); // Amazônia <-> Sertão
    adicionarVizinho(territories[1], territories[0]);
    adicionarVizinho(territories[1], territories[2]); // Sertão <-> Litoral
    adicionarVizinho(territories[2], territories[1]);

    // --- Criar missões ---
    int nMissions = 2;
    Mission **missions = malloc(sizeof(Mission *) * nMissions);
    if (!missions) { perror("malloc missions"); liberarMemoria(territories, nTerritories, NULL, 0); exit(EXIT_FAILURE); }

    missions[0] = criarMissao("Conquistar 3 territórios da região Norte", 0);
    missions[1] = criarMissao("Eliminar jogador 2", 2);

    // --- Exemplo de validação e ataque ---
    Territory *from = territories[0]; // Amazônia (owner=1)
    Territory *to = territories[1];   // Sertão (owner=2)
    int playerId = 1;

    printf("Tentativa de ataque de %s para %s pelo jogador %d\n", from->name, to->name, playerId);
    if (validarAtaque(from, to, playerId)) {
        printf("Ataque válido. Resolvendo combate...\n");
        resolverAtaque(from, to);
    } else {
        printf("Ataque inválido: só é permitido atacar territórios inimigos vizinhos com exércitos suficientes.\n");
    }

    // --- Final: liberar toda a memória antes de sair ---
    liberarMemoria(territories, nTerritories, missions, nMissions);

    printf("Memória liberada com sucesso. Encerrando.\n");
    return 0;
}