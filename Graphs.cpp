

#include "Graphs.h"

using namespace std;
// #define DEBUG

map<int, Dataa *> Data_mp;

int Graph::get_vertices(void)
{
    return v_num;
}

void Graph::set_vertices(int v)
{

    v_num = v;
    for (int i = 0; i < v; i++)
    {
        vector<int> V;
        edges.push_back(V);
    }
    unique_ptr<Minisat::Solver> solver(new Minisat::Solver());
    unique_ptr<Minisat::Solver> solver2(new Minisat::Solver());

    X = (Minisat::Lit **)malloc((v + 1) * sizeof(Minisat::Lit *));

    for (int i = 0; i <= v; i++)
    {
        X[i] = (Minisat::Lit *)malloc((v + 1) * sizeof(Minisat::Lit));
    }

    X2 = (Minisat::Lit **)malloc((v + 1) * sizeof(Minisat::Lit *));

    for (int i = 0; i <= v; i++)
    {
        X2[i] = (Minisat::Lit *)malloc((v + 1) * sizeof(Minisat::Lit));
    }
}

bool Graph::add_edges(int s, int d)
{
    if (s < 0 or s >= v_num or d < 0 or d >= v_num)
    {
        cerr << "Error: vertex not in range" << endl;
        return false;
    }

    if (find(edges[s].begin(), edges[s].end(), d) == edges[s].end())
    {
        edges[s].push_back(d);
    }

    if (find(edges[d].begin(), edges[d].end(), s) == edges[d].end())
    {
        edges[d].push_back(s);
    }

    return true;
}

void Graph::print_graph(void)
{
    for (int i = 0; i < v_num; i++)
    {
        cout << i << " -->";
        vector<int>::iterator it;
        for (it = edges[i].begin(); it != edges[i].end(); ++it)
        {
            cout << *it << " ";
        }

        cout << endl;
    }
}

/* Check if graph has 0 or more edges */
bool Graph::No_edges(void)
{
    for (int i = 0; i < v_num; i++)
    {
        vector<int>::iterator it;
        for (it = edges[i].begin(); it != edges[i].end(); ++it)
        {
            return false;
        }
    }

    return true;
}

void Graph::empty_graph(void)
{
    for (int i = 0; i < v_num; i++)
    {
        edges[i].clear();
    }
    v_num = 0;
    first_run = true;
}

void Graph::initialize_X(void)
{
    int n = v_num;

    // X = (Minisat::Lit**)malloc((n+1) * sizeof(Minisat::Lit*));

    // for(int i=0;i <= n;i++) {
    //     X[i] = (Minisat::Lit*)malloc((n+1) * sizeof(Minisat::Lit));
    // }

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            X[i][j] = Minisat::mkLit(solver->newVar());
        }
    }
}

void Graph::initialize_X2(void)
{
    int n = v_num;

    // X2 = (Minisat::Lit**)malloc((n+1) * sizeof(Minisat::Lit*));

    // for(int i=0;i <= n;i++) {
    //     X2[i] = (Minisat::Lit*)malloc((n+1) * sizeof(Minisat::Lit));
    // }
    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            // cout<<"i == "<<i<<" j = "<<j<<endl;
            X2[i][j] = Minisat::mkLit(solver2->newVar());
        }
    }
}

bool Graph::solve_for_k(int k)
{
    int n = v_num;
    initialize_X();

    auto edges_2(edges);

    for (int i = 1; i <= k; i++)
    {
        Minisat::vec<Minisat::Lit> FIRST_CLOUSE;
        for (int j = 1; j <= n; j++)
        {
            FIRST_CLOUSE.push(X[j][i]);
        }
        solver->addClause(FIRST_CLOUSE);
    }

    for (int j = 1; j <= n; j++)
    {
        for (int p = 1; p <= k; p++)
        {
            for (int q = p + 1; q <= k; q++)
            {
                solver->addClause(~X[j][p], ~X[j][q]);
            }
        }
    }

    for (int i = 1; i <= k; i++)
    {
        for (int p = 1; p <= n; p++)
        {
            for (int q = p + 1; q <= n; q++)
            {
                solver->addClause(~X[p][i], ~X[q][i]);
            }
        }
    }

    for (int s = 1; s <= n; s++)
    {
        for (auto d : edges_2[s - 1])
        {
            Minisat::vec<Minisat::Lit> FOURTH_CLOUSE;

            for (int i = 1; i <= k; i++)
            {
                // cout<<"s == "<<s<<" i == "<<i<<"  d == "<<d+1<<endl;
                FOURTH_CLOUSE.push(X[s][i]);
                FOURTH_CLOUSE.push(X[d + 1][i]);
            }

            solver->addClause(FOURTH_CLOUSE);
        }
    }

    return solver->solve();
}

bool Graph::solve_for_k_3cnf(int k)
{
    int n = v_num;
    initialize_X2();
    auto edges_1(edges);
    Minisat::Lit Dummy[n + 1];

    // for( int j=1; j<=n; j++)
    // {
    //     Dummy[j] = Minisat::mkLit(solver2->newVar());
    // }

    for (int i = 1; i <= k; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            Dummy[j] = Minisat::mkLit(solver2->newVar());

            if (j == 1)
            {
                solver2->addClause(X2[j][i], Dummy[j]);
            }
            else if (j == n)
            {

                solver2->addClause(~Dummy[j - 1], X2[j][i]);
            }
            else
            {
                solver2->addClause(~Dummy[j - 1], X2[j][i], Dummy[j]);
            }
        }
    }

    for (int i = 1; i <= k; i++)
    {
        for (int p = 1; p <= n; p++)
        {
            for (int q = p + 1; q <= n; q++)
            {
                solver2->addClause(~X2[p][i], ~X2[q][i]);
            }
        }
    }

    for (int j = 1; j <= n; j++)
    {
        for (int p = 1; p <= k; p++)
        {
            for (int q = p + 1; q <= k; q++)
            {
                solver2->addClause(~X2[j][p], ~X2[j][q]);
            }
        }
    }

    Minisat::Lit Dummy_3[2][k + 1];
    Minisat::Lit join_dummy = Minisat::mkLit(solver2->newVar());


    for (int s = 1; s <= n; s++)
    {
        for (auto d : edges_1[s - 1])
        {
            join_dummy = Minisat::mkLit(solver2->newVar());

            for (int i = 1; i <= k; i++)
            {
                Dummy_3[0][i] = Minisat::mkLit(solver2->newVar());
                Dummy_3[1][i] = Minisat::mkLit(solver2->newVar());

                if (i == 1 && k != 1)
                {

                    solver2->addClause(X2[s][i], Dummy_3[0][i]);
                    solver2->addClause(~join_dummy, X2[d + 1][i], Dummy_3[1][i]);
                }
                else if (i == k)
                {
                    if (i == 1)
                    {
                        solver2->addClause(X2[s][i], X2[d + 1][i]);
                    }
                    else
                    {
                        solver2->addClause(~Dummy_3[0][i - 1], X2[s][i], join_dummy);
                        solver2->addClause(~Dummy_3[1][i - 1], X2[d + 1][i]);
                    }
                }
                else
                {
                    solver2->addClause(~Dummy_3[0][i - 1], X2[s][i], Dummy_3[0][i]);
                    solver2->addClause(~Dummy_3[1][i - 1], X2[d + 1][i], Dummy_3[1][i]);
                }
            }
        }
    }

    return solver2->solve();
}

void Graph::print_vertex_cover(int k)
{
    int n = v_num;
    // cout<<" k == "<<k<<" cnf \n";
    int ans[k + 1];
    memset(ans, -1, sizeof(ans));
    for (int j = 1; j <= k; j++)
    {
        for (int i = 1; i <= n; i++)
        {
            Minisat::lbool v = solver->modelValue(X[i][j]);
            if (v == Minisat::l_True)
            {
                if (ans[j] != -1)
                {
                    cout << "Error : appearing twicee in ans\n";
                }

                ans[j] = i - 1;
            }
        }
    }

    sort(ans, ans + (k + 1));

    vector<int> temp_sol;
    for (int j = 1; j < k; j++)
    {
#ifdef DEBUG
        cout << ans[j] << "  ";
        cout << " THIS IS FFOR TEMp_SOLL " << "  ";
#endif
        temp_sol.push_back(ans[j]);
    }
#ifdef DEBUG
    cout << ans[k] << endl;
#endif
    temp_sol.push_back(ans[k]);

    Data_mp[v_num]->VC[Algos::CNF_SAT_VC] = (temp_sol);
}

void Graph::print_vertex_cover_3cnf(int k)
{

    int n = v_num;
    int ans[k + 1];
    memset(ans, -1, sizeof(ans));
    for (int j = 1; j <= k; j++)
    {
        for (int i = 1; i <= n; i++)
        {
            Minisat::lbool v = solver2->modelValue(X2[i][j]);
            if (v == Minisat::l_True)
            {
                if (ans[j] != -1)
                {
                    cout << "Error : appearing twicee in ans\n";
                }

                ans[j] = i - 1;
            }
        }
        // cout<<endl;
    }

    sort(ans, ans + (k + 1));

    vector<int> temp_sol;
    for (int j = 1; j < k; j++)
    {
#ifdef DEBUG
        cout << ans[j] << " ";
#endif
        temp_sol.push_back(ans[j]);
    }

    temp_sol.push_back(ans[k]);

    Data_mp[v_num]->VC[Algos::CNF_cnf3_SAT_VC] = (temp_sol);
#ifdef DEBUG
    cout << ans[k] << endl;
#endif
}

void Graph::CNF_SAT_VC(void)
{
    // cout<<" inside CNF_SAT\n";
    int lo = 1;
    int high = v_num;
    int k = (lo + high) / 2;

    while (lo < high)
    {
        // cout<<" lo == "<<lo<<"  high == "<<high<<" for cnf\n";

        solver.reset(new Minisat::Solver());

        k = (lo + high) / 2;
#ifdef DEBUG
        cout<<"CNF:  k s== "<<k<<endl;
#endif
        if (solve_for_k(k))
        {
#ifdef DEBUG
            // cout<<"cnf solved for k == "<<k<<endl;
#endif
            high = k;
        }

        else
        {
            lo = k + 1;
        }
    }

    solver.reset(new Minisat::Solver());
    k = (lo + high) / 2;

    if (solve_for_k(k))
    {
        // cout<<"out cnf solved for k == "<<k<<endl;
#ifdef DEBUG
        cout << "CNF-SAT-VC: ";
#endif
        print_vertex_cover(k);
        Data_mp[v_num]->VC_sizes[Algos::CNF_SAT_VC].push_back(k);
    }
    else
    {
        cout << "Not able to solve\n";
    }

    pclock("Testing clock refined cnf  ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::CNF_SAT_VC]);
    Data_mp[v_num]->Times[Algos::CNF_SAT_VC].push_back(ts[Algos::CNF_SAT_VC]);

    return;
}

void Graph::CNF_SAT_VC_3(void)
{

    // unique_ptr<Minisat::Solver> solver2(new Minisat::Solver());

    // cout<<" inside 3 CNF_SAT\n";
    int lo = 1;
    int high = v_num;
    int k = (lo + high) / 2;
    int last_solved = -1;
    int success_sol = -1;
    while (lo < high)
    {
        solver2.reset(new Minisat::Solver());

        k = (lo + high) / 2;
        last_solved = k;
#ifdef DEBUG
        cout << "3CNF:  k == " << k << endl;
#endif
        if (solve_for_k_3cnf(k))
        {
            success_sol = k;
#ifdef DEBUG
            cout << "3cnf solved for k == " << k << endl;
#endif
            high = k;
        }

        else
        {
            lo = k + 1;
        }
    }
    // cout<<" lo == "<<lo<<"  high == "<<high<<" for 3cnf\n";

    k = (lo + high) / 2;

    if (last_solved != k || success_sol != k)
    {
        // cout<<"reseting after final\n";
        solver2.reset(new Minisat::Solver());
    }

    if ((last_solved == k && success_sol == k) || solve_for_k_3cnf(k))
    {
        // cout<<"out 33cnf solved for k == "<<k<<endl;
#ifdef DEBUG
        cout << "CNF-3-SAT-VC: ";
#endif
        print_vertex_cover_3cnf(k);
        Data_mp[v_num]->VC_sizes[Algos::CNF_cnf3_SAT_VC].push_back(k);
    }
    else
    {
        cout << "Not able to solve\n";
    }

    pclock("Testing clock refined 3cnf  ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::CNF_cnf3_SAT_VC]);
    Data_mp[v_num]->Times[Algos::CNF_cnf3_SAT_VC].push_back(ts[Algos::CNF_cnf3_SAT_VC]);

    return;
}

void Graph::VC1_print_rev_map(map<int, vector<int>> rev_map)
{

    for (auto vc : rev_map)
    {
        cout << "  " << vc.first << "-->";
        for (auto v : vc.second)
        {
            cout << v << " ";
        }
        cout << endl;
    }
}

void Graph::APPROX_VC_1(void)
{

    APPROX_VC_1_vertex_cover.clear();
    vector<vector<int>> adj_list = edges;

    map<int, int> mp; // vertex to deg mapping

    map<int, vector<int>> rev_map; // degree to vertex mapping

    for (int s = 0; s < v_num; s++)
    {
        for (auto d : adj_list[s])
        {
            if (mp.find(d) == mp.end())
            {
                mp[d] = 1;
            }
            else
            {
                mp[d]++;
            }
        }
    }

    for (auto mp_ele : mp)
    {
        if (rev_map.find(mp_ele.second) == rev_map.end())
        {
            vector<int> v;
            v.push_back(mp_ele.first);
            rev_map[mp_ele.second] = v;
        }
        else
        {
            rev_map[mp_ele.second].push_back(mp_ele.first);
        }
    }

    while (true)
    {

        if (rev_map.rbegin() == rev_map.rend())
            break;

        vector<int> highest_vertex_list = (rev_map.rbegin())->second;
        int highest_degree = rev_map.rbegin()->first;

        if (highest_vertex_list.size() == 0)
        {
            rev_map.erase(highest_degree);
            continue;
        }

        int curr_vertex = highest_vertex_list[0];
        // cout<<"current_V = "<<curr_vertex<<endl;

        rev_map[highest_degree].erase(find(rev_map[highest_degree].begin(), rev_map[highest_degree].end(), curr_vertex));
        if (rev_map[highest_degree].size() == 0)
        {
            rev_map.erase(highest_degree);
        }

        APPROX_VC_1_vertex_cover.push_back(curr_vertex);

        for (auto d : adj_list[curr_vertex])
        {
            if (mp[d] == 0)
            {
                continue;
            }

            mp[d]--;

            adj_list[d].erase(find(adj_list[d].begin(), adj_list[d].end(), curr_vertex));

            if (mp[d] == 0)
            {

                mp.erase(d);

                if (find(rev_map[1].begin(), rev_map[1].end(), d) != rev_map[1].end())
                {
                    rev_map[1].erase(find(rev_map[1].begin(), rev_map[1].end(), d));
                }
            }

            else
            {

                int new_degree = mp[d];
                int prev_degree = mp[d] + 1;

                if(find(rev_map[prev_degree].begin(), rev_map[prev_degree].end(), d) != rev_map[prev_degree].end())
                rev_map[prev_degree].erase(find(rev_map[prev_degree].begin(), rev_map[prev_degree].end(), d));

                if (rev_map[prev_degree].size() == 0)
                {
                    rev_map.erase(prev_degree);
                }

                if (rev_map.find(new_degree) == rev_map.end())
                {
                    vector<int> v;
                    v.push_back(d);
                    rev_map[new_degree] = v;
                }
                else
                {
                    rev_map[new_degree].push_back(d);
                }
            }
        }
    }

    sort(APPROX_VC_1_vertex_cover.begin(), APPROX_VC_1_vertex_cover.end());

#ifdef DEBUG
    cout << "APPROX-VC-1:";
    for (auto vc : APPROX_VC_1_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif

    Data_mp[v_num]->VC[Algos::APPROX_VC_1] = (APPROX_VC_1_vertex_cover);

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("APPROX_VC_1 CPU time:", cid);

    pclock("Testing clock approx vc1 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::APPROX_VC_1]);
    Data_mp[v_num]->Times[Algos::APPROX_VC_1].push_back(ts[Algos::APPROX_VC_1]);
    Data_mp[v_num]->VC_sizes[Algos::APPROX_VC_1].push_back(APPROX_VC_1_vertex_cover.size());

    return;
}

bool Graph::check_vc(vector<int> vc)
{

    if (vc.size() == 0)
        return false;

    for (int i = 0; i < v_num; i++)
    {
        vector<int>::iterator it;
        for (it = edges[i].begin(); it != edges[i].end(); ++it)
        {

            if (find(vc.begin(), vc.end(), i) == vc.end() && find(vc.begin(), vc.end(), *it) == vc.end())
            {
                return false;
            }
        }
    }
    return true;
}

void Graph::recur(vector<int> APPROX_vertex_cover, vector<int> curr_vc, uint i, vector<int> &refined_ans)
{

    // if(check_vc(curr_vc) ){
    //     cout<<"chechk vc returend true for  : \n";
    //     for(auto vc : curr_vc) {
    //         cout<<" "<<vc;
    //     }
    //     cout<<endl;

    // }
    if (check_vc(curr_vc) && curr_vc.size() < refined_ans.size())
    {
        refined_ans = curr_vc;
        return;
    }

    if (i == APPROX_vertex_cover.size())
        return;

    curr_vc.push_back(APPROX_vertex_cover[i]);
    recur(APPROX_vertex_cover, curr_vc, i + 1, refined_ans);
    curr_vc.pop_back();

    recur(APPROX_vertex_cover, curr_vc, i + 1, refined_ans);
}

/*
void Graph::Refined_APPROX_VC_1(void)
{

    REFINED_APPROX_VC_1_vertex_cover = APPROX_VC_1_vertex_cover;

    vector<int> curr_vc;
    recur(APPROX_VC_1_vertex_cover, curr_vc, 0, REFINED_APPROX_VC_1_vertex_cover);
#ifdef DEBUG
    cout << "REFINED-APPROX-VC-1:";
    for (auto vc : REFINED_APPROX_VC_1_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif
    Data_mp[v_num]->VC[Algos::REFINED_APPROX_VC_1] = (REFINED_APPROX_VC_1_vertex_cover);

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("REFINED-APPROX-VC-1 CPU time:", cid);

    pclock("Testing clock refined vc1 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::REFINED_APPROX_VC_1]);
    Data_mp[v_num]->Times[Algos::REFINED_APPROX_VC_1].push_back(ts[Algos::REFINED_APPROX_VC_1]);
    Data_mp[v_num]->VC_sizes[Algos::REFINED_APPROX_VC_1].push_back(REFINED_APPROX_VC_1_vertex_cover.size());

    return;
}

void Graph::Refined_APPROX_VC_2(void)
{

    REFINED_APPROX_VC_2_vertex_cover = APPROX_VC_2_vertex_cover;
    vector<int> curr_vc;
    recur(APPROX_VC_2_vertex_cover, curr_vc, 0, REFINED_APPROX_VC_2_vertex_cover);

    Data_mp[v_num]->VC[Algos::REFINED_APPROX_VC_2] = (REFINED_APPROX_VC_2_vertex_cover);

#ifdef DEBUG
    cout << "REFINED-APPROX-VC-2:";
    for (auto vc : REFINED_APPROX_VC_2_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("REFINED-APPROX-VC-2 CPU time:", cid);

    pclock("Testing clock refined vc2 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::REFINED_APPROX_VC_2]);
    Data_mp[v_num]->Times[Algos::REFINED_APPROX_VC_2].push_back(ts[Algos::REFINED_APPROX_VC_2]);
    Data_mp[v_num]->VC_sizes[Algos::REFINED_APPROX_VC_2].push_back(REFINED_APPROX_VC_2_vertex_cover.size());

    return;
}
*/
void Graph::APPROX_VC_2(void)
{

    APPROX_VC_2_vertex_cover.clear();

    // print_graph();s
    auto adj_list(edges);
    auto adj_list_cpy(edges);

    while (check_vc(APPROX_VC_2_vertex_cover) == false)
    {
        bool added_in_vc = false;
        adj_list = adj_list_cpy;
        // int s = (rand()) % v_num;
        for (int s = 0; s < v_num; s++)
        {
            // cout<<"for s : == "<<s;

            if (find(APPROX_VC_2_vertex_cover.begin(), APPROX_VC_2_vertex_cover.end(), s) != APPROX_VC_2_vertex_cover.end())
            {
                // cout<<" ignored s = "<<s<<endl;
                continue;
            }

            for (int d : adj_list[s])
            {

                if (find(APPROX_VC_2_vertex_cover.begin(), APPROX_VC_2_vertex_cover.end(), d) != APPROX_VC_2_vertex_cover.end())
                {
                    // cout<<" ignored d = "<<d<<endl;
                    continue;
                }

                // cout<<"\ns == "<<s<<" d == "<<d<<endl;
                if (s != d)
                {
                    APPROX_VC_2_vertex_cover.push_back(s);
                    APPROX_VC_2_vertex_cover.push_back(d);
                }
                else
                {
                    APPROX_VC_2_vertex_cover.push_back(s);
                }

                added_in_vc = true;

                // remove all edges containing s and d
                for (int j = 0; j < v_num; j++)
                {

                    if (j == s || j == d)
                        continue;

                    auto copyy(adj_list_cpy[j]);

                    if (find(copyy.begin(), copyy.end(), s) != copyy.end())
                    {
                        // cout<<"cp1 1\n";
                        copyy.erase(find(copyy.begin(), copyy.end(), s));
                        // cout<<"cp1 2\n";
                    }

                    if (find(copyy.begin(), copyy.end(), d) != copyy.end())
                    {
                        // cout<<"cp2 1\n";

                        copyy.erase(find(copyy.begin(), copyy.end(), d));
                        // cout<<"cp2 2\n";
                    }

                    adj_list_cpy[j] = copyy;
                }

                if (added_in_vc)
                    break;
            }

            if (added_in_vc)
            {
                adj_list[APPROX_VC_2_vertex_cover[APPROX_VC_2_vertex_cover.size() - 1]].clear();
                adj_list[APPROX_VC_2_vertex_cover[APPROX_VC_2_vertex_cover.size() - 2]].clear();
                break;
            }
        }
    }

    sort(APPROX_VC_2_vertex_cover.begin(), APPROX_VC_2_vertex_cover.end());

#ifdef DEBUG
    cout << "APPROX-VC-2:";
    for (auto vc : APPROX_VC_2_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif

    Data_mp[v_num]->VC[Algos::APPROX_VC_2] = (APPROX_VC_2_vertex_cover);

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("APPROX-VC-2 CPU time:", cid);

    pclock("Testing clock approx vc2 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::APPROX_VC_2]);
    Data_mp[v_num]->Times[Algos::APPROX_VC_2].push_back(ts[Algos::APPROX_VC_2]);
    Data_mp[v_num]->VC_sizes[Algos::APPROX_VC_2].push_back(APPROX_VC_2_vertex_cover.size());
}

void Graph::append_approx_ratios(int v_num)
{
    int minn = INT_MAX;
    for (int i = 0; i < Algos::ALGOS_MAX; i++)
    {
        if ((i == 0 || i == 1))
        {
            continue;
        }
        // if (v_num > 25 && i == 5)
        //     continue;
        minn = min(minn, (Data_mp[v_num]->VC_sizes[i]).back());
        // cout<<" cover i == "<<i<<"  "<<(Data_mp[v_num]->VC_sizes[i]).back()<<endl;
    }
    // cout <<" VC_min == "<<minn<<endl;
    for (int i = 0; i < Algos::ALGOS_MAX; i++)
    {

        if ( (i == 0 || i == 1))
        {
            continue;
        }
        // if (v_num > 25 && i == 5)
        //     continue;

        Data_mp[v_num]->approx_ratio[i].push_back((float)float(Data_mp[v_num]->VC_sizes[i].back()) / (float)minn);
    }
    //  for( int i=0; i < Algos::ALGOS_MAX; i++)
    //  {
    //     auto v  = Data_mp[v_num]->approx_ratio[i];
    //     for(auto j:v) cout<<" i =="<<i<<" v== "<<j<<endl;
    //  }
}
void Graph::Refined_APPROX_VC_1(void)
{
    int ADJ_M[v_num][v_num];
    set<int> curr_set;
    REFINED_APPROX_VC_1_vertex_cover.clear();
 
    for (int i = 0; i < APPROX_VC_1_vertex_cover.size(); i++)
    {
        curr_set.insert(APPROX_VC_1_vertex_cover[i]);
    }
#ifdef DEBUG
cout<<" refined vc1 received input :";
    for (int i = 0; i < APPROX_VC_1_vertex_cover.size(); i++)
    {
        cout<<APPROX_VC_1_vertex_cover[i]<<" ";
        curr_set.insert(APPROX_VC_1_vertex_cover[i]);
    }
cout<<endl;
#endif
    for (int i = 0; i < v_num; i++)
    {
        for (int j = 0; j < v_num; j++)
        {
            if(find(edges[i].begin(), edges[i].end(),j) == edges[i].end()) {
                ADJ_M[i][j] = 0;
            }
            else {
                ADJ_M[i][j] = 1;
            }
        }
    }

    while (true)
    {
        int max_ele = 0;
        int max_i = -1;
        queue<int> testtest;

        for (int i = 0; i < v_num; i++)
        {
            int count = 0;
            if(curr_set.find(i)!=curr_set.end()){
            for (int j = 0; j < v_num; j++)
            {
                if (ADJ_M[i][j] == 1) count++;
            }}

            if(max_ele <= count) {
                max_i = i;
                max_ele = count;
            }
        }
        // cout<<"max_ deg vert  =="<<max_i  <<" and count == "<<max_ele<<endl;

        if (max_ele != 0)
        {
            REFINED_APPROX_VC_1_vertex_cover.push_back(max_i);
            if (curr_set.find(max_i) != curr_set.end())
            { 
                
                for (int i = 0; i < v_num; i++)
                {
                    if ( curr_set.find(i) != curr_set.end() && ADJ_M[max_i][i] )
                    {
                        int travel_all = 0;
                        while (travel_all < v_num)
                        { 
                            if(curr_set.find(travel_all) == curr_set.end() && ADJ_M[i][travel_all] )
                            break;
                            travel_all++;
                        }

                        ADJ_M[i][max_i] = 0;
                        testtest.push(ADJ_M[i][max_i]);
                        
                        if (travel_all == v_num)  {
                            curr_set.erase(i);
                             cout<<" erased i ==  "<<i<<endl;
                             

                        }

                        ADJ_M[max_i][i] = 0;
                        
                    }
                    else if (ADJ_M[max_i][i])
                     {   ADJ_M[max_i][i] = 0;
                            while(!testtest.empty()){
                                testtest.pop();
                            }
                         ADJ_M[i][max_i] = 0;
                     }
                }

                
            }
            else
            {
                for (int i = 0; i < v_num; i++)
                {   
                    if (ADJ_M[max_i][i])
                    {
                        ADJ_M[max_i][i] = 0;
                        while(!testtest.empty()){
                            testtest.pop();
                        }
                        ADJ_M[i][max_i] = 0;
                    }

                }

            }
        }
        else
            break;
    }


    sort(REFINED_APPROX_VC_1_vertex_cover.begin(), REFINED_APPROX_VC_1_vertex_cover.end());


#ifdef DEBUG
    cout << "debug REFINED-APPROX-VC-1:";
    for (auto vc : REFINED_APPROX_VC_1_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif

    Data_mp[v_num]->VC[Algos::REFINED_APPROX_VC_1] = (REFINED_APPROX_VC_1_vertex_cover);

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("REFINED-APPROX-VC-1 CPU time:", cid);

    pclock("Testing clock refined vc1 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::REFINED_APPROX_VC_1]);
    Data_mp[v_num]->Times[Algos::REFINED_APPROX_VC_1].push_back(ts[Algos::REFINED_APPROX_VC_1]);
    Data_mp[v_num]->VC_sizes[Algos::REFINED_APPROX_VC_1].push_back(REFINED_APPROX_VC_1_vertex_cover.size());

    return;
}

void Graph::Refined_APPROX_VC_2(void)
{
    int qqq = 0;
    qqq++;

    int ADJ_M[v_num][v_num];
    set<int> curr_set;
    REFINED_APPROX_VC_2_vertex_cover.clear();
 
    queue<int> testtest;
    for (int i = 0; i < APPROX_VC_2_vertex_cover.size(); i++)
        curr_set.insert(APPROX_VC_2_vertex_cover[i]);
    testtest.push(1);
    for (int i = 0; i < v_num; i++)
    {
        for (int j = 0; j < v_num; j++)
        {
            if(find(edges[i].begin(), edges[i].end(),j) == edges[i].end()) {
                ADJ_M[i][j] = 0;
            }
            else {
                ADJ_M[i][j] = 1;
            }
        }
    }

    while (true)
    {
        int max_ele = 0;
        int max_i = -1;

        for (int i = 0; i < v_num; i++)
        {
            int count = 0;
                        if(curr_set.find(i)!=curr_set.end()){

            for (int j = 0; j < v_num; j++)
            {
                if (ADJ_M[i][j] == 1) count++;
                }
                        }
            if(max_ele <= count) {
                max_i = i;
                max_ele = count;
            }
        }

        if (max_ele != 0)
        {
            if (curr_set.find(max_i) != curr_set.end())
            {
                REFINED_APPROX_VC_2_vertex_cover.push_back(max_i);
                for (int i = 0; i < v_num; i++)
                {
                    if ( curr_set.find(i) != curr_set.end() && ADJ_M[max_i][i] )
                    {
                        int travel_all = 0;
                        while (travel_all < v_num)
                        { 
                            if(curr_set.find(travel_all) == curr_set.end() && ADJ_M[i][travel_all] )
                                break;

                            travel_all++;
                        }

                        ADJ_M[i][max_i] = 0;
                        // testtest.push(2);
                        testtest.push(ADJ_M[i][max_i]);
                        if (travel_all == v_num)  {
                            curr_set.erase(i);
                            // cout<<" erased i ==  "<<i<<endl;

                        }

                        ADJ_M[max_i][i] = 0;
                        
                        
                    }
                    else if (ADJ_M[max_i][i])
                     {   ADJ_M[max_i][i] = 0;
                         while(!testtest.empty()){
                            testtest.pop();
                        }
                         ADJ_M[i][max_i] = 0;
                     }
                }

                
            }
            else
            {
                for (int i = 0; i < v_num; i++)\
                {   
                    if (ADJ_M[max_i][i])
                    {
                        ADJ_M[max_i][i] = 0;
                        ADJ_M[i][max_i] = 0;
                    }

                }

            }
        }
        else
            break;
    }

    sort(REFINED_APPROX_VC_2_vertex_cover.begin(), REFINED_APPROX_VC_2_vertex_cover.end());



#ifdef DEBUG
    cout << "REFINED-APPROX-VC-2:";
    for (auto vc : REFINED_APPROX_VC_2_vertex_cover)
    {
        cout << " " << vc;
    }
    cout << endl;
#endif
    Data_mp[v_num]->VC[Algos::REFINED_APPROX_VC_2] = (REFINED_APPROX_VC_2_vertex_cover);

    // clockid_t cid;
    // pthread_getcpuclockid(pthread_self(), &cid);
    // pclock("REFINED-APPROX-VC-1 CPU time:", cid);

    pclock("Testing clock refined vc2 ", CLOCK_THREAD_CPUTIME_ID, ts[Algos::REFINED_APPROX_VC_2]);
    Data_mp[v_num]->Times[Algos::REFINED_APPROX_VC_2].push_back(ts[Algos::REFINED_APPROX_VC_2]);
    Data_mp[v_num]->VC_sizes[Algos::REFINED_APPROX_VC_2].push_back(REFINED_APPROX_VC_2_vertex_cover.size());

    return;
}

void Graph::cal_algo_results(int v_num)
{

    // for(auto items:Data_mp) {
    auto items = Data_mp[v_num];

    for (int i = 0; i < Algos::ALGOS_MAX; i++)
    {
        if ( (i == 0 || i == 1))
        {
            continue;
        }
        // if (v_num > 25 && i == 5)
        //     continue;

        float mean = 0, summ = 0;
        float summ_sq = 0;
        float sd = 0;
        // bool first_r = true;
        for (auto t : items->Times[i])
        {
            summ = (summ + t.tv_sec * 1000.0 + t.tv_nsec / 1000000.0);
        }

        mean = (float)summ / items->Times[i].size();

        // cout<<"mean == "<<mean<<endl;
        // summ += t.tv_nsec/1000000;

        // summ_sq += (t.tv_nsec / 1000000) * (t.tv_nsec / 1000000);

        for (auto t : items->Times[i])
        {
            float dev = t.tv_sec * 1000 + (t.tv_nsec / 1000000.0) - mean;
            summ_sq += dev * dev;
            // cout<<" dev = "<< dev<<endl;
        }
        // cout<<" summ_sq = "<< summ_sq <<endl;

        sd = (float)sqrt((float)summ_sq / float(v_num));

        items->means_time[i] = mean;
        items->sd_time[i] = sd;
        // FOR APPP
        summ = 0, mean = 0.0, sd = 0.0, summ_sq = 0.0;
        for (auto a : items->approx_ratio[i])
        {
            summ += a;
        }
        mean = (float)summ / items->approx_ratio[i].size();

        for (auto t : items->approx_ratio[i])
        {
            float dev = t - mean;
            summ_sq += dev * dev;
        }

        sd = (float)sqrt((float)summ_sq / float(v_num));

        items->means_aprx_ratio[i] = mean;
        items->sd_aprx_ratio[i] = sd;

        // cout<<" For v = "<<v_num;
        // cout<<" i == "<<i<<" mean == "<<mean<<" sd == "<<sd<<endl;
    }

    // }
}

void Graph::print_each_algo_results(void)
{


    for (int i = 0; i < Algos::ALGOS_MAX; i++)
    {
                for (auto items : Data_mp)
        {


            if ( (i == 0 || i == 1))
            {
                continue;
            }
            // if (items.first > 25 && i == 5)
            //     continue;

            cout << " For V = " << items.first;
            cout << " i == " << i << "  ";
            cout << " mean_time == " << items.second->means_time[i];
            cout << " sd_time == " << items.second->sd_time[i];
            cout << " mean_aprx_ratio == " << items.second->means_aprx_ratio[i];
            cout << " sd_aprx_ratio == " << items.second->sd_aprx_ratio[i] << endl;
        }
    }
}

void Graph::display_answers(int v_num)
{

    for (int i = 0; i < Algos::ALGOS_MAX; i++)
    {
        bool first = true;
        if(i == 0 && timed_out_0 == true) {
            cout << "CNF-SAT-VC: timeout\n";
            continue;
        }
        if(i == 1 && timed_out_1 == true) {
            cout << "CNF-3-SAT-VC: timeout\n";
            continue;
        }

        for (int j = 0; j < Data_mp[v_num]->VC[i].size() - 1; j++)
        {
            if (first)
            {
                if (i == 0)
                {
                    cout << "CNF-SAT-VC: ";
                }
                else if (i == 1)
                {
                    cout << "CNF-3-SAT-VC: ";
                }
                else if (i == 2)
                {
                    cout << "APPROX-VC-1: ";
                }
                else if (i == 3)
                {
                    cout << "APPROX-VC-2: ";
                }
                else if (i == 4)
                {
                    cout << "REFINED-APPROX-VC-1: ";
                }
                else if (i == 5)
                {
                    cout << "REFINED-APPROX-VC-2: ";
                }
                first = false;
            }
            cout << Data_mp[v_num]->VC[i][j] << ",";
        }

        if (first)
        {

            if (i == 0)
            {
                cout << "CNF-SAT-VC: ";
            }
            else if (i == 1)
            {
                cout << "CNF-3-SAT-VC: ";
            }
            else if (i == 2)
            {
                cout << "APPROX-VC-1: ";
            }
            else if (i == 3)
            {
                cout << "APPROX-VC-2: ";
            }
            else if (i == 4)
            {
                cout << "REFINED-APPROX-VC-1: ";
            }
            else if (i == 5)
            {
                cout << "REFINED-APPROX-VC-2: ";
            }
        }

        cout << Data_mp[v_num]->VC[i][Data_mp[v_num]->VC[i].size() - 1];
        cout << endl;
    }
}

void pclock(string msg, clockid_t cid, struct timespec &ts)
{

#ifdef DEBUG
    cout << msg;
#endif
    if (clock_gettime(cid, &ts) == -1)
        cout << "Error: clock_gettime" << endl;
#ifdef DEBUG
    printf("%4jd.%03ld\n", (intmax_t)ts.tv_sec, ts.tv_nsec / 1000000);
#endif
}
