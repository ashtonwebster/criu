import policy_pb2 
from google.protobuf.json_format import Parse 
import sys

if len(sys.argv) != 3:
    print("usage: python json2pb.py <input> <output>")
    sys.exit(-1)

with open(sys.argv[1], 'rb') as f:
    jsonstr = f.read()

p = Parse(jsonstr, policy_pb2.redact_tasks_entry())

with open(sys.argv[2], 'wb') as f:
    f.write(p.SerializeToString())
