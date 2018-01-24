#include "policy_parse.h"
#include "images/policy.pb-c.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"
#include "cr_options.h"

#define MAX_MESSAGE_SIZE 1024
#define MAX_OMITTED_PROCESSES 1024
#define OMIT_IMG_FILENAME "omit.img"

static OmittedProcesses omitted_processes;

int parse_hex(char *str, unsigned char **out_hex, size_t *out_size) {
	*out_size = strlen(str) % 2 == 0 ? 
		strlen(str) / 2 : 
		strlen(str) / 2 + 1;
	assert((*out_hex = malloc(*out_size)) != 0);
	size_t i;
	char *pos = str;	
	for (i = 0; i < *out_size; i++, pos += 2) {
        sscanf(pos, "%2hhx", &((*out_hex)[i]));
    }
	return 0;
}

// out size is in number of longs
/*int parse_hex(char *str, unsigned long **out_hex, size_t *out_size) {
	// number of characters per 8-ish bytes
	unsigned int chars_per_long = sizeof(unsigned long) * 2;
	// size = strlen(string) / chars_per_long + (strlen(string) % chars_per_long == 0 ? 0 : 1)
	*out_size = ((strlen(str) - 1) % chars_per_long == 0) ?
		(strlen(str) - 1) / chars_per_long :
		(strlen(str) - 1) / chars_per_long + 1;
	// malloc(size)
	assert((*out_hex = malloc(*out_size * sizeof(unsigned long))) != 0);
	// for each grouping of (sizeof(unsigned long) * 2) characters
	int i;
	char *cur_ptr = str, *next_ptr = NULL;
	for (i = 0; i < *out_size; i++) {
		// strtoul
		*out_hex[i] = strtoul(cur_ptr, &next_ptr, 16);
		if (errno != 0) {
			pr_info("error: errno: %d\n", errno);
		}
	}
	return 0;
}*/

void init_omitted_processes() {
	OmittedProcesses my_omitted_processes = OMITTED_PROCESSES__INIT;
	omitted_processes = my_omitted_processes;
	omitted_processes.n_omitted_processes = 0;
	omitted_processes.omitted_processes = 
		calloc(sizeof(OmittedProcess *), MAX_OMITTED_PROCESSES);
	assert(omitted_processes.omitted_processes);
}

int add_omitted_process(int pid, char *reason) {
	OmittedProcess *new_op = xmalloc(sizeof(OmittedProcess));
	OmittedProcess init_op = OMITTED_PROCESS__INIT;
	memcpy(new_op, &init_op, sizeof(OmittedProcess));
	omitted_processes.omitted_processes[omitted_processes.n_omitted_processes] =
		new_op;
	new_op->pid = pid;
	new_op->reason = reason;
	omitted_processes.n_omitted_processes++;
	return 0;
}

int dump_omitted_processes() {
	size_t len = omitted_processes__get_packed_size(&omitted_processes);
	void *buf = malloc(len);
	omitted_processes__pack(&omitted_processes, buf);
	// write to file
	int fd = open(OMIT_IMG_FILENAME, O_WRONLY | O_CREAT, 0644);
	if (fd < 0) {
		pr_info("could not open %s, fd=%d\n", OMIT_IMG_FILENAME, fd);
		assert(fd >= 0);
	}
	assert(write(fd, buf, len) == len);
	close(fd);
	free(buf);
	return 0;
}

OmittedProcesses *read_omitted_porcesses(void) {
	const uint8_t buf[MAX_MESSAGE_SIZE];
	int real_size;
	int fd = open(OMIT_IMG_FILENAME, O_RDONLY);
	if (fd < 0) {
		pr_info("No omitted processes\n");
		return NULL;
	}

	assert((real_size = read(fd, (void*)buf, MAX_MESSAGE_SIZE)) >= 0);
	close(fd);
	return omitted_processes__unpack(NULL, real_size, buf);
}

static struct raw_action *parse_raw_action(RawActionEntry *pb_ra) {
	struct raw_action *parsed_ra = xmalloc(sizeof(struct raw_action));
	parsed_ra->type = RAW_TYPE;
	parsed_ra->offset = pb_ra->offset;
	memcpy(parsed_ra->replace_bytes, pb_ra->replace_bytes, 
			strlen(pb_ra->replace_bytes) + 1);
	parsed_ra->rb_len = strlen(pb_ra->replace_bytes) + 1;
	// default next to NULL
	parsed_ra->next = NULL;
	return parsed_ra;
}

static struct deref_action *parse_deref_action(DerefActionEntry *pb_da) {
	struct deref_action *parsed_da = xmalloc(sizeof(struct deref_action));
	parsed_da->type = DEREF_TYPE;
	parsed_da->offset = pb_da->offset;
	if (pb_da->ra) {
		parsed_da->post_action.ra = parse_raw_action(pb_da->ra);
		parsed_da->post_action.ra->next = NULL;
	} else if (pb_da->da) {
		parsed_da->post_action.da = parse_deref_action(pb_da->da);
		parsed_da->post_action.da->next = NULL;
	} else {
		pr_err("error parsing protobuf policy: at least one of {ra"
				", da} must be non-null");
		return NULL;
	}
	// default next to null
	parsed_da->next = NULL;
	return parsed_da;
}

// parse protobuf style task to struct redact_task
static struct redact_task *parse_tasks(RedactTaskEntry **tasks, int n_tasks) {
	int i, j;
	struct redact_task *ret_tasks = NULL, *prev_task = NULL;

	for (i = 0; i < n_tasks; i++) {
		RedactTaskEntry *pb_task = tasks[i];
		struct redact_task *parsed_task = malloc(sizeof(struct redact_task));
		assert(parsed_task);
		if (i == 0) ret_tasks = parsed_task;
		if (prev_task) prev_task->next = parsed_task;
		memcpy(parsed_task->match.magic, pb_task->match->magic, 
				strlen(pb_task->match->magic));
		parsed_task->match.magic_len = strlen(parsed_task->match.magic) + 1;

		struct raw_action *prev_ra = NULL;
		parsed_task->raw_actions = NULL;
		for (j = 0; j < pb_task->n_raw_actions; j++) {
			struct raw_action *parsed_ra = 
				parse_raw_action(pb_task->raw_actions[j]);
			if (j == 0) parsed_task->raw_actions = parsed_ra;
			if (prev_ra) prev_ra->next = parsed_ra;
			prev_ra = parsed_ra;
		}

		struct deref_action *prev_da = NULL;
		parsed_task->deref_actions = NULL;
		for (j = 0; j < pb_task->n_deref_actions; j++) {
			DerefActionEntry *pb_da = pb_task->deref_actions[j];
			struct deref_action *parsed_da = parse_deref_action(pb_da);
			if (j == 0) parsed_task->deref_actions = parsed_da;
			if (prev_da) prev_da->next = parsed_da;
			prev_da = parsed_da;
		}
		prev_task = parsed_task;
		parsed_task->next = NULL;
	}
	return ret_tasks;
}

struct policy *parse_policy(char *policy_path) {
	const uint8_t buf[MAX_MESSAGE_SIZE];
	int real_size;
	int fd = open(policy_path, O_RDONLY);
	assert(fd >= 0);

	assert((real_size = read(fd, (void*)buf, MAX_MESSAGE_SIZE)) > 0);
	close(fd);
	Policy *policy = policy__unpack(NULL, real_size, buf);
	struct policy *mypolicy = xmalloc(sizeof(struct policy));
	assert(policy!= 0);

	mypolicy->tasks = parse_tasks(policy->tasks, policy->n_tasks);
	mypolicy->n_tcp_assertions = policy->n_tcp_assertions;
	mypolicy->tcp_assertions = policy->tcp_assertions;
	mypolicy->process_omit_matches = policy->process_omit_matches;
	// free later
	//policy__free_unpacked(policy, NULL);
	return mypolicy;
}
