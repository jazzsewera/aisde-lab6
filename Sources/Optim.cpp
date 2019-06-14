/**********************************************************************************
*                                     OPTIM
*
*  Podział zbioru wierzchołków grafu prostego na dwa równoliczne podzbiory.
*	Celem jest minimalizacja sumarycznego kosztu krawędzi łączących te podzbiory (przekroju).
*	Wykorzystujemy symulowane odprezanie i/lub algorytm Kernighana-Lina (K-L).
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 30 maja 2017
*
***********************************************************************************/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <stdexcept>

#include "Utils.h"
#include "Graph.h"
#include "Config.h"
#include "RNG.h"

const double MIN_TEMPERATURE = 1e-20;  // Minimalna temperatura, poniżej której już nie schodzimy.

// Para wierzchołków
typedef struct {
   int v;
   int w;
} VertexPair;


// Schładzanie - funkcja wypukła
double cool(double T, double a) {
   return T > MIN_TEMPERATURE ? a*T : T;
}

// Schładzanie liniowe.
// Parametry wywołania:
// temperatura początkowa i końcowa, numer kroku, całkowita liczba kroków.
double cool2(double Tstart, double Tstop, int i, int N) {
	return (Tstart - i*(Tstart-Tstop)/static_cast<double>(N));
}

// SCHŁADZANIE - TA FUNKCJA CZEKA NA TWOJE POMYSŁY.
// ZWRACANA TEMPERATURA POWINNA BYĆ DODATNIA.
// LISTA ARGUMENTÓW MOŻE BYĆ OCZYWIŚCIE ZMODYFIKOWANA.
double cool3(double Tstart, double Tstop, int i, int N) {
	//return (Tstart * pow(0.9, i));
  return (log(Tstart - i*(Tstart-Tstop)/static_cast<double>(N) + 1));
}

double cool4() {
  return 0;
}

// Dokonuje dwupodziału grafu za pomocą symulowanego odprężania.
// Wartości funkcji celu w każdym kroku zapisuje w pliku 'outfile'.
int Anneal(Graph& graph, const Config& conf, RNG& randomGenerator, std::ofstream& outfile) {
	double	 T = conf.get_T();	// Temperatura początkowa.
	double	 a = conf.get_a();	// "Stała czasowa" zmian temperatury.
	int		 N = conf.get_N();	// Maksymalna liczba kroków (zmian temperatury).
	int		 s = conf.get_s();	// Liczba prób przy danej temperaturze.

	double  delta = 0;		// Zmiana kosztu podziału grafu w stosunku do poprzedniej iteracji.
	long    initial_cost, prev_cost, new_cost, min_cost;
	int     v = 0, w = 0;   // Indeksy wierzchołków zamienianych miejscami.

	initial_cost = graph.getObjective();
	prev_cost    = initial_cost;
	new_cost     = initial_cost;
	min_cost     = initial_cost;

	std::cout << "*****  Algorytm symulowanego odprężania (wyżarzania) *****\n" << std::endl;

	// Pętla, w której 'N' razy zmieniamy temperaturę.
	for (int i = 0; i < N; i++) {
		if(conf.format_is_matrix() == true)
   			outfile << std::setprecision(4) << T;

		// Wykonujemy 's' prób (!) zamian wierzchołków w danej temperaturze
		for (int j = 0; j < s; j++) {
			// Losujemy po jednym wierzchołku z każdej z dwóch części grafu i
			// zamieniamy je miejscami "na próbę".

			v = randomGenerator.randomInt();
			do // 'v' i 'w' powinny być różnymi wierzchołkami
   			w = randomGenerator.randomInt();
			while (v == w);

			// Zamieniamy wierzchołki na próbę
			graph.swapVertices(v, w);

			// Wyznaczamy zmianę kosztu, jaką spowodowałaby proponowana zamiana wierzchołków.
			// Koszt jest tu unormowany do kosztu początkowego.
			new_cost = graph.getObjective();
			delta = (new_cost-prev_cost)/static_cast<double>(initial_cost);

    		// Jeżeli zamiana wierzchołków spowodowała zmniejszenie kosztu przekroju,
    		// zostanie na pewno zaakceptowany. Istnieje także pewna szansa
    		// akceptacji zamiany zwiększającej koszt.
         if (randomGenerator.randomFrom0To1() < exp(-delta/T)) {
        		prev_cost = new_cost;
        		if (prev_cost < min_cost)
   					min_cost = prev_cost;
			}
      	// Jeśli wzrost kosztu nie został zaakceptowany,
      	// to cofamy zamianę, czyli ponownie zamieniamy wierzchołki.
      	else
      		graph.swapVertices(v, w);

			if (conf.format_is_matrix() == true)
   			outfile << ";" << prev_cost;
			else
  				outfile << std::setprecision(6) << T << ";" << prev_cost << "\n";
		}
		// Zmniejszamy temperaturę zgodnie ze scenariuszem zdefiniowanym w funkcji cool().
		// Warto poeksperymentować z własnymi "scenariuszami schładzania".
		T = cool(T, a);
    // T = cool2(T, 0, i, N);
    // T = cool3(T, 0, i, N);
    // T = cool4();

		if (conf.format_is_matrix())
			outfile << "\n";
	}
	std::cout << " Temperatura końcowa           " << std::setprecision(3) << T << std::endl;
	std::cout << " Koszt początkowego podziału   " << initial_cost << std::endl;
	return min_cost;
}


// Algorytm Kernighana-Lina
Graph& KernighanLin(Graph& graph, bool detailed_output) {
	// Ile wierzchołków w grafie
	int  V = graph.getV();

	// Zyski z zamian:
   long gain; // dla obecnie rozpatrywanej pary
	long max_gain = 0; // dla najlepszej pary znalezionej w bieżącej iteracji,
	long accum_gain = 0; // zysk skumulowany po dotychczas wykonnaych zamianach
	long max_accum_gain = 0; // maksymalny osiągnięty zysk skumulowany
	long last_good_change = -1; // liczba zamian, które do tego max. zysku doprowadziły.

	// "Desirabilities" D = E-I, czyli zyski zewnętrzne minus wewnętrzne poszczególnych wierzchołków
	long *D = new long[V];

	// Które wierzchołki zostały już raz zamienione, więc nie będą uwzględnione w kolejnych iteracjach
   bool *been_swapped = new bool[V];

	// Najlepsze pary wyznaczane w kolejnych iteracjach
	VertexPair *best_swaps = new VertexPair[V/2];

	// Bieżące indeksy wierzchołków - po jednym z każdego podzbioru
   int i, j;

	// Para wierzchołków, których zamiana maksymalizuje zysk w kolejnej z |V|/2 "głównych" iteracji
	int best_i = 0, best_j = 0;

	// Zmienne tymczasowe
	long  c, c1, c2;
	int  cnt;

	std::cout << "\n********   Algorytm Kernighana-Lina   ********\n" << std::endl;
	std::cout << " Koszt początkowego podziału   " << graph.getObjective() << "\n" << std::endl;

   for (int n = 0; n < V/2; n++)
		best_swaps[n] = { 0, 0 };
   for (int n = 0; n < V; n++)
      been_swapped[n] = false;

	// Dla każdego wierzchołka obliczamy potencjalny zysk, jaki przyniosłoby
	// jego przeniesienie do drugiego podzbioru: D[i] = External[i] - Internal[i]
	for (i = 0; i < V; i++) {
		D[i] = 0;
		// Szukamy sąsiadów wierzchołka 'i'
		for (j = 0; j < V; j++) {
           if (i == j or graph.getWeight(i, j) == INF)
              continue;
           if (graph.getSubset(i) != graph.getSubset(j))
              D[i] += graph.getWeight(i, j);
           else
              D[i] -= graph.getWeight(i, j);
		}
	}

   // Algorytm Kernighana-Lina dokonuje zawsze |V|/2 zamian wierzchołków.
   for (int n = 0; n < V/2; n++) {
      cnt = 0;
      if (detailed_output)
         std::cout << " Iteracja " << n+1 << "\n";
      // Wyznaczamy zyski z potencjalnej zamiany poszczególnych par wierzchołków
      for (i = 0; i < V-1; i++) {
         for (j = i+1; j < V; j++) {
         	// Rozważamy tylko te wierzchołki, które już nie zostały przeniesione
         	// i tylko pary należące do różnych podzbiorów.
            if (been_swapped[i] == false and been_swapped[j] == false and graph.getSubset(i) != graph.getSubset(j)) {
               c = graph.getWeight(i, j);
               if (c == INF) c = 0;
               gain = D[i] + D[j] - 2*c;
               if (detailed_output)
                  std::cout << "  g(" << i << ", " << j << ") = " << gain << "\n";
               if (cnt++ == 0 or gain > max_gain) {
						max_gain = gain;
						best_i = i;
						best_j = j;
					}
				}
			}
		}

		// Odnotowujemy parę wierzchołków, których zamiana przyniesie największy zysk...
      best_swaps[n].v = best_i;
	  	best_swaps[n].w = best_j;

		// ...i "blokujemy" ją, aby nie była uwzględniana przy dalszych zamianach
		been_swapped[best_i] = true;
		been_swapped[best_j] = true;

		if (detailed_output)
			std::cout << " Najlepsza zamiana: " << best_i << " i " << best_j << ". Zysk: " << max_gain << "\n\n";

		// Zapamiętujemy skumulowany zysk, który przyniosło pierwszych 'n' zamian.
		accum_gain += max_gain;

		if (accum_gain > max_accum_gain) {
			max_accum_gain = accum_gain;
			last_good_change = n;
		}
      if (not detailed_output)
     		std::cout << (graph.getObjective() - accum_gain) << "\n";

		// Na koniec bieżącej iteracji modyfikujemy 'desirabilities' wszystkich wierzchołków
		// połączonych krawędziami z 'best_i' z 'best_j'.
		for (i = 0; i < V; i++) {
			if (not been_swapped[i]) {
            	c1 = graph.getWeight(i, best_i);
            	c2 = graph.getWeight(i, best_j);
            	if (c1 == INF) c1 = 0;
            	if (c2 == INF) c2 = 0;

            	// Jeśli wierzchołek 'i' należy do tego samego podzbioru, do którego
            	// przed zamianą należał 'best_i', wewnętrzna krawędź 'i-best_i' stanie się zewnętrzną,
            	// zaś zewnętrzna krawędź 'i-best_j' stanie się wewnętrzną.
            	if (graph.getSubset(i) == graph.getSubset(best_i))
               	D[i] += 2*(c1 - c2);
               // Analogicznie postępujemy, jeśli 'i' należy do tego samego podzbioru,
               // do którego przed zamianą należał 'best_j'.
            	else
               	D[i] -= 2*(c1 - c2);
			}
		}
	}

	// W końcu zamieniamy pierwszych 'last_good_change' par wierzchołków
	// spośród wszystkich zapamiętanych w tabeli 'best_swaps'.
	for (int n = 0; n <= last_good_change; n++)
   	graph.swapVertices(best_swaps[n].v, best_swaps[n].w);

	if (detailed_output) {
		if (last_good_change > -1) {
			std::cout << " Ostatecznie wykonujemy " << deklinuj("pierwszą ", "pierwsze ", "pierwszych ", last_good_change+1);
			std::cout << last_good_change+1 << deklinuj(" zamianę", " zamiany", " zamian", last_good_change+1) << "\n";
		}
		else
			std::cout << " Ostatecznie nie wykonujemy żadnych zamian, gdyż tylko zwiększyłyby koszt przekroju\n";
	}
	std::cout << " Koszt ostatecznego podziału     " <<  graph.getObjective() << "\n";

   delete [] best_swaps;
   delete [] been_swapped;
   delete [] D;

   return graph;
}


int main(int argc, char* argv[]) {
	long       min_cost;  // Minimalna wartość funkcji celu osiagnięta w trakcie odprężania.
	time_point from_time, to_time;
	long       time_taken = 0;
	std::ifstream infile;
	std::ofstream valfile, resultfile;
	Config conf;

	// Wczytanie pliku konfiguracyjnego
	try {
		conf.readFromFile("config.txt");
	}
	catch (const std::invalid_argument& ex) {
		std::cout << "TRAGEDIA! Zła wartość parametru \'" << ex.what() << "\' w pliku konfiguracyjnym!" << std::endl;
		exit(EXIT_FAILURE);
	}
	conf.print();

	infile.open(conf.graphFile());
	if (infile.fail()) {
		std::cout << " TRAGEDIA! Nie mogę otworzyć pliku \'" << conf.graphFile() << "\'!\n";
		exit (EXIT_FAILURE);
	}

	valfile.open(conf.valuesFile());
	if (valfile.fail()) {
		std::cout << " TRAGEDIA! Nie mogę utworzyć pliku " << conf.valuesFile() << "\'!\n";
		exit (EXIT_FAILURE);
	}

	resultfile.open(conf.resultsFile());
	if (resultfile.fail()) {
		std::cout << " TRAGEDIA! Nie mogę utworzyć pliku " << conf.resultsFile() << "\'!\n";
		exit (EXIT_FAILURE);
	}

	std::cout << " Wczytywanie grafu z pliku '" << conf.graphFile() << "'...";

	// Ponieważ oba algorytmy modyfikują oryginalne grafy, potrzebne są
	// dwa identyczne egzemplarze: dla metody symulowanego odprężania i dla Kernighana-Lina (K-L).
 	Graph graph_for_SA(infile);

 	// Plik dla algorytmu K-L będzie kopią tego użytego dla symulowanego odprężania.
	// Kopię musimy stworzyć już teraz, ponieważ algorytmy dokonujące biskecji
	// operują na oryginalnych grafach, modyfikując ich podział.
	Graph graph_for_KL(graph_for_SA);

	// Dokonujemy wstępnego podziału: wierzchołki o parzystych indeksach
	// są przydzielane do jednego podzbioru, te o nieparzystych do drugiego.
	graph_for_KL.partitionEvenOdd();

 	std::cout << " gotowe\n";
	std::cout << " Graf ma " << graph_for_SA.getV() << deklinuj(" wierzchołek ", " wierzchołki ", " wierzchołków ", graph_for_SA.getV());
	std::cout << "i " << graph_for_SA.getE() << deklinuj(" krawędź", "krawędzie", " krawędzi", graph_for_SA.getE()) << ".\n\n";

	// Generator liczb pseudolosowych o rozkładzie równomiernym [0, V-1]
	RNG randomNumberGenerator(graph_for_SA.getV()-1);

	for (int i = 0; i < conf.get_p(); i++) {
		// Wybieramy początkowy podział grafu dla algorytmu symulowanego odprężania
		if(conf.get_p() == 1)
			// Jeśli symulowane odprężanie ma być uruchomione tylko raz,
			// początkowy podział jest deterministyczny: wierzchołki o parzystych indeksach
			// są przydzielane do jednego podzbioru, te o nieparzystych do drugiego.
			graph_for_SA.partitionEvenOdd();
		else
			// W przeciwnym wypadku przed każdym uruchomieniem symulowanego odprężania
			// dokonujemy losowego podziału grafu
			graph_for_SA.partitionRandomly(randomNumberGenerator);

// **************** SYMULOWANE ODPRĘŻANIE ****************
		from_time = currentTime(); // czas start
		min_cost  = Anneal(graph_for_SA, conf, randomNumberGenerator, valfile);
  		to_time   = currentTime(); // czas stop

  		time_taken += timeDiffMicroseconds(from_time, to_time);
		resultfile << min_cost << std::endl;

		std::cout << " Koszt ostatecznego podziału   " << graph_for_SA.getObjective() << "\n";
		std::cout << " Koszt najtańszego podziału    " << min_cost << "\n" << std::endl;
	}

	std::cout << " Obliczenia trwały             " << time_taken;
	std::cout << deklinuj(" mikrosekundę", " mikrosekundy", " mikrosekund", time_taken) << " \n" << std::endl;
	time_taken = 0;



// **************** ALGORYTM KERNIGHANA-LINA (K-L) ****************
// Gdy drugi argument ma wartość 'true', w trakcie wykonywania
// algorytmu są wypisywane szczegółowe informacje.

	from_time  = currentTime(); // czas start
	for (int i = 0; i < conf.how_many_KL_runs(); i++)
		KernighanLin(graph_for_KL, conf.display_KL_details());
  	to_time    = currentTime(); // czas stop

  	time_taken = timeDiffMicroseconds(from_time, to_time);

	std::cout << " Obliczenia trwały               " << time_taken;
	std::cout << deklinuj(" mikrosekundę", " mikrosekundy", " mikrosekund", time_taken) << " \n";
   time_taken = 0;

	if (infile.is_open())     infile.close();
  	if (valfile.is_open())    valfile.close();
  	if (resultfile.is_open()) resultfile.close();
}
