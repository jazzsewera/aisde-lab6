/**********************************************************************************
*                                     GRAPHGEN
*
*	Generator losowych grafow NIESKIEROWANYCH.
*	Tworzony jest graf o zadanej liczbie 'V' wierzchołków i 'E' krawędzi.
*	Stopnie wierzchołków maja wartosci w przyblizeniu rowne 2E/V.
*	Wagi wierzchołków to wartosci bezwzgledne zmiennej losowej
*	o rozkladzie normalnym o wartosci oczekiwanej 0 i odchyleniu standardowym STDDEV.
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 1 lipca 2015
*
***********************************************************************************/

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <climits>

const int STDDEV = 100;
const int SEED   = 300499;	// TU WPISUJEMY NUMER ALBUMU


class Edge {
	public:
		Edge(long _v, long _w, int _weight): v(_v), w(_w), weight(_weight) {}
		long v, w;    // Numery wierzchołków
  		int  weight;  // Waga krawędzi
};

// Lista sąsiedztwa, czyli tablica list obiektów typu Edge
class Graph {
	private:
		long    V, E;      // Ile wierzchołków? Ile krawędzi (docelowo)?
		long    edgesLeft; // Ile chwilowo krawędzi w strukturze?
		Edge  **edges;
		bool   *include;	// Które krawędzie grafu pełnego pozostawic w końcowym grafie
		long    getK(int v, int w); // Indeks tablicy, pod którym znajduje się
		                            // waga krawędzi (v,w).
	public:
		Graph(long V, long E); // Konstruktor grafu o V wierzchołkach i E krawędziach
		~Graph(void);
		long getV(void) { return V; }
		long getActualE(void) { return edgesLeft; }
		long RemoveRandomEdges(long);
		void Save(char *file);
};

// Graf prosty
Graph::Graph(long _V, long _E) {
	V = _V;
	E = _E;
	edgesLeft = V*(V-1)/2;
  	edges   = new Edge*[edgesLeft];
  	include = new bool[edgesLeft];
  	double  x1, x2, r, x;

	if (E > edgesLeft)
		E = edgesLeft;

  	for (int i = 0; i < edgesLeft; i++)
  		include[i] = true;

  	// Przypisywanie krawędziom losowych wag. Sa one wartościami bezwzględnymi
  	// zmiennej pseudolosowej o rozkładzie normalnym, generowanej
  	// za pomocą algorytmu Marsaglii i Braya
 	for (long v = 0; v < V-1; v++) {
 		for (long w = v+1; w < V; w++) {
 			do {
 				x1 = 2*rand()/(double)RAND_MAX-1;
 				x2 = 2*rand()/(double)RAND_MAX-1;
 				r = x1*x1 + x2*x2;
			} while (r >= 1);
			r = sqrt((-2*log(r))/r);
			x = x1*r;
			edges[getK(v, w)] = new Edge(v, w, (int)(STDDEV*fabs(x)+1));
			//edges[getK(v, w)] = new Edge(v, w, (int)(fabs(STDDEV*(x+3))+1));
		}
	}

	// Z grafu wycinamy tyle krawędzi, by zostało ich E
	while (edgesLeft > E)
		RemoveRandomEdges(edgesLeft - E);
}

Graph::~Graph(void) {
	delete [] edges;
}

// Na podstawie numerów wierzchołków 'v' i 'w' zwraca indeks tablicy,
// pod którym można znaleźć wagę krawędzi między 'v' a 'w'
// (graf NIE jest skierowany).
long Graph::getK(int v, int w) {
	if (v >= V || w >= V || v == w)
		return -1;
	if (w > v)
		return (long)(v*(V-0.5*v-1.5)) + w-1;
	if (w < v)
		return (long)(w*(V-0.5*w-1.5)) + v-1;
	return -1;
}


// Usuwa 'm' losowo wybranych krawędzi.
long Graph::RemoveRandomEdges(long m) {
	long    totalEdges = V*(V-1)/2;
	double  rate = (double)(m/(double)edgesLeft); // 'Co którą' krawędź trzeba usunąć.
	double  choice;
	long    cnt = 0;

	for (long n = 0; n < totalEdges; n++) {
		choice = rand()/(double)RAND_MAX;
		if (edgesLeft-cnt == E)
			break;
 		if (choice < rate && include[n] == true) {
 			include[n] = false;
 			cnt++;
		}
	}
	return (edgesLeft-=cnt);
}

// Zapisuje graf do pliku.
void Graph::Save(char * file) {
	long       v, w;
 	int        weight;
 	long       totalEdges = V*(V-1)/2;
	std::ofstream  outfile(file);

	if (outfile.fail()) { std::cout << " Nie moge utworzyć pliku!" << std::endl; exit(EXIT_FAILURE); }

	for (long i = 0; i < totalEdges; i++) {
		v = edges[i]->v;
		w = edges[i]->w;
		weight = edges[i]->weight;
		if (include[i])
			outfile << v << " " << w << " " << weight << std::endl;
	}
	outfile.close();
}

int main(int argc, char* argv[]) {
	char   filename[256];
	long 	 v, e;
	Graph	*g;

	if (argc < 3) {
		std::cout << " Prawidłowe wywołanie: " << argv[0];
  		std::cout << " ile_wierzchołków ile_krawędzi [plik_wynikowy]" << std::endl;
  		exit(EXIT_FAILURE);
	}

	v = atol(argv[1]);
	e = atol(argv[2]);

	if (v <= 0) { std::cout << " Liczba wierzchołków musi byc dodatnia!" << std::endl; exit(EXIT_FAILURE); }

	if (e <= 0) { std::cout << " Liczba krawędzi musi byc dodatnia!" << std::endl; exit(EXIT_FAILURE); }

	if (e < v)  { std::cout << " Liczba krawędzi nie moze byc mniejsza od liczby wierzchołków - inaczej graf skierowany będzie niespójny!" << std::endl; exit(EXIT_FAILURE); }

 	if (argc > 3)
 		strcpy(filename, argv[3]);
 	else
		strcpy(filename, "graf.txt");

	if (SEED == 0) { std::cout << "Proszę nadać stałej SEED wartość swojego numeru albumu" << std::endl; exit(EXIT_FAILURE); }

	srand(SEED);

	std::cout << "\n Generacja ";
 	if (e >= v*(v-1)/2)
 		std::cout << "pełnego ";

	g = new Graph(v, e);
 	std::cout << "grafu nieskierowanego o " << v << " wierzchołkach i " << g->getActualE() << " krawędziach" << std::endl;
	std::cout << " Zapisywanie do pliku '" << filename << "'...";
	g->Save(filename);
	std::cout << " gotowe\n" << std::endl;
	delete g;
	return 0;
}
