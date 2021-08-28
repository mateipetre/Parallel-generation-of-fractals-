# Parallel-generation-of-fractals-

Homework from 'Parallel and Distributed Algorithms' course

MATEI Alexandru-Petrut, 333AA

========================================================================================================

                                          Tema #1 - APD
                        Generare paralela de fractali folosind multimile Mandelbrot si Julia

========================================================================================================

                      Tema a fost implementata pornind de la algoritmii secventiali
                    pentru generarea multimilor Mandelbrot si Julia, in C. Paralelizarea
                    programului a fost facuta prin intermediul bibliotecii Pthreads din C.

                      Programul pastreaza functiile de citire a argumentelor din linie de comanda,
                    citire a fisierelor de intrare, scrierea rezultatului in fisierul de iesire,
                    alocarea memoriei pentru rezultate, eliberarea memoriei alocate si functia
                    principala main care apeleaza toate aceste functii si care contine declarari
                    specifice. Va fi paralelizat prin intermediul a P thread-uri.

                      Pentru implementarea paralelizarii programului, am creat o functie de thread
                    << run_julia_mandelbrot >>, care contine calculul paralelizat al multimilor
                    Julia si Mandelbrot. Pentru simplificarea implementarii, am declarat global
                    variabilele utilizate in calcul, corespondente fiecarei multimi in parte:
                    -> par1 si par2 - 2 structuri de tip params asociate celor 2 multimi
                    -> width1,  width2, height1, height2 - variabile asociate dimensiunilor
                       matricelor care vor stoca rezultatele (cele 2 multimi)
                    -> result1, result2 - matricele in care vor fi stocate

                      Functia de thread contine id-ul thread-ului care va fi rulat la un anumit
                    moment de timp. Se incepe cu calculul paralel al multimii Julia. Paralelizarea
                    propriu-zisa consta in impartirea buclei corespunzatoare inaltimii (height)
                    la cele P thread-uri. In acest scop, se calculeaza index-ul de start si
                    index-ul de final al bucle, start si end, folosind id-ul thread-ului si
                    formule specifice intalnite si la laborator. Algoritmul de creare a multimii
                    Julia este cel secvential folosind paralelizarea descrisa mai sus. Se foloseste
                    o bariera pentru sincronizarea calculelor efectuate in cadrul thread-ului.
                    Mai departe se paralelizeaza si operatia de transformare a rezultatului din
                    coordonate matematice in coordonate ecran folosind aceeasi metoda, se va
                    paraleliza bucla care efectueaza efectiv interschimbarea pe axa inaltimii,
                    folosind 2 indecsi de start si end calculati ca in cazul anterior.
                    Functia continua cu implementarea algoritmului paralelizat pentru calculul
                    multimii Mandelbrot, care se va efectua analog implementarii algoritmului
                    paralelizat pentru calculul multimii Julia, numai ca de aceasta data este
                    folosit algoritmul secvential corespondent multimii Mandelbrot.
                    Analog multimii Julia, se va paraleliza si transformarea coordonatelor matematice
                    in coordonate ecran pentru Mandelbrot.

                      Functia principala main va contine apelul functiilor enumerate la inceputul
                    descrierii programului pentru fiecare multime in parte tot prin intermediul
                    variabilelor globale asociate fiecarei multimi. Pe langa asta, se vor declara
                    variabilele necesare implementarii calculului paralel si se va initializa bariera
                    folosita in functia de thread, care va fi distrusa la sfarsitul programului.
                    Se vor crea cele P thread-uri, care folosesc ca functie de thread pe
                    run_julia_mandelbrot descrisa anterior si se va astepta terminarea lor prin
                    functia de join.

                    FEEDBACK: o tema interesanta si foarte aplicata, am studiat putin utilitatea
                    fractalilor si am vazut ca sunt folositi atat in stiinta, cat si in arta.
