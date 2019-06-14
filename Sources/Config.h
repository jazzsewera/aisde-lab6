/**********************************************************************************
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 30 maja 2017
*
***********************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <cstring>

class Config {
	private:
		std::string graph_file;
		std::string values_file;
		std::string results_file;
		double  T;
		double  a;
		int     p;
		int     N;
		int     s;
		bool    matrix;
		bool    KL_output_detail_level;
		int     KL_runs;

	public:
		Config() { }
		void   readFromFile(const char*);
		void   print(void);
		double get_T(void) const { return T; }
		double get_a(void) const { return a; }
		int    get_p(void) const { return p; }
		int    get_N(void) const { return N; }
		int    get_s(void) const { return s; }
		bool   format_is_matrix(void)   const { return matrix; }
		bool   display_KL_details(void) const { return KL_output_detail_level; }
		int    how_many_KL_runs(void)   const { return KL_runs; }
		std::string graphFile(void)     const { return graph_file; }
		std::string valuesFile(void)    const { return values_file; }
		std::string resultsFile(void)   const { return results_file; }
};

#endif // CONFIG_H
