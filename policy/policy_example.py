from google.protobuf.json_format import MessageToJson
import policy_pb2
import sys

if (len(sys.argv) != 3) :
    print("usage: python policy_example.py <jsonoutput> <protobufoutput>")
    sys.exit(-1)

policy = policy_pb2.policy()

tcp = policy.process_omit_matches.tcp_dest_ip_matches.add()
tcp.match_str = "127.0.0.1"

f_match = policy.process_omit_matches.file_matches.add()
f_match.match_str = "/home/ashton/criu/examples/missing_file/a.txt"

exe_match = policy.process_omit_matches.exe_name_matches.add()
exe_match.match_str = "/bin/sleep"

mem_match = policy.process_omit_matches.memory_matches.add()
mem_match.match_str = "awmagic"

#rt = policy.tasks.add()
#rt.match.magic = "abc123"

#ra = rt.raw_actions.add()
#ra.offset = 4
#ra.replace_bytes = "abcdef"

#da1 = rt.deref_actions.add()
#da1.offset = 8
#da1.da.offset = 0
#da1.da.da.offset = 0
#da1.da.da.ra.offset = 0
#da1.da.da.ra.replace_bytes = "5678"

#ta = policy.tcp_assertions.add()
#ta.ip = "192.168.1.1"
#ta.destroy = True

jsonObj = MessageToJson(policy)
print(jsonObj)

with open(sys.argv[1], 'w') as f:
    f.write(str(jsonObj))

with open(sys.argv[2], 'wb') as f:
    f.write(policy.SerializeToString())


