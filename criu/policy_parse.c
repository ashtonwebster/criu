#include "policy_parse.h"
#include "images/policy.pb-c.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "util.h"
#include "cr_options.h"

#define MAX_MESSAGE_SIZE 1024

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
static struct redact_task *read_policy(char *policy_path) {
	const uint8_t buf[MAX_MESSAGE_SIZE];
	int real_size;
	int fd = open(policy_path, O_RDONLY);
	int i, j;
	struct redact_task *ret_tasks = NULL, *prev_task = NULL;
	assert(fd >= 0);

	assert((real_size = read(fd, (void*)buf, MAX_MESSAGE_SIZE)) > 0);
	close(fd);
	RedactTasksEntry *tasks = 
		redact_tasks_entry__unpack(NULL, real_size, buf);
	assert(tasks != 0);
	pr_info("%s\n", tasks->tasks[0]->match->magic);

	for (i = 0; i < tasks->n_tasks; i++) {
		RedactTaskEntry *pb_task = tasks->tasks[i];
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

	redact_tasks_entry__free_unpacked(tasks, NULL);
	return ret_tasks;
}


struct redact_task *parse_policy(char *policy_path) {
	return read_policy(policy_path);
}
