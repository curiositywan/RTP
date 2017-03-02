#include <stdlib.h>
#include <iostream>

#include "SingleQuerySearcher.h"
#include "../pre_process_query/Stem.h"

#define TRUE 1
#define FALSE 0

#define LETTER(ch) (isupper(ch) || islower(ch))
static char * s;         /* buffer for words tobe stemmed */
#define INC 50           /* size units in which s is increased */
static int i_max = INC;  /* maximum offset in s */


using namespace std;

void SingleQuerySearcher::pre_process_query(){
	full_query = doStemClean(full_query);
}

void SingleQuerySearcher::run_phase1_lucene_jar(){
	system("rm -rf ./target/search_frag.txt");

	//generate SEARCH_FRAGMENT_FILE_NAME
	string command = "java -jar ../src/phase1/search_step/lucene_search.jar " + full_query;
	system(command.c_str());
}


void SingleQuerySearcher::generate_phase1_results(){
  phase1_searcher = new Phase1_Searcher();
  phase1_searcher->query_len = num_words_in_query;
  phase1_searcher->query.clear();
  for (int i=0; i<phase1_searcher->query_len; i++)
        phase1_searcher->query.push_back(query_words_arr[i]);
  phase1_searcher->runSearch();
}

void SingleQuerySearcher::generate_phase1_results_again(){
  phase1_searcher->query_len = num_words_in_query;
  phase1_searcher->query.clear();
  for (int i=0; i < phase1_searcher->query_len; i++)
        phase1_searcher->query.push_back(query_words_arr[i]);
  phase1_searcher->runSearchAgain();
}

void SingleQuerySearcher::run_phase2_search(){
  cout << "Came to phase2 Search :" << endl;
  string command = "./phase2_search "+ to_string(top_k) + " " + to_string(num_words_in_query) + " " + full_query;
  system(command.c_str()); 
}

string SingleQuerySearcher::stemstring(struct stemmer * z, string str_to_stem)
{
  int j=0;
  string result="";
  //char* result=(char*) malloc(strlen(string1)*sizeof(char));  //assuming stemmed string is same or lesser in size than orig string
  while(TRUE)
   {  int ch = str_to_stem[j++];
      if (ch == 0) return result;
      if (LETTER(ch))
      {  int i = 0;
         while(TRUE)
         {  if (i == i_max)
            {  i_max += INC;
               s = (char*)realloc(s, i_max + 1);
            }
            ch = tolower(ch); //forces lower case

            s[i] = ch; i++;
            ch = str_to_stem[j++];
            if (!LETTER(ch)) {j--; break; }
         }
         s[stem(z, s, i - 1) + 1] = 0;
         /* the previous line calls the stemmer and uses its result to
            zero-terminate the string in s */
         result.append(s);
      }

      else{
        char char_to_append = (char) ch;
        result.push_back(char_to_append);
      } 
   }
}

string SingleQuerySearcher::doStem(string str){
    struct stemmer * z = create_stemmer();
    s = (char *) malloc(i_max + 1);
    string result = stemstring(z, str);
    free(s);
    free_stemmer(z);
    return result;
}

bool SingleQuerySearcher::isSpecialChar(char c){
  if((c>='a' && c<='z') || (c >='A' && c<='Z') || (c>='0' && c<='9'))
    return false;
  else 
    return true;
  
}

string SingleQuerySearcher::doClean(string str){
  for(int i=0; i< str.length(); i++){
    if(isSpecialChar(str[i]))
      str[i]=' ';
  }
  return str;
}


string SingleQuerySearcher::doStemClean(string str)
{

  string res = doStem(str);
  res = doClean(res);
  return res;
}

void SingleQuerySearcher::runSearch_without_preprocess(){
    run_phase1_lucene_jar();
    generate_phase1_results();
    run_phase2_search();
}

void SingleQuerySearcher::searchAgain_without_preprocess(){
    run_phase1_lucene_jar();
    generate_phase1_results_again();
    run_phase2_search();
}

void SingleQuerySearcher::runSearch(){
    pre_process_query();
    run_phase1_lucene_jar();
    generate_phase1_results();
    run_phase2_search();
}