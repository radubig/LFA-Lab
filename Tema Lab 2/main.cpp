#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <stdexcept>
#include <queue>
#include <algorithm>
#include <deque>

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

private:
    void remove_unreachable();
    void remove_non_distinguishable();
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
    this->remove_unreachable();
    this->remove_non_distinguishable();
}

void DFA::remove_unreachable()
{
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
        new_states.clear();
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
        auto delta = a.second;
        a.second.clear();
        for(auto& i : delta)
            if(!i.second.empty() && reachable_states.count(i.second[0]) == 1)
                a.second[i.first] = i.second;
    }

    // delete states
    for(const string& bad : unreachable)
        G.erase(bad);
}

void DFA::remove_non_distinguishable()
{
    set<string> Q; // Toate starile
    for(const auto& i : G)
        Q.insert(i.first);
    set<string> F(stari_finale.begin(), stari_finale.end()); //Starile finale
    set<string> Q_minus_F;
    for(const auto& i : Q)
        if(stari_finale.count(i) == 0)
            Q_minus_F.insert(i);

    // Algoritm
    set<set<string>> P, W;
    P.insert(F);
    P.insert(Q_minus_F);
    W = P;

    while(!W.empty())
    {
        set<string> A = *W.begin();
        W.erase(W.begin());
        for(char c : alfabet)
        {
            // X este setul de stari pentru care o tranzitie cu c duce la o stare din A
            set<string> X;
            for(const auto& state : Q)
                if(G[state].count(c) != 0 && A.count(G[state][c][0]) != 0)
                    X.insert(state);

            // foreach set Y : P, unde (X ∩ Y != 0) si (Y \ X != 0) do...
            auto iter = P.begin();
            while(iter != P.end())
            {
                set<string> Y = *iter;
                set<string> X_inter_Y;
                set_intersection(X.begin(), X.end(), Y.begin(), Y.end(), inserter(X_inter_Y, X_inter_Y.begin()));
                set<string> Y_minus_X;
                set_difference(Y.begin(), Y.end(), X.begin(), X.end(), inserter(Y_minus_X, Y_minus_X.begin()));
                if(!X_inter_Y.empty() && !Y_minus_X.empty())
                {
                    P.insert(X_inter_Y);
                    P.insert(Y_minus_X);
                    P.erase(iter);
                    iter = P.begin();

                    if(W.count(Y) != 0)
                    {
                        W.erase(Y);
                        W.insert(X_inter_Y);
                        W.insert(Y_minus_X);
                    }
                    else
                    {
                        if(X_inter_Y.size() <= Y_minus_X.size())
                            W.insert(X_inter_Y);
                        else
                            W.insert(Y_minus_X);
                    }
                }
                else
                    iter++;
            }
        }
    }

    // construire G dupa setul de stari P
    auto G_old = G;
    G.clear();

    // destState contine informatii despre starile in care trebuie sa se ajunga in DFA minimal
    unordered_map<string, string> destState;
    for(const auto& S : P)
    {
        string goodState = *S.begin();
        for(const string& s : S)
            destState[s] = goodState;
    }

    //constructie G
    for(const auto& S : P)
    {
        // luam doar prima stare din S
        string goodState = *S.begin();
        G[goodState];
        for(char c : alfabet) if(G_old[goodState].count(c) != 0)
            G[goodState][c].push_back( destState[G_old[goodState][c][0]] );
    }

    // update stare initiala
    stare_init = destState[stare_init];

    // update stari finale
    auto sf = stari_finale;
    stari_finale.clear();
    for(auto& i : sf)
        stari_finale.insert(destState[i]);
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
        cout << "DFA minimizat: \n";
        cout << dfa << endl;
    }
    catch (const std::runtime_error& err)
    {
        cerr << err.what() << endl;
    }
    return 0;
}
