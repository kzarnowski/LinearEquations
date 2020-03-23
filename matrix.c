#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "debugger.h"

/*------------------------------------------------------------------------
	Konwencja oznaczeń:
	pR / pC     - wskazniki do przechodzenia po wierszach/kolumnach macierzy
	//-- ...    - komentarz dotyczacy jednej lub więcej instrukcji ponizej
	debugger.h  - wydruki kontrolne on/off
------------------------------------------------------------------------*/

void Complement( double** pTabO, double** pTabI, int nRow, int nCol, int nDim ); // wyciecie wiersza i kolumny z macierzy
void ComplMatrix( double** pTabD, double** pTab, int nDim ); // znajduje macierz dopelnien algebraicznych

int CreateMatrix( double*** pTab, int nSize ) {

	//-- Alokuje pamiec na macierz i inicjalizuje 0
	//-- 1 - ok, 0 - blad alokacji

	//-- tablica wskaznikow na wiersze
	*pTab = (double**)malloc( sizeof(double*) * nSize );
	if( !*pTab ) { return 0;}
	memset( *pTab, 0, sizeof(double*) * nSize );

	//-- tablice dla wierszy
	double** pR = *pTab;
	for( int i = 0; i < nSize; i++ ) {
		*pR = (double*)malloc( sizeof(double) * nSize ); // alokuje i-ty wiersz
		if( !*pR ) { return 0;}
		memset( *pR, 0, sizeof(double) * nSize );
		pR++; // przejscie na nastepny wiersz
	}

	return 1;
}

void DeleteMatrix( double*** pTab, int nSize ) {

	//-- zwalnianie pamięci
	double** pR = *pTab;
	for( int i = 0; i < nSize; i++ ) {
		free( *pR ); // zwolnienie tablicy i-tego wiersza
		pR++;
	}
	free( *pTab ); // zwolnienie tablicy wskazników na wiersze
	*pTab = NULL;
}

void TransMatrix( double** pTab, int nDim ) {

	double** pR = pTab; // tutaj potrzebny pomocniczy, bo pTab musi pozostac niezmieniony
	double*  pC = *pTab;
	double tmp = 0;

	for( int i = 0; i < nDim; i++, pR++ ) {
		pC = *pR;
		for( int j = 0; j < i; j++, pC++ ) {
			// indeksowanie wykorzystujemy wylacznie w celu odwolania sie
			// do pozycji "symetrycznej" dla biezacego elementu
			tmp = *pC;
			*pC = pTab[j][i];
			pTab[j][i] = tmp;
		}
	}
}

void InverseMatrix( double** pInv, double** pTab, int nSize, double det ) {

	//-- macierz dolaczona = transponowana macierz dopelnien algebraicznych
	ComplMatrix( pInv, pTab, nSize );

#ifdef _DEBUG_
	// wydruk kontrolny
	printf( "\nComplement Matrix:" );
	PrintMatrix( pInv, nSize );
#endif

	TransMatrix( pInv, nSize );

#ifdef _DEBUG_
	// wydruk kontrolny
	printf( "\nTranspose of a complement matrix:" );
	PrintMatrix( pInv, nSize );
#endif

	//-- macierz odwrotna = 1/det * macierz dolaczona
	double** pR = pInv;
	double* pC = *pInv;

	for( int i = 0; i < nSize; i++, pR++ ) {
		pC = *pR;
		for( int j = 0; j < nSize; j++, pC++ ) {
			*pC /= det; // mnozenie macierzy przez skalar
		}
	}
}

double Det( double** pTab, int nSize ) {

	double d = 0; // wyznacznik
	int sign = 1; // sterowanie znakiem przy rozwinieciu

	if( nSize == 2 ) {
		//wyznacznik 2x2
		return ( pTab[0][0]*pTab[1][1]) - (pTab[0][1]*pTab[1][0] );
	}

	if( nSize == 1 ) {
		return **pTab;
	}

	double** pMinorMx = NULL; // pomocnicza macierz do obliczenia minora
	if( !CreateMatrix(&pMinorMx, nSize - 1) ) {
		printf( "Error: Memory allocation failed." );
		exit( EXIT_FAILURE );
	}

	//-- rozwiniecie wzgl. 0-go wiersza
	double* pC = *pTab;
	for( int j = 0; j < nSize; j++, pC++ ) {
		Complement( pMinorMx, pTab, 0, j, nSize ); // macierz po wycieciu wiersza '0' i kolumny 'j'
		sign = ( j % 2 ) == 0 ? 1 : -1; // znak zalezy od parzystosci numeru kolumny
		d += sign * (*pC) * Det( pMinorMx, nSize - 1 ); // sumowanie minorow
	}

	//-- zwalnianie pomocniczej macierzy
	DeleteMatrix( &pMinorMx, nSize - 1 );
	return d;
}

void LayoutEqu( double** pInv, double* pB, double* pRes, int nSize ) {

	//-- wektor wynikowy = macierz odwrotna * wektor wyrazów wolnych

	double* pC = *pInv;
	double* pTmpB = pB; // pomocniczy do mnożenia przez wyrazy wolne

	for( int i = 0; i < nSize; i++, pInv++ ) {
		pC = *pInv;
		pTmpB = pB;

		//-- mnozenie i-tego wiersza z wyrazami wolnymi
		for( int j = 0; j < nSize; j++, pC++ ) {
			*pRes += *pC * (*pTmpB);
			pTmpB++;
		}
		pRes++; // przejscie do nastepnej niewiadomej
	}
}

void PrintMatrix( double** pTab, int nSize ) {

	putchar('\n');
	double** pR = pTab;
	double* pC = *pR;

	for( int i = 0; i < nSize; i++, pR++ ) {
		pC = *pR;
		for( int j = 0; j < nSize; j++, pC++ ) {
			printf( "%lf\t", *pC );
		}
		putchar('\n');
	}
}

//-----------------------

void Complement( double** pTabO, double** pTabI, int nRow, int nCol, int nDim ) {

	//-- wyciecie wiersza nRow i kolumny nCol z macierzy

	double*  pC_in  = *pTabI; // kolumny macierzy wejściowej
	double*  pC_out = *pTabO; // kolumny macierzy wynikowej

	for( int i = 0; i < nDim; i++, pTabI++, pTabO++ )
	{
		//-- wyciecie wiersza nRow
		if( i == nRow ) {
			pTabO--;
			continue;
		}

		pC_in = *pTabI;
		pC_out = *pTabO;

		for( int j = 0; j < nDim; j++, pC_in++, pC_out++ ) {
			//-- wyciecie kolumny nCol
			if ( j == nCol ) {
				pC_out--;
				continue;
			}
			*pC_out = *pC_in; // kopiowanie do nowej macierzy
		}
	}
}

void ComplMatrix( double** pTabD, double** pTab, int nDim ) {

	//-- znajduje macierz dopelnien algebraicznych

	double** pR_in = pTab;	// potrzebny pomoczniczy bo bedziemy wywolywac Complement(..., pTab, ...)
	double* pC_in  = *pTab;  // kolumny macierzy wejsciowej
	double* pC_out = *pTabD; // kolumny macierzy dopelnien algebraicznych
	double** pMinorMx = NULL; // pomocnicza macierz do obliczenia minora
	int sign = 1; // sterowanie znakiem przy rozwinieciu

	if( !CreateMatrix( &pMinorMx, nDim - 1 ) ) {
		printf( "Error: Memory allocation failed." );
		exit( EXIT_FAILURE );
	}

	for (int i = 0; i < nDim; i++, pR_in++, pTabD++) {
		pC_in = *pR_in;
		pC_out = *pTabD;

		for (int j = 0; j < nDim; j++, pC_in++, pC_out++) {
			sign = (i + j) % 2 == 0 ? 1 : -1; // znak zalezy od parzystosci (wiersz + kolumna)
			Complement( pMinorMx, pTab, i, j, nDim ); // macierz po wycieciu wiersza 'i' oraz kolumny 'j'
			*pC_out = sign * Det( pMinorMx, nDim - 1 ); // sumowanie minorow
		}
	}
	//-- zwolnienie pomocniczej macierzy
	DeleteMatrix( &pMinorMx, nDim - 1 );
}
