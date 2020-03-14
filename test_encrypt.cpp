#include <mpi.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <iostream>
#include <string>

// ALGO caesar chiper // 
char caesar(char msg, int k) {

    if (isupper(msg)) {
        return char(int(msg + k - 65) % 26 + 65);
    }
    else {
        return char(int(msg + k - 97) % 26 + 97);
    }
}

// Temporary array for slave process 
char plaintext[1000];
char a2[1000]; // UNTUK NYIMPEN CHARACTER YG MESTI DI ENKRIP DI SLAVE //
char a3[1000]; // UNTUK NYIMPEN CHARACTER HASIL ENKRIPSI DI ROOT (MASTER) //
char chipertext_master[1000];
char chipertext_slave[1000];
int n, shift;

using namespace std;

int main(int argc, char* argv[])
{
    string hasil;
    int pid, np,
        elements_per_process,
        n_elements_recieved,
        terima_slave;

    // np -> no. of processes 
    // pid -> process id 
    double elapsed_time;
    MPI_Status status;

    // Creation of parallel processes 
    MPI_Init(&argc, &argv);

    // find out process ID, 
    // and how many processes were started 
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);
    elapsed_time = -MPI_Wtime();
    // master process 
    if (pid == 0) {
        int index, i;

        do {
            std::cout << "Masukkan Kata : ";
            std::cin >> plaintext; // MASUKAN PLAINTEXT //
            fflush(stdout);


            std::cout << "Masukan jumlah shift : ";
            std::cin >> shift;
            fflush(stdout);


            n = strlen(plaintext);
            elements_per_process = n / np;
            if (n < np) {
                printf("Jumlah Karakter kurang dari jumlah processor, Silakan input ulang kata\n\n");
            }
            
        } while (n < np);
       
        // check if more than 1 processes are run 
        
    }

    MPI_Bcast(&shift, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (pid == 0) {
        int index, i;
        if (np > 1) {
            // distributes the portion of array 
            // to child processes
            for (i = 1; i < np - 1; i++) {
                index = i * elements_per_process;

                MPI_Send(&elements_per_process,
                    1, MPI_INT, i, 0,
                    MPI_COMM_WORLD);
                MPI_Send((void*)&plaintext[index],
                    elements_per_process,
                    MPI_CHAR, i, 0,
                    MPI_COMM_WORLD);
            }

            // last process adds remaining elements 
            index = i * elements_per_process;
            int elements_left = n - index;

            MPI_Send(&elements_left,
                1, MPI_INT,
                i, 0,
                MPI_COMM_WORLD);
            MPI_Send((void*)&plaintext[index],
                elements_left,
                MPI_CHAR, i, 0,
                MPI_COMM_WORLD);
        }

        // encrypt master process
        for (i = 0; i < elements_per_process; i++) {
            printf("processor %d ada data %c\n", pid, plaintext[i]);
            chipertext_master[i] = caesar(plaintext[i], shift);
        }


        printf("MASTER : %s", chipertext_master);


    }
    // slave processes 
    else {
        MPI_Recv(&n_elements_recieved,
            1, MPI_INT, 0, 0,
            MPI_COMM_WORLD,
            &status);

        // stores the received array segment 
        // in local array a2 
        MPI_Recv(&a2, n_elements_recieved,
            MPI_CHAR, 0, 0,
            MPI_COMM_WORLD,
            &status);


        for (int i = 0; i < n_elements_recieved; i++) {
            printf("processor %d ada data %c\n", pid, a2[i]);
            chipertext_slave[i] = caesar(a2[i], shift);
        }

        // NGE SEND hasil enkripsi slave ke master // 
        MPI_Send((void*)&chipertext_slave,
            n_elements_recieved,
            MPI_CHAR, 0, 99,
            MPI_COMM_WORLD);


        MPI_Send(&n_elements_recieved,
            1, MPI_INT,
            0, 0,
            MPI_COMM_WORLD);


        printf("SLAVE : %s\n\n", chipertext_slave);


    }
    // PRINT TIME //
    if (pid == 0) {
        elapsed_time += MPI_Wtime();
        // printf("Hasil encoding adalah %s", hasil);
        printf("\n\nTotal elapsed time: %10.6f\n", elapsed_time);

        // INI Ngerecieve semua enkripsi dari setiap slave processor //
        for (int i = 1; i < np;i++) {
            MPI_Recv(&terima_slave,
                1, MPI_INT, MPI_ANY_SOURCE, 0,
                MPI_COMM_WORLD,
                &status);

            MPI_Recv(&a3, terima_slave,
                MPI_CHAR, MPI_ANY_SOURCE, 99,
                MPI_COMM_WORLD,
                &status);

            strcat_s(chipertext_master, sizeof(a3) ,a3);

        }


        // GABUNGIN HASIL ENKRIPSI SLAVE SAMA MASTER //
        
        printf("Hasil encode : %s \n", chipertext_master);

    }

    // cleans up all MPI state before exit of process 
    MPI_Finalize();

    return 0;
}
