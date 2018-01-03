#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

int main(int args, char **argv) {
	xmlDocPtr doc;
	xmlNodePtr cur;
	doc = xmlParseFile(argv[1]);
	cur = xmlDocGetRootElement(doc);
	cur = cur->xmlChildrenNode->next->xmlChildrenNode->next->xmlChildrenNode->next;
	printf("%s\n", xmlGetProp(cur, (const xmlChar *) "ID"));
	xmlCleanupParser();
}
