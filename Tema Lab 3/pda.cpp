// Coltos Radu-Mihai
// Grupa 151
// Implementare automat pushdown (determinist)
// Cod adaptat dupa implementarea DFA din Lab 1

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <stack>

using namespace std;

int n, m;
unordered_map<int, int> stare; // stare[nume] = index
vector<unordered_map<string, pair<string, int>> > G;
// G e la baza un vector de N stari
// G[i] = map-ul de tranzitii care pleaca din starea cu INDEXUL i
// G[i][l+L] = starea din care se poate ajunge plecand din i cu litera l si avand in varful stivei L
//             ca o pereche <cuvant_nou, stare_noua>
int S, nrF, nrCuv;
unordered_set<int> stari_finale;
vector<string> cuvinte;
vector<bool> rezultate;

void citire()
{
    ifstream in("input.txt");
    in >> n; // numarul de stari
    G.resize(n);
    for(int i = 0; i < n; i++) // starile
    {
        int x;
        in >> x;
        stare[x] = i;
    }
    in >> m; // numarul de tranzitii
    for(int i = 0; i < m; i++)
    {
        int x, y;
        char l, L;
        string cuv;
        in >> x >> y >> l >> L >> cuv; // tranzitiile
        G[stare[x]][{l,L}] = make_pair(cuv, y);
    }
    in >> S; // starea initiala
    in >> nrF; // nr stari finale
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
    stack<char> stk;
    stk.push('$'); //conventie: $ e baza stivei
    int curent = S;
    for(char l : cuvant)
    {
        if(stk.empty()) // daca stiva e goala automatul se opreste
            return false;

        char L = stk.top();
        stk.pop();
        if(G[stare[curent]].count({l,L}) == 1) // Daca exista trazitii pe litera curenta din cuvant
        {
            auto x = G[stare[curent]][{l,L}];
            curent = x.second; // atunci actualizez starea curenta
            if(x.first != "#") // conventie: # este pe post de lambda
                for(auto i = x.first.rbegin(); i != x.first.rend(); i++)
                    stk.push(*i); // si adaug literele pe stiva in ordine inversa
        }
        else return false;
    }

    // La final verific daca am ajuns intr-o stare finala
    if(stari_finale.count(curent) == 1)
        return true;
    return false;
}

int main()
{
    citire();
    for(const string& cuvant : cuvinte)
        rezultate.push_back(validare(cuvant));

    ofstream out("output.txt");
    for(bool i : rezultate)
        if(i) out << "DA\n";
        else out << "NU\n";
    out.close();
    return 0;
}