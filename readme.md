To build solution:
1. Make sure DLXOS works and has the compiler exposed
2. Run ./compile_os.sh to build the OS changes
3. For each question, run the respective bash script (i.e. ./compile_qx.sh where x is problem)
4. To run each question, run the ./run_qx.sh where x is the problem!
5. The script will dump the terminal output into a text file called dump_qx.txt

Notes:
Opted to copy the buffer/atmosphere code across makeprocs/, producer/, and consumer/ processes instead of creating a shared lib due to time constraints