#!/usr/bin/env python3
import sys

with open(sys.argv[1], 'r') as f:
    lines = f.readlines()

new_lines = []
enhance_commit_hash = None

for line in lines:
    if line.startswith('pick') and 'docs: enhance readme formatting' in line:
        parts = line.split(' ', 2)
        enhance_commit_hash = parts[1]

for line in lines:
    if line.startswith('pick'):
        parts = line.split(' ', 2)
        msg = parts[2].strip() if len(parts) >= 3 else ""
        if 'docs: enhance readme formatting' in msg:
            continue
        
        if 'chore: sanitize telemetry comments' in msg:
            new_lines.append(f"reword {parts[1]} {msg}\n")
        elif 'chore: sanitize AI hallmarks' in msg:
            new_lines.append(f"reword {parts[1]} {msg}\n")
        else:
            new_lines.append(line)
        
        if 'docs: update docs and readme' in msg:
            if enhance_commit_hash:
                new_lines.append(f"squash {enhance_commit_hash} docs: enhance readme formatting\n")
    else:
        new_lines.append(line)

with open(sys.argv[1], 'w') as f:
    f.writelines(new_lines)
