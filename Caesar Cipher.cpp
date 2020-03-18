// CREATED BY ANDREAS AGUSTINUS //
// Contact person: Andreasagustinus11@gmail.com //
// FEEL FREE TO USE THE CODE, FOR EDUCATIONAL PURPOSE //

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
//UNTUK TERIMA INPUT DARI USER 
char plaintext[1000]; 

// UNTUK MENYIMPAN CHARACTER YG AKAN DI KIRIM, UNTUK DI ENCODE DI SETIAP SLAVE //
char placeholder_slave[1000]; 

//MENYIMPAN CHARACTER HASIL ENKRIPSI DARI SETIAP SLAVE KE DI ROOT (MASTER),
//UNTUK DI KIRIM KE ROOT (MASTER) 
char placeholder_master[1000]; 

// MENAMPUNG HASAIL ENKRIPSI MASTER  //
char chipertext_master[1000]; 

// MENAMPUNG HASAIL ENKRIPSI SLAVE  //
char chipertext_slave[1000]; 

// N = JUMLAH DATA, SHIFT = BANYAKNYA PERGESERAN KARAKTER //
int n, shift;

using namespace std;

int main(int argc, char* argv[])
{
    int rank, size,
        elements_per_process,
        n_elements_recieved,
        terima_slave;

    // SIZE -> BANYAKNYA PROCESS
    // RANK -> PROCESS ID
    double elapsed_time;
    MPI_Status status;

    // INISIALISASI MPI
    MPI_Init(&argc, &argv);

    // find out process ID, 
    // and how many processes were started 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

   
    elapsed_time = -MPI_Wtime();

    // master process 
    if (rank == 0) {
        int index, i;

        do {
            // MASUKAN PLAINTEXT //
            std::cout << "Masukkan Kata : ";
            std::cin >> plaintext; 
            fflush(stdout);


            // MASUKAN SHIFT //
            std::cout << "Masukan jumlah shift : ";
            std::cin >> shift;
            fflush(stdout);

            // MENGAMBIL PANJANG PLAINTEXT//
            n = strlen(plaintext);

            // MEMBAGI TASK YANG HARUS DI KERJAKAN PER PROCESSOR //
            elements_per_process = n / size;

            // ERROR HANDLING APABILA JUMLAH KARAKTER KURANAG DARI JUMLAH PROCESSOR, PROCESS YG DI INPUT TERLALU BANYAK// 
            if (n < size) {
                printf("Jumlah Karakter kurang dari jumlah processor, Silakan input ulang kata\n\n");
            }
            
        } while (n < size);
    }

    // BROADCAST KE SEMUA PROCESSOR NILAI SHIFT / PERGESERAN KARAKTER //
    MPI_Bcast(&shift, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // BROADCAST PANJANG PLAINTEXT KE SEMUA PROCESSOR //
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        int index, i;
        if (size > 1) {
            // DISTRIBUSI TASK KE SLAVE
            for (i = 1; i < size - 1; i++) {
                index = i * elements_per_process;
                // KOMUNIKASI DARI MASTER KE SLAVE
                MPI_Send(&elements_per_process,
                    1, MPI_INT, i, 0,
                    MPI_COMM_WORLD);
                MPI_Send((void*)&plaintext[index],
                    elements_per_process,
                    MPI_CHAR, i, 0,
                    MPI_COMM_WORLD);
            }

            // TASK SISA YANG HARUS DI KERJAKAN OLEH SLAVE PROCESSOR //
            index = i * elements_per_process;
            int elements_left = n - index;

            // KOMUNIKASI DARI MASTER KE SLAVE
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
            printf("processor %d ada data %c\n", rank, plaintext[i]);
            chipertext_master[i] = caesar(plaintext[i], shift);
        }
        printf("MASTER : %s", chipertext_master);
    }


    // slave processes 
    else {
        // MENERIMA KIRIMAN DARI MASTER
        MPI_Recv(&n_elements_recieved,
            1, MPI_INT, 0, 0,
            MPI_COMM_WORLD,
            &status);
        MPI_Recv(&placeholder_slave, n_elements_recieved,
            MPI_CHAR, 0, 0,
            MPI_COMM_WORLD,
            &status);

        // SLAVE MELAKUKAN ENKRIPSI DARI SETIAP TASK (CHARACTER) YANG IA DAPAT //
        for (int i = 0; i < n_elements_recieved; i++) {
            printf("processor %d ada data %c\n", rank, placeholder_slave[i]);
            chipertext_slave[i] = caesar(placeholder_slave[i], shift);
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

    // PRINT HASIL & WAKTU YG DIBUTUHKAN //
    if (rank == 0) {
        elapsed_time += MPI_Wtime();
        // printf("Hasil encoding adalah %s", hasil);
        printf("\n\nTotal elapsed time: %10.6f\n", elapsed_time);

        // INI Nge-recieve semua enkripsi dari setiap slave processor //
        for (int i = 1; i < size;i++) {
            MPI_Recv(&terima_slave,
                1, MPI_INT, MPI_ANY_SOURCE, 0,
                MPI_COMM_WORLD,
                &status);

            MPI_Recv(&placeholder_master, terima_slave,
                MPI_CHAR, MPI_ANY_SOURCE, 99,
                MPI_COMM_WORLD,
                &status);

            // CONCAT HASIL ENRKIPSI DARI SLAVE PROCESS //
            strcat_s(chipertext_master, sizeof(placeholder_master) , placeholder_master);

        }
        // GABUNGIN HASIL ENKRIPSI SLAVE SAMA MASTER //
        printf("Hasil encode : %s \n", chipertext_master);
    }

    // cleans up all MPI state before exit of process 
    MPI_Finalize();

    return 0;
}
