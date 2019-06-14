/**********************************************************************************
*                                     GRAPH
*
*  Klasa reprezentująca ważony graf prosty (nieskierowany).
*  Wykorzystywana przez algorytmy biskecji.
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 1 czerwca 2017
*
***********************************************************************************/

#include <iostream>
#include <fstream>
#include <cstdlib>

#include "Graph.h"

// Tworzy graf prosty (nieskierowany) na podstawie opisu wczytanego z pliku 'infile'
Graph::Graph(std::ifstream& infile) {
	int  v, w;    // Numery wierzchołków
	long weight;  // Waga krawędzi

	V = 0;				// Liczba wierzchołków
	E = 0;				// Liczba krawędzi
	tabsize = 0;

  	// Zliczamy wierzchołki.
	while (infile >> v >> w >> weight) {
		if (v > V) V = v;
		if (w > V) V = w;
	}
	V++;		// Numery wierzchołków powinny zaczynać się od zera.

	if (V % 2) {
		std::cout << " TRAGEDIA! Graf powinien mieć parzystą liczbę wierzchołków, a ten ma ich " << V << std::endl;
		exit (EXIT_FAILURE);
	}

	tabsize = V*(V-1)/2;
	weights = new long[tabsize];
	subset  = new bool[V];

	for (int i = 0; i < tabsize; i++)
		weights[i] = INF;

	// Co drugi wierzchołek umieszczamy w podzbiorze 'false',
 	// co drugi - w podzbiorze 'true'.
	for (int i = 0; i < V;) {
		subset[i++]	= false;
		subset[i++] = true;
	}

	// Wracamy do początku pliku.
	infile.clear();
	infile.seekg(0);

	// Dodajemy krawędzie do grafu.
	while (infile >> v >> w >> weight) {
		weights[getK(v, w)] = weight;
		E++;
	}

	cost = getObjective();
}


// Konstruktor kopiujący
Graph::Graph(const Graph& g) : V(g.V), tabsize(g.tabsize), cost(g.cost) {
	weights = new long[tabsize];
	subset  = new bool[V];

	for (int i = 0; i < tabsize; i++)
		weights[i] = g.weights[i];

	for (int i = 0; i < V; i++)
		subset[i] = g.subset[i];
}


Graph::~Graph(void) {
	delete [] weights;
	delete [] subset;
}


// Na podstawie numerów wierzchołków 'v' i 'w' zwraca indeks tablicy,
// pod którym można znaleźć wagę krawędzi łączącej 'v' z 'w'
// (graf NIE jest skierowany).
long Graph::getK(int v, int w) const {
	if (v >= V or w >= V or v == w)
		return -1;
	if (w > v)
		return (long)(v*(V-0.5*v-1.5) + w - 1);
	if (w < v)
		return (long)(w*(V-0.5*w-1.5) + v - 1);
	return -1;
}


// Zwraca wartość funkcji celu. Jest nią sumaryczna waga
// przekroju grafu, czyli krawędzi łączacych obie jego częsci.
long Graph::getObjective(void) const {
	long partitionCost = 0;
	long weight;

	for (int v = 0; v < V; v++) {
		for (int w = v; w < V; w++) {
			if (subset[v] != subset[w]) {
				weight = weights[getK(v, w)];
				if (weight != INF)
					partitionCost += weight;
			}
		}
	}
	return partitionCost;
}


// Zwraca wagę krawedzi laczącej wierzchołki 'v' i 'w'.
// Graf NIE jest skierowany.
long Graph::getWeight(int v, int w) const {
	if (v >= 0 and w >= 0 and v < V and w < V)
		return weights[getK(v, w)];
	else
		return INF;
}


// Zwraca podzbiór, do którego należy wierzchołek 'v'
int Graph::getSubset(int v) const {
	if (v >= 0 and v < V)
		return subset[v];
	else
		return INF;
}


// Zwraca liczbę wierzchołków grafu.
int Graph::getV(void) const { return V; }


// Zwraca liczbę krawędzi grafu.
int Graph::getE(void) const { return E; }


// Zamienia miejscami wierzchołki 'v' i 'w' należące do różnych podzbiorów
bool Graph::swapVertices(int v, int w) {
	if (subset[v] != subset[w]) {
		subset[v] = not subset[v];
		subset[w] = not subset[w];
		return true;
	}
	return false;
}


void Graph::print(void) const {
	long weight;
 	for (int i = 0; i < V; i++) {
 		for (int j = i+1; j < V; j++) {
 			weight = weights[getK(i, j)];
 			if (weight != INF)
				std::cout << weight << "\t";
			else
				std::cout << "-\t";
		}
		std::cout << std::endl;
	}
}


// Jeżeli odprężanie ma byc uruchamiane tylko raz, podział zbioru wierzchołków
// na podzbiory jest deterministyczny. Wierzchołki o nieparzystych indeksach
// trafiają do jednego podzbioru, zaś te o parzystych do drugiego.
void Graph::partitionEvenOdd(void) {
	for (int i = 0; i < V-1; i += 2) {
		subset[i]   = true;
		subset[i+1] = false;
	}
}


// Jezeli odprężanie ma być uruchamiane wielokrotnie,
// podział zbioru wierzchołków na podzbiory musi być losowy.
void Graph::partitionRandomly(RNG& randomVertexSelector) {
	// Najpierw dzielimy regularnie...
	partitionEvenOdd();
	// ... a potem mieszamy.
	for (int k = 0; k < V/2; k++) {
		int i = randomVertexSelector.randomInt();
		int j = randomVertexSelector.randomInt();
		if (subset[i] != subset[j]) {
			subset[i] = not subset[i];
			subset[j] = not subset[j];
		}
	}
}
