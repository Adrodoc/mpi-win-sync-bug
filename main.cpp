#include <iostream>
#include <mpi.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    MPI_Init(NULL, NULL);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Aint win_size = rank == 0 ? sizeof(int) : 0;
    int *mem;
    MPI_Win win;
    MPI_Win_allocate(win_size, sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &mem, &win);
    if (rank == 0)
        mem[0] = 1;
    MPI_Win_lock_all(0, win);

    switch (rank)
    {
    case 0:
    {
        std::cout << "0: Spinning..." << std::endl;
        do
        {
            MPI_Win_sync(win);
        } while (*mem == 1);
        std::cout << "2: Finished" << std::endl;
        break;
    }
    case 1:
    {
        std::cout << "1: Sleeping..." << std::endl;
        sleep(1);
        std::cout << "1: Accumulating..." << std::endl;
        int zero = 0;
        MPI_Accumulate(&zero, 1, MPI_INT,
                       0, 0, 1, MPI_INT,
                       MPI_REPLACE, win);
        std::cout << "1: Flushing..." << std::endl;
        MPI_Win_flush_local(0, win);
        std::cout << "1: Finished" << std::endl;
        break;
    }
    default:
        break;
    }
    MPI_Win_unlock_all(win);

    MPI_Finalize();
    return 0;
}
