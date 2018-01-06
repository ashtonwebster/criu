from google.protobuf.json_format import MessageToJson
import policy_pb2
import sys

if (len(sys.argv) != 3) :
    print("usage: python policy_example.py <jsonoutput> <protobufoutput>")
    sys.exit(-1)

redact_tasks_entry = policy_pb2.redact_tasks_entry()
rt = redact_tasks_entry.tasks.add()
rt.match.magic = "abc123"

ra = rt.raw_actions.add()
ra.offset = 4
ra.replace_bytes = "abcdef"

da1 = rt.deref_actions.add()
da1.offset = 8
da1.da.offset = 0
da1.da.da.offset = 0
da1.da.da.ra.offset = 0
da1.da.da.ra.replace_bytes = "5678"

jsonObj = MessageToJson(redact_tasks_entry)
print(jsonObj)

with open(sys.argv[1], 'w') as f:
    f.write(str(jsonObj))

with open(sys.argv[2], 'wb') as f:
    f.write(redact_tasks_entry.SerializeToString())


