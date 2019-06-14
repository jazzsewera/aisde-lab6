/**********************************************************************************
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 30 maja 2017
*
***********************************************************************************/

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "Config.h"

void Config::print(void) {
   std::cout << "\n**************   Konfiguracja   **************\n";
	std::cout << " Plik z grafem                 " << graph_file << "\n";
	std::cout << " Plik z wynikami pośrednimi    " << values_file;
	std::cout << " (format ";
	if (matrix == true)
		std::cout << "matrix)\n";
	else
		std::cout << "vector)\n";
	std::cout << " Plik z wynikami końcowymi     " << results_file << "\n";
	std::cout << " Temperatura początkowa        " << T << "\n";
	std::cout << " Liczba punktów startowych     " << p << "\n";
	std::cout << " Liczba zmian temperatury      " << N << "\n";
	std::cout << " Liczba prób przy danej temp.  " << s << "\n";
	std::cout << "**********************************************\n" << std::endl;
}

void Config::readFromFile(const char *filename) {
	std::fstream infile;
	std::string  line, name, value;

	infile.open(filename, std::ios::in);
	if (infile.fail()) {
		std::cout << " TRAGEDIA! Nie mogę otworzyć pliku \'" << filename << "\'!" << std::endl;
		exit (EXIT_FAILURE);
	}

	while (std::getline(infile, line)) {
		if (line[0] == '*' or line[0] == '\n')
			continue;
		std::istringstream lnstream(line);
		if (lnstream >> name >> value) {
			if (name.compare("GraphFile") == 0)
				graph_file = value;
			else if (name.compare("ValuesFile") == 0)
				values_file = value;
			else if (name.compare("ResultsFile") == 0)
				results_file = value;
			else if (name.compare("T") == 0) {
				try { T = std::stod(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("T"); }
			}
			else if (name.compare("a") == 0) {
				try { a = std::stod(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("a"); }
			}
			else if (name.compare("p") == 0) {
				try { p = std::stoi(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("p"); }
			}
			else if (name.compare("N") == 0) {
				try { N = std::stoi(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("N"); }
			}
			else if (name.compare("s") == 0) {
				try { s = std::stoi(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("s"); }
			}
			else if (name.compare("format") == 0) {
				if (value.compare("matrix") == 0)
					matrix = true;
				else if (value.compare("vector") == 0)
					matrix = false;
				else
					throw std::invalid_argument("format");
			}
			else if (name.compare("KernighanLinDetails") == 0) {
				if (value.compare("T") == 0)
					KL_output_detail_level = true;
				else if (value.compare("N") == 0)
					KL_output_detail_level = false;
				else
					throw std::invalid_argument("DisplayKernighanLinDetails");
			}
			else if (name.compare("KernighanLinRuns") == 0) {
				try { KL_runs = std::stoi(value); }
				catch (const std::invalid_argument& ex) { throw std::invalid_argument("KernighanLinRuns"); }
			}
			else {}
		}
	}
	if (T <= 0) throw std::invalid_argument("T");
	if (a <= 0) throw std::invalid_argument("a");
	if (p <= 0) throw std::invalid_argument("p");
	if (N <= 0) throw std::invalid_argument("N");
	if (s <= 0) throw std::invalid_argument("s");
	if (KL_runs <= 0) throw std::invalid_argument("KernighanLinRuns");
}
