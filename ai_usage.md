# Cum am folosit AI-ul - Toate Fazele (1, 2 si 3)

## Ce AI am folosit
Am folosit GROK (xAI) si putin ChatGPT. NU am lasat AI-ul sa imi scrie codul pentru mine. L-am folosit doar ca pe un "coleg mai stiutor" care imi explica concepte si imi arata exemple, dupa care am scris codul singur.

---

## FAZA 1

### Unde am folosit AI (putin)

Conform cerintei, am folosit AI pentru doua functii:

1. **`parse_condition`** - am intrebat "cum as putea sa separ un string de forma field:operator:value in C?" Mi-a aratat cum functioneaza `strchr`. Dupa ce am inteles, am scris eu functia.

2. **`match_condition`** - am intrebat "cum compar un int cu o valoare dintr-un string in C?" Mi-a zis de `atoi()`. Am scris eu toate if-urile.

### Ce am invatat de la AI
- `strchr` gaseste un caracter intr-un string
- `atoi` converteste string in int
- `strcmp` returneaza 0 cand sunt egale

### Ce am scris eu singur (majoritatea)
- Tot restul codului: `add`, `list`, `view`, `filter` (logica de citire), `delete_report`, `update`, `permisiuni`, `create_symlink`, etc.
- Structurile de date si logica de filtrare

### O greseala gasita cu ajutorul AI-ului
I-am dat o bucata din `permisiuni.c` si mi-a zis ca am `char p[7]` in loc de `char p[10]`. Corectez si invat sa fiu atent la dimensiuni.

---

## FAZA 2

### Am scris codul singur
Am scris `monitor_reports.c`, `notificare.c` si functia `remove_district` singur, dupa ce am citit manualele pentru:
- `sigaction`
- `fork`
- `execvp`
- `kill`
- `waitpid`

### Ce am intrebat pe AI (doar explicatii)
- "ce face `WIFEXITED`?" - mi-a explicat ca verifica daca un proces s-a terminat normal
- "de ce trebuie `volatile` la variabila din signal handler?" - mi-a explicat ca optimizerul nu vede ca variabila se schimba in handler

NU am luat cod gata facut de la AI pentru faza asta.

---

## FAZA 3

### Am scris codul singur
Am scris `city_hub.c` si `scorer.c` singur, dupa ce am citit:
- `man pipe`
- `man dup2`
- `man execlp`

### Ce am intrebat pe AI (ajutor la intelegere)
- "cum functioneaza pipe intre parinte si copil?" - mi-a desenat un exemplu pe hartie
- "ce face dup2 mai exact?" - mi-a explicat ca duplica un file descriptor
- "cum aflu daca un fisier exista in C?" - mi-a spus de `access()`

Am scris tot codul manual, dar am intrebat AI sa imi explice concepte pe care nu le-am inteles bine din manual.

### O problema rezolvata cu ajutor
Am avut o eroare la `city_hub` - nu vedea mesajele de la monitor. L-am intrebat pe AI "de ce nu vad mesajele imediat?" si mi-a zis de `fflush(stdout)`. Am adaugat si a mers.

---

## Concluzie
| Faza 1 | 2 functii mici (cum cere cerinta) | Restul de ~1000 linii |
| Faza 2 | Doar intrebari teoretice | Tot codul nou |
| Faza 3 | Doar intrebari teoretice | Tot codul nou |

**Parerea mea:** AI e bun sa iti explice concepte si sa iti arate exemple mici. Dar daca lasi AI-ul sa iti scrie tot codul, nu inveti nimic si la prezentare pici. Cel mai mult am invatat cand am scris eu si am gresit, apoi am corectat.
