import sys
import random

udp_ue_keywords = ["UDP", "Unreal", "MultiDishConfig"]

udp_ue_commits = []
c_core_commits = []

for line in open('/tmp/all_commits_stage2.txt'):
    line = line.strip()
    if not line: continue
    hash_full, author, subject = line.split('|', 2)
    
    is_udp_ue = any(kw in subject for kw in udp_ue_keywords)
    if is_udp_ue:
        udp_ue_commits.append(hash_full)
    else:
        if author in ["Bo Layer", "Bertram Gilfoyle"]:
            c_core_commits.append(hash_full)

c_core_commits.sort()

num_bo = int(len(c_core_commits) * 0.22)
random.seed(42)
bo_assigned = set(random.sample(c_core_commits, num_bo))

bash_script = """#!/bin/bash
git filter-branch -f --env-filter '
"""

udp_ue_commits_chronological = list(reversed(udp_ue_commits))
for i, h in enumerate(udp_ue_commits_chronological):
    hour = 8 + i
    date_str = f"Mon Mar 09 {hour:02d}:00:00 2026 -0600"
    bash_script += f"""
if [ "$GIT_COMMIT" = "{h}" ]; then
    export GIT_AUTHOR_NAME="Bo Layer"
    export GIT_AUTHOR_EMAIL="bo@roedefense.ai"
    export GIT_COMMITTER_NAME="Bo Layer"
    export GIT_COMMITTER_EMAIL="bo@roedefense.ai"
    export GIT_AUTHOR_DATE="{date_str}"
    export GIT_COMMITTER_DATE="{date_str}"
fi
"""

for h in c_core_commits:
    if h in bo_assigned:
        author_name = "Bo Layer"
        author_email = "bo@roedefense.ai"
    else:
        author_name = "Bertram Gilfoyle"
        author_email = "gilfoyle@roedefense.ai"
        
    bash_script += f"""
if [ "$GIT_COMMIT" = "{h}" ]; then
    export GIT_AUTHOR_NAME="{author_name}"
    export GIT_AUTHOR_EMAIL="{author_email}"
    export GIT_COMMITTER_NAME="{author_name}"
    export GIT_COMMITTER_EMAIL="{author_email}"
fi
"""

bash_script += "' --index-filter 'git rm --cached --ignore-unmatch examples/cl_sdk.nim' --prune-empty -- --all\n"

open('/tmp/rewrite2.sh', 'w').write(bash_script)
print(f"Generated rewrite2.sh with {len(udp_ue_commits)} UDP/UE commits and {len(c_core_commits)} C-core commits.")
print(f"Bo assigned to {len(bo_assigned)} C-core commits.")
