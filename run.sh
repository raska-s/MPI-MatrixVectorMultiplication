##!/bin/bash

# // This script runs all 5 codes using the same matrix and input size
# // Follow the steps and save, then run using ./run.sh (ensure file is an executable by chmod +x run.sh - else use bash run.sh)

echo '_________________________________'
echo '**HPC Assignment Testing Script**'
echo '_________________________________'
echo 'Setting environment variables...'
module purge
module load intel

# STEP 1. Set matrix sizes x, y, and z
echo 'Defining global macros to set matrix sizes...'
#x = n lines in matrix A
#y = n columns in matrix A = n lines in B
#z = n columns in B
# SET y = $x to have square matrix A.
x=7
y=7
z=3

# STEP 2. Select which scripts to compile (comment/uncomment to set)
echo 'Compiling all scripts using icc and mpicc...'
icc -D M=$x -D N=$y -D P=$z serial.c -o serial.exe
mpicc -D M=$x -D N=$y -D P=$z para1f.c -o para1f.exe
mpicc -D M=$x -D N=$y -D P=$z para2f.c -o para2f.exe
mpicc -D M=$x -D N=$y -D P=$z para3f.c -o para3f.exe
# para_extern is only possible if x=y
mpicc -D M=$x -D P=$z para_extern.c -o para_extern.exe
echo 'Compilation complete.'

# Writing jobscripts using mpi.sub template. DO NOT CHANGE ANYTHING!
echo 'Writing jobscripts...'
cp mpi.sub mpi_1.sub
cp mpi.sub mpi_2.sub
cp mpi.sub mpi_3.sub
cp mpi.sub mpi_4.sub
# Here the script automatically sets the processes required to run the scripts
sed -i "26a\#PBS -l select=1:ncpus=$x:mpiprocs=$x" mpi_1.sub
sed -i '85a\mpirun -machinefile $PBS_NODEFILE -np ${cpus} ./para1f.exe' mpi_1.sub
sed -i "26a\#PBS -l select=1:ncpus=$y:mpiprocs=$y" mpi_2.sub
sed -i '85a\mpirun -machinefile $PBS_NODEFILE -np ${cpus} ./para2f.exe' mpi_2.sub
sed -i "26a\#PBS -l select=1:ncpus=$z:mpiprocs=$z" mpi_3.sub
sed -i '85a\mpirun -machinefile $PBS_NODEFILE -np ${cpus} ./para3f.exe' mpi_3.sub
sed -i "26a\#PBS -l select=1:ncpus=$z:mpiprocs=$z" mpi_4.sub
sed -i '85a\mpirun -machinefile $PBS_NODEFILE -np ${cpus} ./para_extern.exe' mpi_4.sub

# STEP 3. Select which jobs to submit - see above to see which .sub corresponds to each script
echo 'Submitting jobs...'
qsub mpi_1.sub
qsub mpi_2.sub
qsub mpi_3.sub
qsub mpi_4.sub
./serial.exe > HPTC_serial

#!! DO NOT CHANGE ANYTHING PAST THIS LINE!
echo 'Jobs submitted. Waiting for jobs to end...'
echo 'Opening watch qstat. Press Ctrl-C to exit when jobs are done.'
sleep 3
watch qstat
read -p "If jobs have ended, press enter to continue."

# All outputs moved to a new directory titled M(X)_N(Y)_Date&Time and junk files removed
echo 'Clearing junk files...'
dt=$(date '+%d-%m-%H:%M');
mkdir M${x}_N${y}_${dt}
mv HPTC_* M${x}_N${y}_${dt}
rm mpi_*
rm *.exe

#Printing outputs to console
echo 'Output of all jobs (para1, para2, para3, para_extern, serial)'
tail M${x}_N${y}_${dt}/HPTC_*
echo 'Tests complete.'
