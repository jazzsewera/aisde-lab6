/**********************************************************************************
*
*	Autor: Dominik Kasprowicz
*	Ostatnia aktualizacja: 30 maja 2017
*
***********************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <chrono>

typedef std::chrono::steady_clock::time_point time_point;

// Do pomiaru czasu wykonania algorytmów
time_point currentTime(void) { return std::chrono::steady_clock::now(); }

// Do pomiaru czasu wykonania algorytmów
long timeDiffMicroseconds(time_point from_time, time_point to_time) {
	return std::chrono::duration_cast<std::chrono::microseconds>(to_time - from_time).count();
}

// Deklinacja rzeczowników. Proszę zwrócić uwagę na nazwy parametrów wywołania.
// Przykładowe wywołanie: deklinuj("ziemniak", "ziemniaki", "ziemniaków", 3)
// zwróci string "ziemniaki", ponieważ chodzi akurat o trzy sztuki.
const std::string& deklinuj(
	const std::string& jeden,
	const std::string& dwa_trzy_cztery,
	const std::string& piec_i_wiecej,
	long ile) {

	long jednostki  = ile % 10;
	long dziesiatki = ile % 100;

	if (ile == 1)
		return jeden;
	if ((jednostki != 2 and jednostki != 3 and jednostki != 4) or
	    dziesiatki == 12 or dziesiatki == 13 or dziesiatki == 14
	   )
		return piec_i_wiecej;
	return dwa_trzy_cztery;
}

#endif // UTILS_H
