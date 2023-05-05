#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <queue>
#include <algorithm>

using namespace std;

class Automat
{
public:
    Automat();
    void readFromFile(const std::string& file);
    void clear() noexcept;
    friend ostream& operator<< (ostream& os, const Automat& at);
    const string& getStareInit() const;
    const string& getAlfabet() const;
    const vector<string>& getStareFromG(const string& stare_plecare, char lit) const;
    bool existsStareFromG(const string& stare_plecare, char lit) const;
    const unordered_set<string>& getStariFinale() const;

protected:
    string alfabet;
    unordered_map<string, unordered_map<char, vector<string> > > G;
    string stare_init;
    unordered_set<string> stari_finale;
};


class NFA : public Automat
{
public:
    NFA() : Automat() {}
};


class DFA : public Automat
{
public:
    DFA() : Automat() {}
    explicit DFA(const NFA& nfa);
    void minimize();
};

Automat::Automat() = default;

void Automat::clear() noexcept
{
    *this = Automat();
}

void Automat::readFromFile(const std::string& file)
{
    unsigned int m;
    ifstream in(file);

    if(!in.is_open())
        throw runtime_error("File " + file + " not opened!");
    this->clear();
    unsigned int nr_stari;
    in >> nr_stari;
    for(int i = 0; i < nr_stari; i++)
    {
        string x;
        in >> x;
    }
    in >> alfabet;
    in >> m;
    for(int i = 0; i < m; i++)
    {
        string x, y;
        char c;
        in >> x >> y >> c;
        G[x][c].push_back(y);
    }
    in >> stare_init;
    int nr_stari_finale;
    in >> nr_stari_finale;
    for(int i = 0; i < nr_stari_finale; i++)
    {
        string x;
        in >> x;
        stari_finale.insert(x);
    }
    in.close();
}

ostream& operator<<(ostream& os, const Automat& at)
{
    os << "Nr stari: " << at.G.size() << "\n";
    os << "Starile automatului: ";
    for(const auto& i: at.G)
        os << i.first << " ";
    os << "\nDate automat:\n";
    for(const auto& i: at.G)
    {
        os << "Starea " << i.first << " are urmatoarele tranzitii:\n";
        for(const auto& lit: i.second)
        {
            os << "\t-cu litera " << lit.first << " se ajunge in starile ";
            for(const auto& j : lit.second)
                os << j << " ";
            os << "\n";
        }
    }
    os << "\nStarea initiala: " << at.stare_init;
    os << "\nStari finale: ";
    for(const auto& i: at.stari_finale)
        os << i << " ";
    os << "\n";
    return os;
}

const string& Automat::getStareInit() const
{
    return stare_init;
}

const string& Automat::getAlfabet() const
{
    return alfabet;
}

const unordered_set<string>& Automat::getStariFinale() const
{
    return stari_finale;
}

bool Automat::existsStareFromG(const std::string& stare_plecare, char lit) const
{
    return G.count(stare_plecare) > 0 &&
           G.at(stare_plecare).count(lit) > 0;
}

const vector<string>& Automat::getStareFromG(const string& stare_plecare, char lit) const
{
    return G.at(stare_plecare).at(lit);
}

DFA::DFA(const NFA& nfa)
    :Automat()
{
    stare_init = nfa.getStareInit();
    alfabet = nfa.getAlfabet();

    queue<vector<string>> q;
    q.push({stare_init});
    while(!q.empty())
    {
        vector<string> fr = q.front();
        q.pop();

        string stare_veche;
        for(const auto& i : fr)
            stare_veche += i;

        for(char i : alfabet)
        {
            set<string> s;
            for(const string& j : fr) if(nfa.existsStareFromG(j, i))
                for(const auto& k : nfa.getStareFromG(j, i))
                    s.insert(k);

            if(s.empty()) continue;

            string stare_noua{};
            bool add_new_final_state = false;
            for(const auto& j : s)
            {
                stare_noua += j;
                if(nfa.getStariFinale().count(j) > 0)
                    add_new_final_state = true;
            }

            vector<string> reachable_states(s.begin(), s.end());
            if(G.count(stare_noua) == 0)
            {
                G[stare_noua] = {};
                q.push(reachable_states);
                if(add_new_final_state)
                    stari_finale.insert(stare_noua);
            }
            G[stare_veche][i].push_back(stare_noua);
        }
    }
}

void DFA::minimize()
{
    // Unreachable states
    set<string> reachable_states;
    set<string> new_states;
    reachable_states.insert(stare_init);
    new_states.insert(stare_init);

    while(!new_states.empty())
    {
        set<string> temp;
        for(const auto& q : new_states)
            for(const auto& c : alfabet)
                for(const string& i : G[q][c])
                    temp.insert(i);
        set_difference(temp.begin(), temp.end(), reachable_states.begin(), reachable_states.end(),
                       inserter(new_states, new_states.begin())); // new_states = temp \ reachable_states
        for(const auto& i : new_states)
            reachable_states.insert(i); // reachable_states += new_states
    }

    set<string> unreachable;
    for(const auto& i : G)
        unreachable.insert(i.first);
    for(const auto& i : reachable_states)
        unreachable.erase(i);

    // update DFA
    // delete tranzitions
    for(auto& a : G)
    {
        auto it = a.second.begin();
        while(it != a.second.end())
        {
            for(const string& bad : unreachable)
                if(it->second[0] == bad)
                {
                    it = a.second.erase(it);
                    break;
                }
                else ++it;
        }
    }
    // delete states
    for(const string& bad : unreachable)
        G.erase(bad);
}


int main()
{
    try
    {
        NFA nfa;
        nfa.readFromFile("input.txt");
        cout << "Date despre NFA: \n" << nfa << "\n\n\n";

        DFA dfa(nfa);
        cout << "Date despre DFA-ul construit din NFA: \n";
        cout << dfa << endl;

        dfa.minimize();
    }
    catch (const std::runtime_error& err)
    {
        cerr << err.what() << endl;
    }
    return 0;
}
