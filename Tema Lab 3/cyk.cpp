// Coltos Radu-Mihai
// Grupa 151
// Algoritmul CYK

/* Exemplu de input: (input.txt)
4
S -> AB | BC
A -> BA | a
B -> CC | b
C -> AB | a
2
baaba
bbbbb

Output: (output.txt)
Cuvantul baaba este acceptat.
Cuvantul bbbbb este respins.

*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

const char delim[] = " ->|";
const int LIM = 1000;

struct Gramatica
{
    unordered_map<char, string> terminale;
    unordered_map<char, vector<string>> nonterminale;
} G;
vector<string> cuvinte;

void citire()
{
    int n;
    char s[LIM+5];
    ifstream in("input.txt");

    // citire gramatica
    in >> n; in.get();
    for(int i = 0; i < n; i++)
    {
        char key;
        in >> key;
        in.getline(s, LIM);
        char* p = strtok(s, delim);
        while(p)
        {
            if(*p >= 'a' && *p <= 'z')
                G.terminale[key] += *p;
            else
                G.nonterminale[key].emplace_back(p);
            p = strtok(nullptr, delim);
        }
    }

    // citire cuvinte
    in >> n; in.get();
    for(int i = 0; i < n; i++)
    {
        string cuv;
        in >> cuv;
        cuvinte.push_back(cuv);
    }

    in.close();
}

[[maybe_unused]]
void afisare_gramatica()
{
    ofstream out("output.txt");
    for(const auto& i : G.nonterminale)
    {
        out << i.first << " -> ";
        for(const auto& j : i.second)
            out << j << " ";
        out << "\n";
    }
    for(const auto& i : G.terminale)
    {
        out << i.first << " -> ";
        for(const auto& j : i.second)
            out << j << " ";
        out << "\n";
    }
    out.close();
}

bool verificare_cuvant(const string& cuvant)
{
    // Algoritm: https://www.geeksforgeeks.org/cyk-algorithm-for-context-free-grammar/amp/

    int n = cuvant.size();
    vector<vector<unordered_set<char>>> dp;
    dp.resize(n+1);
    for(auto& i : dp)
        i.resize(n+1);

    for(int i = 1; i <= n; i++)
    {
        for(const auto& j : G.terminale)  // j este un pair <cheie, {terminale}>
            for(const auto& term : j.second)
                if(cuvant.find(term) != string::npos)
                    dp[i][i].insert(j.first);
    }

    for(int l = 2; l <= n; l++)
        for(int i = 1; i <= n - l + 1; i++)
        {
            int j = i + l - 1;
            for(int k = i; k <= j - 1; k++)
                for(const auto& rule : G.nonterminale) // for each rule A -> BC
                    for(const string& q : rule.second) // A = rule.first; B = q[0]; C = q[1]
                        if( dp[i][k].count(q[0]) == 1 &&
                            dp[k+1][j].count(q[1]) == 1)
                                dp[i][j].insert(rule.first);
        }

    if(dp[1][n].count('S') == 1)
        return true;
    return false;
}

int main()
{
    citire();
    //afisare_gramatica();
    for(const string& cuv : cuvinte)
        if(verificare_cuvant(cuv))
            cout << "Cuvantul " << cuv << " este acceptat.\n";
        else
            cout << "Cuvantul " << cuv << " este respins.\n";
    return 0;
}
