#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BRANCO 1
#define CINZA 2
#define PRETO 3
#define INFINITO 999999

typedef struct Lista Lista;

typedef struct Vertice
{
    int id;
    int grau;
    double closeness;
    double betweenness;
    double clustering;
    Lista *listaAdj;

    //BFS
    int cor;
    int dist;

    //Lista, fila
    struct Vertice *proximo;
}Vertice;

struct Lista
{
    Vertice *inicio, *fim;
};

typedef struct Fila
{
    Vertice *inicio, *fim;
}Fila;

typedef struct Grafo
{
    Vertice *vert;
    int nVerts;
}Grafo;

void InicializarGrafo(Grafo *g, int nVerts);
void InserirVertice(Grafo *g, Vertice v);
void InserirAresta(Grafo *g, int v1, int v2);
int BFS(Grafo *g, int vInicial);
void FloydWarshall(Grafo *g, int *M[]);
void Betweenness(Grafo *g);
void Clustering(Grafo *g);

void InicializarLista(Lista *lista);
void InserirLista(Lista *lista, Vertice v);

int Vazia(Lista *lista);
void Quicksort(Vertice *all, int esq, int dir, int esc);

void InicializarFila(Fila *f);
void Enfileirar(Fila *f, Vertice v);
int FilaVazia(Fila *f);
Vertice Desenfileirar(Fila *f);

int main(int argc, char* argv[])
{
    int i, j, k;

    Grafo g;

    Vertice v;
    char wordAux[20];
    int nVerts;

    FILE *f = fopen(argv[1], "r");

    fscanf(f, "%s", wordAux); //Joga fora "*Vertices"
    fscanf(f, "%d", &nVerts);

    InicializarGrafo(&g, nVerts);

    for (i = 0; i < nVerts; i++)
    {
        fscanf(f, "%d", &v.id);
        v.id--;

        InserirVertice(&g, v);

        fscanf(f, "%s", wordAux); //"#id"
    }

    fscanf(f, "%s", wordAux); //*Arcs

    int id1, id2;
    while (fscanf(f, "%d %d", &id1, &id2) != EOF)
        InserirAresta(&g, id1-1, id2-1);

    double closeness;
    for (i = 0; i < nVerts; i++)
    {
        closeness = BFS(&g, i);
        closeness /= nVerts;
        closeness = 1/closeness;

        g.vert[i].closeness = closeness;
    }

    Betweenness(&g);
    Clustering(&g);

    FILE *tudo = fopen("grafo.txt", "w");

    fprintf(tudo, "VERTICE\t\tGRAU\tCLOSENESS\tBETWEENNESS\tCLUSTERING\n");
    for (i = 0; i < g.nVerts; i++)
        fprintf(tudo, "%d\t\t%d\t%.2f\t\t%.2f\t\t%.2f\n", g.vert[i].id+1, g.vert[i].grau, g.vert[i].closeness, g.vert[i].betweenness, g.vert[i].clustering);
    fclose(tudo);

    FILE *grau = fopen("grau.txt", "w"); //Salva as informacoes manipuladas no novo arquivo txt
    FILE *clos = fopen("closeness.txt", "w");
    FILE *betw = fopen("betweenness.txt", "w");
    FILE *clus = fopen("clustering.txt", "w");

    Quicksort(g.vert, 0, g.nVerts-1, 1);
    fprintf(grau, "VERTICE\tGRAU\n");
    for (i = 0; i < g.nVerts; i++)
        fprintf(grau, "%d\t%d\n", g.vert[i].id+1, g.vert[i].grau);
    fclose(grau);

    Quicksort(g.vert, 0, g.nVerts-1, 2);
    fprintf(clos, "VERTICE\tCLOSENESS\n");
    for (i = 0; i < g.nVerts; i++)
        fprintf(clos, "%d\t%.2f\n", g.vert[i].id+1, g.vert[i].closeness);
    fclose(clos);

    Quicksort(g.vert, 0, g.nVerts-1, 3);
    fprintf(betw, "VERTICE\tBETWEENNESS\n");
    for (i = 0; i < g.nVerts; i++)
        fprintf(betw, "%d\t%f\n", g.vert[i].id+1, g.vert[i].betweenness);
    fclose(betw);

    Quicksort(g.vert, 0, g.nVerts-1, 4);
    fprintf(clus, "VERTICE\tCLUSTERING\n");
    for (i = 0; i < g.nVerts; i++)
        fprintf(clus, "%d\t%.2f\n", g.vert[i].id+1, g.vert[i].clustering);
    fclose(clus);

	return 0;
}

void InicializarGrafo(Grafo *g, int nVerts)
{
    g->nVerts = 0;
    g->vert = (Vertice*) malloc(nVerts * sizeof(Vertice));
}

void InserirVertice(Grafo *g, Vertice v)
{
    v.listaAdj = (Lista*) malloc(sizeof(Lista));
    InicializarLista(v.listaAdj);

    v.grau = 0;
    v.betweenness= 0;
    g->vert[g->nVerts] = v;
    g->nVerts++;
}

void InserirAresta(Grafo *g, int v1, int v2)
{
    g->vert[v1].grau++;
    g->vert[v2].grau++;
    InserirLista(g->vert[v1].listaAdj, g->vert[v2]);
    InserirLista(g->vert[v2].listaAdj, g->vert[v1]);
}

void InicializarLista(Lista *lista)
{
    lista->inicio = NULL;
    lista->fim = NULL;
}

void InicializarFila(Fila *f)
{
    f->inicio = NULL;
    f->fim = NULL;
}

void InserirLista(Lista *lista, Vertice v)
{
    Vertice *aux = (Vertice*) malloc(sizeof(Vertice));

    *aux = v;

    if (Vazia(lista))
    {
        lista->inicio = aux;
    }
    else
    {
        lista->fim->proximo = aux;
    }

    aux->proximo = NULL;

    lista->fim = aux;
}

int Vazia(Lista *lista)
{
    if (lista->inicio == NULL)
        return 1;
    else
        return 0;
}

void Quicksort(Vertice *all, int esq, int dir, int esc)
{
    if (esc == 1)
    {
        int i = esq, j = dir;
        int pivot = (i + j)/2;
        int item;
        item = all[pivot].grau;

        do
        {
            while (item < all[i].grau) i++;
            while (item > all[j].grau) j--;

            if (i <= j)
            {
                Vertice aux = all[i];
                all[i] = all[j];
                all[j] = aux;
                i++;
                j--;
            }
        }while (i <= j);

        if (esq < j) Quicksort(all, esq, j, esc);
        if (i < dir) Quicksort(all, i, dir, esc);
    }
    if (esc == 2)
    {
        int i = esq, j = dir;
        int pivot = (i + j)/2;
        double item;
        item = all[pivot].closeness;

        do
        {
            while (item < all[i].closeness) i++;
            while (item > all[j].closeness) j--;

            if (i <= j)
            {
                Vertice aux = all[i];
                all[i] = all[j];
                all[j] = aux;
                i++;
                j--;
            }
        }while (i <= j);

        if (esq < j) Quicksort(all, esq, j, esc);
        if (i < dir) Quicksort(all, i, dir, esc);
    }
    if (esc == 3)
    {
        int i = esq, j = dir;
        int pivot = (i + j)/2;
        double item;
        item = all[pivot].betweenness;

        do
        {
            while (item < all[i].betweenness) i++;
            while (item > all[j].betweenness) j--;

            if (i <= j)
            {
                Vertice aux = all[i];
                all[i] = all[j];
                all[j] = aux;
                i++;
                j--;
            }
        }while (i <= j);

        if (esq < j) Quicksort(all, esq, j, esc);
        if (i < dir) Quicksort(all, i, dir, esc);
    }
    if (esc == 4)
    {
        int i = esq, j = dir;
        int pivot = (i + j)/2;
        double item;
        item = all[pivot].clustering;

        do
        {
            while (item < all[i].clustering) i++;
            while (item > all[j].clustering) j--;

            if (i <= j)
            {
                Vertice aux = all[i];
                all[i] = all[j];
                all[j] = aux;
                i++;
                j--;
            }
        }while (i <= j);

        if (esq < j) Quicksort(all, esq, j, esc);
        if (i < dir) Quicksort(all, i, dir, esc);
    }
}

int BFS(Grafo *g, int vInicial)
{
    int i;
    int dists[g->nVerts];
    int cores[g->nVerts];
    int total = 0;

    for (i = 0; i < g->nVerts; i++)
    {
        cores[i] = BRANCO;
        dists[i] = INFINITO;
    }

    Fila f;
    f.inicio = NULL;
    f.fim = NULL;

    cores[vInicial] = CINZA;
    dists[vInicial] = 0;

    Vertice v;
    v.id = vInicial;
    v.cor = CINZA;
    v.dist = 0;

    Enfileirar(&f, v);

    while(!FilaVazia(&f))    {
        Vertice v2 = Desenfileirar(&f);
        cores[v2.id] = PRETO;
        total += v2.dist;

        Vertice *aux = g->vert[v2.id].listaAdj->inicio;

        while (aux != NULL)
        {
            if (cores[aux->id] == BRANCO)
            {
                cores[aux->id] = CINZA;
                dists[aux->id] = dists[v2.id]+1;

                v.id = aux->id;
                v.cor = CINZA;
                v.dist = dists[v2.id]+1;
                Enfileirar(&f, v);
            }
            aux = aux->proximo;
        }
    }

    return total;
}

void FloydWarshall(Grafo *g, int *M[])
{
    int i, j, k;
    for (i = 0; i < g->nVerts; i++)
    {
        Vertice *aux = g->vert[i].listaAdj->inicio;
        while (aux != NULL)
        {
            M[i][aux->id] = 1;

            aux = aux->proximo;
        }
    }

    for (i = 0; i < g->nVerts; i++)
        for (j = 0; j < g->nVerts; j++)
            for (k = 0; k < g->nVerts; k++)
                if (M[j][i] + M[i][k] < M[j][k])
                    M[j][k] = M[j][i] + M[i][k];
}

void Betweenness(Grafo *g)
{
    int i, j, k, l;
    int *M[g->nVerts];
    int nMin[g->nVerts][g->nVerts];
    double min[g->nVerts];

    for (i = 0; i < g->nVerts; i++)
    {
        min[i] = 0;
        M[i] = (int*) malloc(g->nVerts * sizeof(int));
        for (j = 0; j < g->nVerts; j++)
        {
            nMin[i][j] = 0;
            M[i][j] = INFINITO;
            if (i == j) M[i][j] = 0;
        }
    }

    FloydWarshall(g, M);

    int ***passa = (int***) malloc(g->nVerts * sizeof(int**));
    for (i = 0; i < g->nVerts; i++)
    {
        passa[i] = (int**) malloc(g->nVerts * sizeof(int*));
        for (j = 0; j < g->nVerts; j++)
        {
            passa[i][j] = (int*) malloc(g->nVerts * sizeof(int));
            for (k = 0; k < g->nVerts; k++)
                passa[i][j][k] = 0;
        }
    }

    for (i = 0; i < g->nVerts; i++)
    {
        for (j = 0; j < g->nVerts; j++)
        {
            for (k = 0; k < g->nVerts; k++)
            {
                if (M[j][i] + M[i][k] == M[j][k] && j != k && M[j][k] != 1)
                {
                    if (i != k && i != j)
                    {
                        passa[i][j][k] = 1;

                        for (l = 0; l < g->nVerts; l++)
                            if (l != k && l != j && l != i)
                                if (passa[l][j][i] || passa[l][i][k])
                                    passa[l][j][k]++;
                        nMin[j][k]++;
                    }
                    if (M[j][k] == 1) nMin[j][k] = 1;
                }
            }
        }
    }


    for (i = 0; i < g->nVerts; i++)
    {
        for (j = 0; j < g->nVerts; j++)
        {
            for (k = 0; k < g->nVerts; k++)
            {
                if (nMin[j][k] != 0)
                    min[i] += (float)passa[i][j][k] / (float)nMin[j][k];
            }
        }
        g->vert[i].betweenness = min[i];
    }
}

void Clustering(Grafo *g)
{
    int i;
    int nTriang[g->nVerts];

    for (i = 0; i < g->nVerts; i++)
    {
        nTriang[i] = 0;

        Vertice *aux = g->vert[i].listaAdj->inicio;

        while (aux != NULL)
        {
            Vertice *aux2 = g->vert[aux->id].listaAdj->inicio;

            while (aux2 != NULL)
            {
                Vertice *aux3 = g->vert[aux2->id].listaAdj->inicio;

                while (aux3 != NULL)
                {
                    if (aux3->id == i)
                        nTriang[i]++;

                    aux3 = aux3->proximo;
                }

                aux2 = aux2->proximo;
            }

            aux = aux->proximo;
        }

        nTriang[i] /= 2;

        if (g->vert[i].grau > 1)
            g->vert[i].clustering = (float)(2*nTriang[i])/(float)(g->vert[i].grau*(g->vert[i].grau-1));
        else
            g->vert[i].clustering = 0;
    }
}

void Enfileirar(Fila *f, Vertice v)
{
    Vertice *aux = (Vertice*) malloc(sizeof(Vertice));
    *aux = v;

    aux->proximo = NULL;

    if (f->inicio == NULL)
        f->inicio = aux;

    if (f->fim != NULL)
        f->fim->proximo = aux;

    f->fim = aux;
}

int FilaVazia(Fila *f)
{
    if (f->inicio == NULL) return 1;
    return 0;
}

Vertice Desenfileirar(Fila *f)
{
    Vertice v = *f->inicio;

    Vertice *aux = f->inicio->proximo;

    f->inicio = aux;

    return v;
}
