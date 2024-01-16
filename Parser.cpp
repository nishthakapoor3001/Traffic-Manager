#include "Parser.h"

string ltrim(const string &s)
{
    size_t start = s.find_first_not_of(" ");
    return (start ==  string::npos) ? "" : s.substr(start);
}
 
string rtrim(const string &s)
{
    size_t end = s.find_last_not_of(" ");
    return (end ==  string::npos) ? "" : s.substr(0, end + 1);
}
 
string trim(const string &s) {
    return rtrim(ltrim(s));
}

bool query_E(Graph* g, string edges_list)
{
    // cout<<"V "<<g->v_num<<endl;
    // cout<<edges_list<<endl;
    unsigned long long pos = 0, t_pos = 0;
    string delimiter = ">,<";
    string token;
    int start = 0;
    int source,destination;

    edges_list.erase(0, 2);
    while ((pos = edges_list.find(delimiter)) && pos != std::string::npos) { 
        if( g->first_run) start = 2;
        else start = 0;
        // cout<<" first = "<<g->first_run<<endl;
        g->first_run = false;
        token = edges_list.substr(0, pos);
		t_pos = token.find(',');
  		if (t_pos == string::npos) { cerr<<"Error: invalid edges input"; return false;}
        // cout<<" start"<<start <<" delim == "<<t_pos<<endl;
		source = stoi(edges_list.substr(start,t_pos - start));
        // cout<<"for desti  start"<<start <<" delim == "<<t_pos<<endl;
		destination = stoi(edges_list.substr(t_pos+1 , token.length() - t_pos - 1));

	g->add_edges(source,destination);

        // g->print_graph();


		edges_list.erase(0, pos + delimiter.length());
	}
    
	token = edges_list;
    t_pos = token.find(','); 
    
    if (t_pos  == string::npos) { 
    	return true;
    }

    if(g->first_run) start = 2;
    else start = 0;

    t_pos = token.find(','); 

    source = stoi(edges_list.substr(start,t_pos - start));
    destination = stoi(edges_list.substr(t_pos+1 , token.length() - t_pos - 3));
    
	if (g->add_edges(source,destination) == false) return false;
    // g->print_graph();
    return true;

}

void query_V(Graph* g, int V)
{
    g->empty_graph();
    g->set_vertices(V);
}