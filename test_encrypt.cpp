#include <stdio.h>
#include "mpi.h"
#include <stdlib.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
	int rank, size;
	string plaintext;
	string chipertext;
	int shift;
	int n;
	MPI_Comm comm = MPI_COMM_WORLD;
	char key_character, encrypted_character, character_to_encrypt;

	// INIT MPI //
	MPI_Init(&argc, &argv);
	// Return rank processor //
	MPI_Comm_rank(comm, &rank);
	// Return jumlah process //
	MPI_Comm_size(comm, &size);
	MPI_Status status;



	if (rank == 0) {
		cout << "Masukkan Kata : ";
		cin >> plaintext; // MASUKAN PLAINTEXT //
		n = plaintext.length();// PANJANG PLAINTEXT //
		cout << "Masukan jumlah shift : ";
		getchar(); // fflush;
		cin >> shift;
	}

	MPI_Bcast(&shift, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	MPI_Send(&plaintext, 1 , MPI_CHAR, 1, 99 , MPI_COMM_WORLD);
	MPI_Recv(&plaintext, 1, MPI_CHAR, 0, 99, MPI_COMM_WORLD, &status);


	cout << "Kata " << plaintext << "Pada processor " << rank << " dari total processor " << size << "Nilai n & shift " << n << " " << shift;




	MPI_Finalize();
}
