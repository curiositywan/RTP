/* This program is used to run a batch query
 * Author: Nimisha Srinivasa
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

#include "SingleQuerySearcher.h"

using namespace std;

typedef chrono::system_clock Clock;
string rel_path_to_target_dir3 = "./";

// NOTE: the query_file contains the cleaned, stemmed data
int main(int argc, char** argv){
	if(argc<=2){
		cout << "Illegal num of args. Format: ./batch_query_search <query_file> <top_k>" << endl;
		exit(1);
	}
	string query_file_name = string(argv[1]);
	int top_k = atoi(argv[2]);

	ifstream fin;
	string line;
	bool first_time = true;
	int curr_query_num=1;

    chrono::time_point<Clock> start, end;
    chrono::duration<double> elapsed_seconds;
    double phase1_time_for_all_queries = 0;
    double phase2_time_for_all_queries = 0;
    start = Clock::now();  // start ticking

	SingleQuerySearcher* single_searcher = new SingleQuerySearcher();
	single_searcher->top_k = top_k;

    fin.open(rel_path_to_target_dir3 + query_file_name);
    
    while (std::getline(fin, line)){
    	cout << "####################Processing query number: " << curr_query_num << "####################" << endl;

        single_searcher->full_query = line;

    	if(first_time){
    		single_searcher->runSearch_without_preprocess();
    		first_time = false;
            phase1_time_for_all_queries += single_searcher->total_phase1_time_per_query;
            phase2_time_for_all_queries += single_searcher->total_phase2_time_per_query;
    	}else{
    		single_searcher->searchAgain_without_preprocess();
            phase1_time_for_all_queries += single_searcher->total_phase1_time_per_query;
            phase2_time_for_all_queries += single_searcher->total_phase2_time_per_query;
    	}

    	cout << "#################### Done with query number: " << curr_query_num << "####################" << endl;
    	curr_query_num ++;
    }
    delete single_searcher;
    curr_query_num--;
    
    end = Clock::now();
    elapsed_seconds = end - start;
    cout << "The batch query took: " << elapsed_seconds.count() << endl;

    cout << "Batch query stats:"<< endl;
    cout << "total time for phase1=" << phase1_time_for_all_queries << " avg. phase 1 search time per query=" << phase1_time_for_all_queries/curr_query_num << endl;
    cout << "total time for phase2=" << phase2_time_for_all_queries << " avg. phase 2 search time per query=" << phase2_time_for_all_queries/curr_query_num << endl;
}