#include <regex.h>
#include <stdio.h>
#include "regex_internal.h"
#include <stdlib.h>
#include <time.h>
#define MAX_OUTPUT 1024

int generate_random(char *pattern, char *const output, int max_len) {
	//generate the compiled regex structure
	memset(output, '\0', max_len);
	regex_t comp_regex;
	int r;
	if ((r = regcomp (&comp_regex, pattern, REG_EXTENDED)) != 0) {
		return r;
	}
	// need to use the internal type re_dfa_t to access dfa
	struct re_dfa_t *dfa = (re_dfa_t*)comp_regex.buffer;

	int count = dfa->init_state->non_eps_nodes.nelem;
	int cur_node_ind = dfa->init_state->non_eps_nodes.elems[rand() % count]; 
	re_token_t cur_node = dfa->nodes[cur_node_ind];
	int c, output_ind = 0;

	while (cur_node.type != END_OF_RE && output_ind < max_len) {
		//generate
		if (cur_node.type == CHARACTER) {
			output[output_ind++] = cur_node.opr.c;
		} else if (cur_node.type == SIMPLE_BRACKET) {
			char valid_chars[128];
			int vc_count = 0;
			for (c = 0; c < 128; c++) {
				if (bitset_contain(cur_node.opr.sbcset, c)) {
					valid_chars[vc_count++] = c;
				}
			}
			output[output_ind++] = valid_chars[rand() % vc_count];
		} else if (cur_node.type == OP_PERIOD) {
			output[output_ind++] = 32 + (rand() % 95);
		}
		
		//transition
		if (dfa->nexts[cur_node_ind] > 0) {
			// only one option
			cur_node_ind = dfa->nexts[cur_node_ind];
			cur_node = dfa->nodes[cur_node_ind];
		} else if (dfa->edests[cur_node_ind].nelem > 0) {
			// epsilon destinations (for |, *, etc)
			cur_node_ind = dfa->edests[cur_node_ind].elems[rand() %
				dfa->edests[cur_node_ind].nelem];
			cur_node = dfa->nodes[cur_node_ind];
		} else {
			// nowhere to go...
			return -1;
		}
	}
	return 0;

}

/*
int main() {
	char output[MAX_OUTPUT];
	generate_random("a|b", output, 1024);
	printf("%s\n", output);
	return 0;
}
*/
/*int main() {
	regex_t comp_regex;
	srand(time(0));
	char output[MAX_OUTPUT];
	memset(output, '\0', 1024);

	re_dfa_t *dfa;
	//int r = regcomp (&comp_regex, "[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}", REG_EXTENDED);
	//int r = regcomp (&comp_regex, "((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)", REG_EXTENDED);
	//int r = regcomp (&comp_regex, "192\\.168\\.1\\.1", REG_EXTENDED);
	int r = regcomp (&comp_regex, "0[x][0-9A-F]{32}", REG_EXTENDED);
	//int r = regcomp (&comp_regex, "a|b", REG_EXTENDED);

	dfa = (re_dfa_t*)comp_regex.__buffer;
	r = regexec(&comp_regex, "255.255.255.255", 0, NULL, 0);
	//r = regexec(&comp_regex, "12", 0, NULL, 0);
	printf("test match: %d\n", r);

	generate_random(dfa, output, 1024);
	printf("%s\n", output);
	
	return 0;
}*/
