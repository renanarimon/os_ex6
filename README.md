# os_ex6
-------
## Taliya Shitret & Renana Rimon 
-------
In this exercise we built a system that checks whether the text is a polynomial.
We built a server, which is able to listen to several clients in parallel, receive texts from them,
and write to the screen (stdOut), the original text, and whether it is a polynomial.

__
## How To Run:
open the terminal and run the command: make all </br>

1. for runing the regular client-server: </br>
   run the command: ./server
   run at another terminal the command: ./client
   Insert through the client the string you would like to check
   - If you would like to exit, enter the word: "quit"
   
   ![image](https://user-images.githubusercontent.com/77155986/212955728-189633c8-891a-4f4b-b38e-f0a39679bb94.png)
   
2. for runing poll server run the command: ./pollServer <number between 2 to 10>
   then run at another terminal the commend: ./pollClient

  ![image](https://user-images.githubusercontent.com/77155986/212957380-cacd8294-0e64-434d-b5fb-ff32a67cd5e1.png)

