#ifndef __POLICY_PARSE_H__
#define __POLICY_PARSE_H__

#include "cr_options.h"

#define MAX_MAGIC_LEN 1024
#define MAX_REPLACE_LEN 1024

// add more to this... such as matching on specific fields
struct redact_match {
	char magic[MAX_MAGIC_LEN];
	unsigned int magic_len;
};

// probably should be an enum...
#define RAW_TYPE 1
#define DEREF_TYPE 2

struct raw_action {
	int type;
	int offset;
	char replace_bytes[MAX_REPLACE_LEN];
	unsigned int rb_len;
	struct raw_action *next;
};

union post_action {
	struct raw_action *ra;
	struct deref_action *da;
}; // what to do after dereferencing

struct deref_action {
	int type;
	int offset; // offset from magic number or previous pointer address
	union post_action post_action;
	struct deref_action *next;
};

struct redact_task {
	struct redact_match match;
	// TODO: change names to *_head instead of *_actions
	struct raw_action *raw_actions;
	struct deref_action *deref_actions;
	struct redact_task *next;
};

struct redact_task *parse_policy(char *policy_path);

#endif /* __POLICY_PARSE_H__ */

