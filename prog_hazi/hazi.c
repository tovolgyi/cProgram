#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "debugmalloc.h"

typedef struct eloadas
{
    char cim[31];
    char datum[11];
    char neve[26];
    struct eloadas *next;
} eloadas, *eloadas_ptr;

typedef struct szinesz
{
    char nev[26];
    char tulajdonsag[16];
    int viccek;
    double arany;
    struct szinesz *next;
} szinesz, *szinesz_ptr;

void beolvasas(char *file01, char *file02, eloadas_ptr *eloadOutput, szinesz_ptr *szineszOutput)
{
    FILE *eloadasFile = fopen(file01, "r"); // fájlok megnyitása
    FILE *szineszFile = fopen(file02, "r");
    if (eloadasFile == NULL) // Ha nem tudja megnyitni,jelezzen vissza
        printf("Hiba az eloadasok.txt megnyitasaval!");
    if (szineszFile == NULL)
        printf("Hiba a szineszek.txt megnyitasaval!");

    eloadas_ptr eloadasLista = NULL;
    while (!(feof(eloadasFile))) // Fájl végéig olvas
    {
        eloadas *p = (eloadas *)malloc(sizeof(eloadas));            // Helyet foglal az előadásnak
        fscanf(eloadasFile, "%s %s %s", p->cim, p->datum, p->neve); // Elhelyezi a listában
        p->next = eloadasLista;                                     // Shifteli egyet a listaelemet
        eloadasLista = p;
    }
    fclose(eloadasFile); // Bezárja a fájlt

    szinesz_ptr szineszLista = NULL;
    while (!(feof(szineszFile)))
    {
        szinesz *q = (szinesz *)malloc(sizeof(szinesz));                                    // Helyet foglal a színésznek
        fscanf(szineszFile, "%s %s %d %lf", q->nev, q->tulajdonsag, &q->viccek, &q->arany); // Elhelyezi a listában
        q->next = szineszLista;                                                             // Shifteli egyet a listaelemet
        szineszLista = q;
    }
    szinesz *tmp = szineszLista;
    szineszLista = (szinesz *)malloc(sizeof(szinesz));
    szineszLista->next = tmp; // Strázsa létrehozása

    *eloadOutput = eloadasLista;   // Visszaadja az előadás listát
    *szineszOutput = szineszLista; // Visszaajda a színész listát
    fclose(szineszFile);           // Bezárja a fájlt
}

void valogatas(eloadas_ptr eloadOutput, szinesz_ptr szineszOutput, szinesz_ptr *joSzineszekOutput)
{

    eloadas_ptr headEload = eloadOutput; // A paraméterek átvétele
    szinesz_ptr headSzinesz = szineszOutput->next;
    szinesz_ptr joSzineszek = NULL; // Outputok deklarálása
    szinesz_ptr nemTalalhato = NULL;

    while (headEload != NULL) // Az előadás lista végéig
    {
        headSzinesz = szineszOutput->next; // A strázsa átlépése
        while (headSzinesz != NULL)        // A színész lista végéig
        {
            if (strcmp(headEload->neve, headSzinesz->nev) == 0) // Ha az előadő neve és a színész neve megegyezik
            {
                szinesz *p = (szinesz *)malloc(sizeof(szinesz)); // Helyezze el az új listában
                *p = *headSzinesz;
                p->next = joSzineszek;
                joSzineszek = p;
            }
            headSzinesz = headSzinesz->next; // Léptesse az előadás listát, ha a színészek végére ért
        }
        headEload = headEload->next;
    }
    *joSzineszekOutput = joSzineszek;
}

float relativ(szinesz inpt)
{
    return inpt.viccek * inpt.arany; // A viccek hatásosságát adja vissza (elmondott viccek * sikeres viccek aránya)
}

szinesz_ptr kivalasztas(szinesz_ptr joSzineszekOutput)
{
    szinesz_ptr head = joSzineszekOutput;
    szinesz_ptr kovi = joSzineszekOutput->next;
    szinesz_ptr abszolutMin = joSzineszekOutput;
    szinesz_ptr abszolutMax = joSzineszekOutput;
    szinesz_ptr relativMin, relativMax = joSzineszekOutput;
    szinesz_ptr kivalasztasOutput;

    while (head != NULL)
    {
        if (head->viccek > abszolutMax->viccek)
        {
            abszolutMax = head;
        }
        else if (head->viccek < abszolutMin->viccek)
        {
            abszolutMin = head;
        }

        if (relativ(*head) >= relativ(*relativMax))
        {
            relativMax = head;
        }
        else if (relativ(*head) <= relativ(*relativMin))
        {
            relativMin = head;
        }
        head = kovi;
        if (kovi != NULL)
            kovi = kovi->next;
    }
    kivalasztasOutput = abszolutMax;
    abszolutMax->next = abszolutMin;
    abszolutMin->next = relativMax;
    relativMax->next = relativMin;
    relativMin->next = NULL;

    return kivalasztasOutput;
}

eloadas_ptr szelektalas(eloadas_ptr eloadOutput, szinesz_ptr szineszOutput)
{
    eloadas_ptr headEload = eloadOutput;
    eloadas_ptr kovetkezo = headEload->next;
    szinesz_ptr headSzinesz = szineszOutput->next;
    eloadas_ptr szelektalasOutput = NULL;
    int szamlalo = 0;
    while (headEload->next != NULL)
    {
        headSzinesz = szineszOutput->next;
        while (headSzinesz != NULL)
        {
            if (strcmp(headEload->neve, headSzinesz->nev) == 0) // Ha a két ugyanaz
                szamlalo++;                                     // A számláló nőjön

            headSzinesz = headSzinesz->next; // Lépjen a következő színészre
        }
        if (szamlalo == 0) // Ha a számláló nem nőttt
        {
            if (szelektalasOutput == NULL)
            {                                  // Ha ez az első elem
                szelektalasOutput = headEload; // Legyen vele egyenlő
                szelektalasOutput->next = NULL;
            }
            else
            {                                        // Ha már van a listában elem
                szelektalasOutput->next = headEload; // Legyen ez a következő elem
                szelektalasOutput->next->next = NULL;
            }

            szamlalo = 0; // Számláló nullázása
        }
        headEload = kovetkezo;
        if (kovetkezo->next != NULL)
            kovetkezo = kovetkezo->next;
        szamlalo = 0;
    }
    return szelektalasOutput;
}

void kiiratas(szinesz_ptr kivalasztasOutput, eloadas_ptr szelektalasOutput)
{
    char abszolutMax[26], abszolutMin[26], relativMax[26], relativMin[26];
    for (int i = 0; i < 4; i++)
    {
        if (i == 0)
            strcpy(abszolutMax, kivalasztasOutput->nev);
        else if (i == 1)
            strcpy(abszolutMin, kivalasztasOutput->nev);
        else if (i == 2)
            strcpy(relativMax, kivalasztasOutput->nev);
        else if (i == 3)
            strcpy(relativMin, kivalasztasOutput->nev);

        kivalasztasOutput = kivalasztasOutput->next;
    }

    printf("%s mondta a legtobb sikeres viccet.\n", abszolutMax);
    printf("%s mondta a legkevesebb sikeres viccet.\n", abszolutMin);
    printf("%s viccei voltak a legjobbak.\n", relativMax);
    printf("%s viccei voltak a legrosszabbak.\n\n", relativMin);

    eloadas_ptr head = szelektalasOutput;
    while (head != NULL)
    {
        printf("%s", head->neve);
        if (head->next != NULL)
            printf(", ");
        head = head->next;
    }
    printf(" nem talalhatok az adatbazisban!");
}

void szabaditas(szinesz_ptr szineszOutput, eloadas_ptr eloadOutput)
{
    szinesz_ptr headSzinesz = szineszOutput; // Paraméter átvétele
    while (szineszOutput->next != NULL)      // Az utolsó elemig
    {
        headSzinesz = szineszOutput;
        szineszOutput = szineszOutput->next; // Lépteti a pointert
        free(headSzinesz);                   // Freeli az eredetit
    }
    eloadas_ptr headEload = eloadOutput; // Paraméter átvétele

    while (eloadOutput->next != NULL) // Az utolsó elemig
    {
        headEload = eloadOutput;
        eloadOutput = eloadOutput->next; // Lépteti az előzőt
        free(headEload);                 // Az eredetit felszabadítja
    }
}

int main()
{
    char eloadasFile[] = "eloadasok.txt"; // txt fajlok megnyitasa
    char szineszFile[] = "szineszek.txt";

    eloadas_ptr eloadas_t = NULL; // helyfoglalas a beolvasas kimenetenek (szinesz lista es eloadas lista)
    szinesz_ptr szinesz_t = NULL;
    beolvasas(eloadasFile, szineszFile, &eloadas_t, &szinesz_t); // fajlok beolvasasa

    szinesz_ptr joSzineszek = NULL;                // helyfoglalas az adatbazisban szereplo/nem szereplo szineszeknek
    valogatas(eloadas_t, szinesz_t, &joSzineszek); // szineszek szortitozasa

    szinesz_ptr kivalasztasOutput = kivalasztas(joSzineszek); // 4 megfelelo szinesz kivalasztasa

    eloadas_ptr szelektalasOutput = szelektalas(eloadas_t, szinesz_t);

    kiiratas(kivalasztasOutput, szelektalasOutput);

    szabaditas(szinesz_t, eloadas_t);

    return 0;
}