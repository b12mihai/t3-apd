Tema 3 APD - Calcul paralel folosind MPI
===================================

Author: Barbulescu Mihai
Group: 331CA

Continut arhiva
--------------
	* complex.c - functii pentru lucrul cu numere complexe si calculul multimilor Mandelbrot si Julia
	* complex.h
	* main.c - sursa principala a programului
	* Makefile
	* README
	* run_tema3.sh - script care compileaza tema si apoi efectueaza testele publice
	Primeste un singur argument: numarul de procesoare. Exemplu de rulare:
			./run_tema3.sh 8
	* test/ - folderul cu teste publice. Trebuie ca scriptul de rulare run_tema3.sh
	si acest folder sa se afle in acelasi folder
	* util.c - functii pentru parsarea fisierului de intrare, pentru scrierea
	imaginii si care descriu taskurile master / slave
	* util.h

Compilare si rulare
------------------

* make - doar pentru compilare, genereaza tema3 ca executabil

* ./run_tema3.sh {numar_procesoare} - compileaza si apoi ruleaza tema. Afiseaza
daca testul este OK sau FAILED si cat timp a luat rularea mpirun (doar real, 
pentru ca e cel interesant)

Detalii de implementare
---------------------

Am impartit datele de prelucrat in fragmente aproximativ egale, astfel "sclavul"
cu rank-ul i primeste un numar de linii egal cu height / numar_taskuri.

La pornire master-ul trimite slave-ului linia cu care incepe prelucarea + 
numarul de linii si datele din fisierul de intrare fara de care nu poate
face prelucrarea. Astfel doar masterul are acces la fisierul de intrare,
procesele slave primind datele strict necesare pentru calcul. 

Pentru fiecare tip de multime am urmat algoritmii descrisi in enuntul temei, fara
nici o abatere, sub forma a doua functii in complex.h.

Pentru precizie am lucrat cu double si am renuntat la a pune
vreun flag de optimizare la compilare.

Tema a fost testata atat pe fep cat si pe masina locala: Ubuntu 12.04, 64bit 
