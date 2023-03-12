// Coltos Radu-Mihai
// Grupa 151
// Problema a)

/// Nota: Solutie adaptata dupa NFA.cpp

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>

using namespace std;

int n, m;
unordered_map<int, int> stare; // stare[nume] = index
vector<unordered_map<char, int> > G;
// G e la baza un vector de N stari
// G[i] = map-ul de tranzitii care pleaca din starea cu INDEXUL i
// G[i][litera] = toate starile din care se poate ajunge plecand din i cu litera (in cazul DFA va fi una singura)
int S, nrF, nrCuv;
unordered_set<int> stari_finale;
vector<string> cuvinte;
vector<bool> rezultate;

void citire()
{
    ifstream in("input.txt");
    in >> n;
    G.resize(n);
    for(int i = 0; i < n; i++)
    {
        int x;
        in >> x;
        stare[x] = i;
    }
    in >> m;
    for(int i = 0; i < m; i++)
    {
        int x, y;
        char c;
        in >> x >> y >> c;
        G[stare[x]][c] = y;
    }
    in >> S;
    in >> nrF;
    for(int i = 0; i < nrF; i++)
    {
        int x;
        in >> x;
        stari_finale.insert(x);
    }
    in >> nrCuv;
    for(int i = 0; i < nrCuv; i++)
    {
        string s;
        in >> s;
        cuvinte.push_back(s);
    }
    in.close();
}

bool validare(const string& cuvant)
{
    int curent = S;
    for(char poz : cuvant)
    {
        if(G[stare[curent]].count(poz) == 1) // Daca exista trazitii pe litera curenta din cuvant
            curent = G[stare[curent]][poz]; // atunci actualizez starea curenta
        else return false;
    }

    // La final verific daca am ajuns intr-o stare finala
    if(stari_finale.count(curent) == 1)
        return true;
    return false;
}

void Dump_Contents()
{
    cout << "Nr stari: " << n << "\n";
    cout << "Starile automatului: ";
    for(const auto& i: stare)
        cout << i.first << " ";
    cout << "\nNr tranzitii: " << m << "\n";
    cout << "Date DFA:\n";
    for(const auto& i: stare)
    {
        cout << "Starea " << i.first << " are urmatoarele tranzitii:\n";
        for(const auto& lit: G[i.second])
            cout << "\t-cu litera " << lit.first << " se ajunge in starea " << i.second << "\n";
    }
    cout << "\nStarea initiala: " << S;
    cout << "\nStari finale: ";
    for(const auto& i: stari_finale)
        cout << i << " ";
    cout << "\n";
}

int main()
{
    citire();
    //Dump_Contents();
    for(const string& cuvant : cuvinte)
        rezultate.push_back(validare(cuvant));

    ofstream out("output.txt");
    for(bool i : rezultate)
        if(i) out << "DA\n";
        else out << "NU\n";
    out.close();
    return 0;
}
