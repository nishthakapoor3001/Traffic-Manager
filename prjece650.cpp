// AVOID  DECREMENT S AND D
// IN CASE YOU ARE USING THIS FILE 
#include <bits/stdc++.h>
#include <string>
#include <algorithm>
#include <thread>
#include <pthread.h>
#include "Graphs.h"
#include "Parser.h"

using namespace std;

Graph* g = new Graph;

void* process_Input(void* arg) {
    string edges_list;

    // read from stdin until EOF
    while (!cin.eof()) {

        // read a line of input until EOL and store in a string
        string line;
        getline(cin, line);

        // create an input stream based on the line
        // we will use the input stream to parse the line
        istringstream input(line);    
        
        if(trim(line) == ""){
            continue;
        }
        // if eof bail out
        if (input.eof())
            return NULL;

        // read a character
        // Note that whitespace is ignored
        char Query;
        input >> Query;

        int vertices;
        switch(Query)
        {
            case 'V':
                input >> vertices;
                query_V(g, vertices);

                break;

            case 'E':
            
                if ( query_E(g, input.str()) == true)
                {
                    /* Check if graph has zero edges */
                    if(g->No_edges() == true) {
                        // cerr<<"Error: Empty Graph => No Vertex Cover Exist"<<endl;
                        continue;
                    }

                    return NULL;
                }

                break;

            default:
                break;
        }
        continue;
    }

    return NULL;
}

void Group_for_thread_1(void) {
    
    g->APPROX_VC_1();
    g->Refined_APPROX_VC_1();

}
void* group_1 (void * arg) {

    pthread_t thread_APPROX_VC_1,thread_REFINED_APPROX_VC_1;
    pthread_create(&thread_APPROX_VC_1,NULL,(THREADFUNCPTR)(&Graph::APPROX_VC_1), g);
    pthread_join(thread_APPROX_VC_1,NULL);
    pthread_create(&thread_REFINED_APPROX_VC_1,NULL,(THREADFUNCPTR)(&Graph::Refined_APPROX_VC_1), g);
    pthread_join(thread_REFINED_APPROX_VC_1,NULL);
    return NULL;
}

void* group_2 (void * arg) {

    pthread_t thread_APPROX_VC_2,thread_REFINED_APPROX_VC_2;
    pthread_create(&thread_APPROX_VC_2,NULL,(THREADFUNCPTR)(&Graph::APPROX_VC_2), g);
    pthread_join(thread_APPROX_VC_2,NULL);
    pthread_create(&thread_REFINED_APPROX_VC_2,NULL,(THREADFUNCPTR)(&Graph::Refined_APPROX_VC_2), g);
    pthread_join(thread_REFINED_APPROX_VC_2,NULL);
    return NULL;
}

int main(int argc, char **argv) {

    int TIMEOUT = 300; // SECONDS
    while (!cin.eof()) {

            g = new Graph();

            pthread_t thread_input;
            pthread_create(&thread_input,NULL,process_Input, NULL);
            pthread_join(thread_input,NULL);
            // cout<<"done input\n";
            // g->print_graph();
            if (cin.eof())
            return 0;


            struct timespec ts_cnf[2];
            struct timespec ts_vcs[2];
            pthread_t thread_group_VC1, thread_group_VC2;
            pthread_t thread_CNF_SAT_VC, thread_CNF_SAT_VC_3cnf;
            
            clock_gettime(CLOCK_REALTIME, &ts_cnf[0]);
            clock_gettime(CLOCK_REALTIME, &ts_cnf[1]);
            clock_gettime(CLOCK_REALTIME, &ts_vcs[0]);
            clock_gettime(CLOCK_REALTIME, &ts_vcs[1]);

            ts_cnf[0].tv_sec += TIMEOUT;
            ts_cnf[1].tv_sec += TIMEOUT;

            ts_vcs[0].tv_sec += TIMEOUT;
            ts_vcs[1].tv_sec += TIMEOUT;

            Data_mp[g->get_vertices()] = new Dataa();

            pthread_create(&thread_CNF_SAT_VC_3cnf,NULL,(THREADFUNCPTR)(&Graph::CNF_SAT_VC_3), g);
            pthread_create(&thread_CNF_SAT_VC,NULL,(THREADFUNCPTR)(&Graph::CNF_SAT_VC), g);
            pthread_create(&thread_group_VC1,NULL, group_1, NULL);
            pthread_create(&thread_group_VC2,NULL, group_2, NULL);
            

            pthread_join(thread_group_VC1, NULL);
            pthread_join(thread_group_VC2, NULL);

            // pthread_create(&thread_CNF_SAT_VC,NULL,(THREADFUNCPTR)(&Graph::CNF_SAT_VC), g);
            
            if (pthread_timedjoin_np(thread_CNF_SAT_VC,NULL, &ts_cnf[0]) != 0) {
                g->timed_out_0 = true;
            }
            if (pthread_timedjoin_np(thread_CNF_SAT_VC_3cnf,NULL, &ts_cnf[1]) != 0) {
                g->timed_out_1 = true;
            }

            g->display_answers(g->get_vertices());

    }
    return 0;
}
