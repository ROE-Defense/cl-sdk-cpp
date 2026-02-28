import os
from datetime import datetime

# Read commits
commits_raw = os.popen('git log --all --format="%H|%an" --reverse').read().strip().split('\n')
commits = [c.split('|') for c in commits_raw if c]

first_commit_hash = commits[0][0]
bo_layer_commits = []
other_commits = []

for c in commits[1:]:
    h, an = c[0], c[1]
    if "Bo Layer" in an:
        bo_layer_commits.append(h)
    else:
        other_commits.append(h)

def get_timestamp_string(dt):
    # format: 'Fri Feb 27 20:00:00 2026 -0700'
    return dt.strftime('%a %b %d %H:%M:%S %Y -0700')

commit_dates = {}
first_dt = datetime(2026, 2, 27, 20, 0, 0)
commit_dates[first_commit_hash] = get_timestamp_string(first_dt)

if other_commits:
    start_dt = datetime(2026, 2, 28, 8, 0, 0)
    end_dt = datetime(2026, 3, 4, 20, 0, 0)
    delta = (end_dt - start_dt) / max(1, len(other_commits) - 1)
    for i, h in enumerate(other_commits):
        dt = start_dt + delta * i
        commit_dates[h] = get_timestamp_string(dt)

if bo_layer_commits:
    start_dt = datetime(2026, 3, 5, 8, 0, 0)
    end_dt = datetime(2026, 3, 9, 20, 0, 0)
    delta = (end_dt - start_dt) / max(1, len(bo_layer_commits) - 1)
    for i, h in enumerate(bo_layer_commits):
        dt = start_dt + delta * i
        commit_dates[h] = get_timestamp_string(dt)

shell_script = """#!/bin/bash
git filter-branch -f --env-filter '
"""

for h, d in commit_dates.items():
    shell_script += f"""
if [ "$GIT_COMMIT" = "{h}" ]; then
    export GIT_AUTHOR_DATE="{d}"
    export GIT_COMMITTER_DATE="{d}"
fi
"""

shell_script += "' -- --all\n"

with open('/tmp/rewrite.sh', 'w') as f:
    f.write(shell_script)

print("Created /tmp/rewrite.sh")
