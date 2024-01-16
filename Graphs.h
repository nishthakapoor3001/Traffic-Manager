#ifndef GRAPHS_H
#define GRAPHS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"

using namespace std;

enum Algos{
    CNF_SAT_VC = 0,
    CNF_cnf3_SAT_VC,
    APPROX_VC_1,
    APPROX_VC_2,
    REFINED_APPROX_VC_1,
    REFINED_APPROX_VC_2,
    ALGOS_MAX
};

class Dataa{
    public:
    vector< timespec> Times[ALGOS_MAX];
    float means_time[ALGOS_MAX];
    float sd_time[ALGOS_MAX];
    vector< int> VC_sizes[ALGOS_MAX];
    vector<int > VC[ALGOS_MAX];
    vector< float> approx_ratio[ALGOS_MAX];
    float means_aprx_ratio[ALGOS_MAX];
    float sd_aprx_ratio[ALGOS_MAX];
};

extern map<int, Dataa*> Data_mp; 

class Graph{
    private:
        int v_num;


        vector<vector<int>> edges;
        unique_ptr<Minisat::Solver> solver;
        unique_ptr<Minisat::Solver> solver2;

        // vector<int> VC[ALGOS_MAX];

        vector<int> APPROX_VC_1_vertex_cover;
        vector<int> REFINED_APPROX_VC_1_vertex_cover;
        vector<int> APPROX_VC_2_vertex_cover;
        vector<int> REFINED_APPROX_VC_2_vertex_cover;

        
        struct timespec ts[ALGOS_MAX];

        Minisat::Lit** X;
        Minisat::Lit** X2;

        void recur(vector<int> APPROX_vertex_cover, vector<int> curr_vc ,uint i, vector<int>& refined_ans );
        bool solve_for_k(int k);
        bool solve_for_k_3cnf(int k);
        void initialize_X(void);
        void initialize_X2(void);
        void VC1_print_rev_map(map<int,vector<int>> rev_map);
        bool check_vc(vector<int> VC);


    public:
        bool first_run;
        int vc_cover = -1;
        bool timed_out_0 = false;
        bool timed_out_1 = false;

        int get_vertices(void);

        void set_vertices(int v);

        void print_vertex_cover(int k);

        void print_vertex_cover_3cnf(int k);

        bool add_edges(int s, int d);

        void print_graph(void);

        void empty_graph (void);

        void CNF_SAT_VC(void);

        void CNF_SAT_VC_3(void);

        void APPROX_VC_1(void);
        
        void Refined_APPROX_VC_1(void);

        void APPROX_VC_2(void);

        void Refined_APPROX_VC_2(void);

        bool No_edges(void);
                    
        void cal_algo_results(int v_num);
        void print_each_algo_results(void);
        void append_approx_ratios(int v_num); 
        void display_answers(int v_num);


};

void pclock(string msg, clockid_t cid, struct timespec& ts);
#endif

