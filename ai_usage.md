# Cum am folosit AI-ul - Faza 1

## Ce AI am folosit
Am folosit Grok, a lui Elon Musk varianta gratis. Face des greseli, daca nu ma uit in cod
ci doar ii dau are foarte mullte greseli pe care trebuie analizate si intelese

## Functia parse_condition

### Ce i-am zis
"Imparte un string de genu severity:>=:2 in trei parti"

### Ce mi-a dat
O functie care gaseste `:` si taie stringul in bucati. Partea din stanga e field-ul, cea din mijloc e operatorul, cea din dreapta e valoarea.

### Am schimbat ceva?
Am pus strncpy in loc de strcpy, sa fiu sigur ca nu ies din buffer.


## ------- 
## Functia match_condition

### Ce i-am zis
"Verifica daca un raport se potriveste cu o conditie de genu severity:>=:2. La numere compari normal, la stringuri doar egal sau diferit."

### Ce mi-a dat
Un if mare care verifica fiecare camp. La severity si timestamp foloseste atoi/atol si compara numere. La categorie si inspector foloseste strcmp.

### Am schimbat ceva?
Nu, a mers din prima.

### Ce am retinut
strcmp da 0 cand doua stringuri sunt la fel. time_t se compara ca un numar normal.

---

## Ce m-a mai ajutat AI-ul
I-am dat tot codul si l-am intrebat daca vede greseli. A gasit:
- Un buffer prea mic (p[7] in loc de p[10])
- Parametrul user lipsa intr-o functie
- Niste log-uri scrise gresit

Am rezolvat tot si acum merge.


## Parerea mea
AI-ul e bun pentru cod repetitiv si sa-ti gaseasca greseli. Dar tot eu a trebuit sa stiu ce vreau de la program. Nu poti doar sa-i dai copy-paste si sa mearga.