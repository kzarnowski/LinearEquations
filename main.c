#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"
#include "debugger.h"

void ReadData( FILE* data, double** pMatrix, double* b, int nDim );

int main( int argc, char* argv[] )
{
    if( argc != 2 ) {
        printf( "Error: Input file is missing." );
        return 1;
    }

    // otworzenie pliku do odczytu
    FILE* data = fopen( argv[1], "r" );
    if( !data ) {
        printf( "Error: Data file does not exist." );
        return 1;
    }

    // odczytanie rozmiaru nDim
    int nDim = 0;
    if( !fscanf( data, "%d", &nDim ) ) {
        printf( "Error: Data file has wrong structure." );
        return 1;
    }

    // wykreowanie macierzy ukladu nDim x nDim
    double** pMatrix = NULL;
    if( !CreateMatrix( &pMatrix, nDim ) ) {
        printf( "Error: Memory allocation failed." );
        return 1;
    }

    // wykreowanie wektora wyrazow wolnych (nDim)
    double* pConstTerms = (double*)malloc( sizeof(double) * nDim );
    if ( !pConstTerms ) {
        printf("Error: Memory allocation failed.");
        return 1;
    }
    memset( pConstTerms, 0, sizeof(double) * nDim );

    // wczytanie danych z pliku
    ReadData( data, pMatrix, pConstTerms, nDim );
    fclose( data );

#ifdef _DEBUG_
    // wydruk kontrolny
    printf( "nDim: %d\n", nDim );
    printf( "Matrix:" );
    PrintMatrix( pMatrix, nDim );
    putchar('\n');
    printf( "Constant terms: \n" );
    for( int i = 0; i < nDim; i++ ) {
        printf( "%lf, ", pConstTerms[i] );
    }
    putchar('\n');
#endif

    // obliczenie wyznacznika i sprawdzenie, czy uklad da sie rozwiazac
    double det = Det( pMatrix, nDim );
    if( det == 0 ) {
        printf( "Error: Equation is contradictory or has infinite number of solutions." );
        return 1;
    }

    // wykreowanie i obliczenie macierzy odwrotnej
    double** pInv = NULL;
    if(!CreateMatrix( &pInv, nDim )) {
        printf( "Error: Memory allocation failed." );
        return 3;
    }
    InverseMatrix( pInv, pMatrix, nDim, det );

#ifdef _DEBUG_
    //wydruk kontrolny
    printf( "\nDet: %lf\n", det );
    printf( "\nInverse of a matrix:" );
    PrintMatrix( pInv, nDim );
    putchar('\n');
#endif

    // wykreowanie wektora wynikowego
    double* pSolutions = (double*)malloc( sizeof(double) * nDim );
    if( !pSolutions ) {
        printf( "Error: Memory allocation failed." );
        return 3;
    }
    memset( pSolutions, 0, sizeof(double)* nDim );

    // rozwiazanie ukladu rownan
    LayoutEqu( pInv, pConstTerms, pSolutions, nDim );

    // wyniki
    printf( "Solutions:\n" );
    double* pTmp = pSolutions;
    for( int i = 0; i < nDim; i++, pTmp++ ) {
        printf( "x%d: %lf\n", i, *pTmp );
    }
    pTmp = NULL;

    // zwolnienie pamieci
    free( pSolutions );
    pSolutions = NULL;
    DeleteMatrix( &pInv, nDim );
    free( pConstTerms );
    pConstTerms = NULL;
    DeleteMatrix( &pMatrix, nDim );

#ifdef _DEBUG_
//wydruk kontrolny
    printf( "\nMemory deallocation:\n" );
    printf( "pMatrix: %p\n", pMatrix );
    printf( "pInv: %p\n", pInv );
    printf( "pSolutions: %p\n", pSolutions );
    printf( "pConstTerms: %p\n", pConstTerms );
#endif

    return 0;
}

void ReadData( FILE* data, double** pMatrix, double* pB, int nDim ) {

    double* pC = *pMatrix;
    // wykorzystujemy wartosc zwracana przez fscanf zeby
    // monitorowac czy plik ma odpowiednia strukture
    for( int i = 0; i < nDim; i++, pMatrix++, pB++ )
    {
        pC = *pMatrix;
        for( int j = 0; j < nDim; j++, pC++ )
        {
            // wczytywanie macierzy
            if( !fscanf( data, "%lf", pC ) ) {
                exit( EXIT_FAILURE );
            }
        }
        // wczytywanie wyrazow wolnych
        if ( !fscanf(data, "%lf", pB) ) {
            exit( EXIT_FAILURE );
        }
    }
}
