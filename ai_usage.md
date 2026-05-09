# Cum am folosit AI-ul - Fazele 1 si 2

## Ce AI am folosit
Am folosit GROK, a lui Elon Musk. L-am folosit strict pentru ce scrie in cerinta:
functiile `parse_condition` si `match_condition`. In rest am scris eu codul.

---

## FAZA 1

### parse_condition

**Ce i-am zis:**
"am un string de forma severity:>=:2 si vreau sa-l impart in trei parti: field, operator si value. fa o functie in C care primeste stringul si intoarce cele trei parti separate"

**Ce mi-a dat:**
O functie care cauta `:` cu `strchr` si taie stringul in bucati.
Prima parte devine field-ul, a doua operatorul, a treia valoarea.

**Ce am schimbat:**
Initial folosea `strcpy` peste tot, am schimbat cu `strncpy` ca sa nu ies din buffer daca primesc un string mai lung decat ma astept. Am mai schimbat si numele parametrilor ca sa se potriveasca cu restul codului meu.

---

### match_condition

**Ce i-am zis:**
"am o structura Raport cu campurile: severitate (int), categorie (char[]), inspector (char[]), timp (time_t). fa o functie care primeste un pointer la raport, un field, un operator si o valoare si returneaza 1 daca raportul satisface conditia"

**Ce mi-a dat:**
Un if mare cu toate cazurile. La `severitate` si `timp` converteste valoarea cu `atoi`/`atol` si compara ca numere. La `categorie` si `inspector` foloseste `strcmp`.

**Ce am invatat:**
- `strcmp` returneaza 0 cand stringurile sunt egale, nu stiam exact ce valoare intoarce
- pentru `time_t` trebuie `atol` nu `atoi` pentru ca pe sisteme de 64 biti e un numar mare

## Corectarea greselilor cu ajutorul AI-ului

Dupa ce am scris restul codului singur, am folosit AI-ul sa-mi gaseasca greseli.
I-am dat bucati din cod si l-am intrebat "vezi ceva gresit aici?". A gasit cateva:

- in functia `permisiunea()` aveam `char p[7]` in loc de `char p[10]`, saream peste ultimele caractere si terminatorul
- in `log_action` uitasem sa verific daca `open` a returnat -1 inainte sa scriu
- la `filter` nu beam newline-ul dupa scanf inainte de fgets, asa ca descrierea se citea gresit

Toate le-am inteles si corectat eu, nu am luat codul fix de la el.

---

## FAZA 2

In faza 2 nu am folosit AI pentru cod, tot ce am adaugat (`monitor_reports.c`, `notificare.c`, `sterge_district.c`) l-am scris eu dupa ce am citit manualele pentru `sigaction`, `fork`, `execvp` si `kill`.

Am folosit AI doar sa intreb "ce face `WIFEXITED`?" si "de ce trebuie `volatile` la variabila din signal handler?" - adica intrebari punctuale sa inteleg niste concepte, nu sa-mi genereze cod.

---

## Ce cred despre AI la programare

E util sa gasesti greseli si sa intelegi rapid un concept nou.
