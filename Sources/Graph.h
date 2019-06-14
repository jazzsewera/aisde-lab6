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

#ifndef GRAPH_H
#define GRAPH_H

#include <fstream>
#include "RNG.h"

// Nieskonczoność (jak widać :)
#define INF (-1)

// Graf nieskierowany. Implementacja tablicowa 
class Graph {
	private:
		int	 V;        // Liczba wierzchołków.
		int    E;        // Liczba krawędzi.
		long   tabsize;  // Liczba krawędzi pełnego grafu prostego o V wierzchołkach.
		long   cost;     // Suma wag krawędzi łączących oba podzbiory, czyli tzw. przekroju. 
		long	*weights;  // Tablica wag krawędzi.
		bool  *subset;   // Tablica flag okresląjacych przynależności poszczególnych 
		                 // wierzchołków do jednego z dwóch podzbiorów.
		long   getK(int v, int w) const; // Zwraca indeks tablicy, pod którym znajduje się waga krawędzi łączącej wierzchołki 'v' i 'w'.

	public:
		Graph(std::ifstream& infile);
		Graph(const Graph& g);
		~Graph(void);
 		int   getV(void) const;
 		int   getE(void) const;
 		long  getWeight(int v, int w) const;
		int   getSubset(int v) const;
 		bool  swapVertices(int v, int w);  // Zamienia miejscami wierzchołki należące do różnych podzbiorów.
		long  getObjective(void) const;    // Zwraca wartość funkcji celu.
 		void  print (void) const;          // Wypisuje na ekranie macierz sąsiedztwa grafu.
 		void  partitionEvenOdd(void);
		void  partitionRandomly(RNG&);
};

#endif // GRAPH_H