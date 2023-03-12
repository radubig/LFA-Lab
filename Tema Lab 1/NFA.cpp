// Coltos Radu-Mihai
// Grupa 151
// Problema b)

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <queue>

using namespace std;

int n, m;
unordered_map<int, int> stare; // stare[nume] = index
vector<unordered_map<char, vector<int> > > G;
// G e la baza un vector de N stari
// G[i] = map-ul de tranzitii care pleaca din starea cu INDEXUL i
// G[i][litera] = toate starile din care se poate ajunge plecand din i cu litera
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
        G[stare[x]][c].push_back(y);
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
    queue<int> q; // coada de stari curente
    q.push(S); // introducem starea initiala
    for(char poz : cuvant)
    {
        if(q.empty()) // Daca coada e goala inseamna ca DFA-ul a dat abort pe toate ramurile
            return false;
        queue<int> qq;
        while(!q.empty())
        {
            int curent = q.front();
            q.pop();
            if(G[stare[curent]].count(poz) == 1) // Daca exista trazitii pe litera curenta din cuvant
                for(auto& i : G[stare[curent]][poz])
                    qq.push(i); // atunci bag intr-o coada noua toate starile in care pot sa ajung
         // else => pe ramura curenta nu exista tranzitia respectiva deci da abort
        }
        q = qq; //la final copiez coada creata
    }

    // La final verific daca am ajuns in cel putin o stare finala
    bool valid = false;
    while(!q.empty())
    {
        if(stari_finale.count(q.front()) == 1)
            valid = true;
        q.pop();
    }
    return valid;
}

void Dump_Contents()
{
    cout << "Nr stari: " << n << "\n";
    cout << "Starile automatului: ";
    for(const auto& i: stare)
        cout << i.first << " ";
    cout << "\nNr tranzitii: " << m << "\n";
    cout << "Date NFA:\n";
    for(const auto& i: stare)
    {
        cout << "Starea " << i.first << " are urmatoarele tranzitii:\n";
        for(const auto& lit: G[i.second])
        {
            cout << "\t-cu litera " << lit.first << " se ajunge in starile ";
            for(const auto& j : lit.second)
                cout << j << " ";
            cout << "\n";
        }
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
