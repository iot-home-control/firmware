#!/usr/bin/env python3

import datetime
from pathlib import Path

cwd = Path(File('sources.lib').srcnode().abspath).parent
git_dir = cwd / ".git"

if git_dir.exists():
    with open(git_dir / "HEAD", "r") as f:
        head_ref = f.read().strip()
    refname = head_ref.split(":")[1].strip()
    with open(git_dir / refname, "r") as f:
        git_version = f.read().strip()[:7]
else:
    git_version = "unknown"

lines = [
    '#pragma once',
    '',
    f'const char *build_date = "{datetime.datetime.now(tz=datetime.timezone.utc).strftime("%Y-%m-%dT%H:%MZ")}";',
    f'const char *git_version = "{git_version}";',
    '',
]

with open("src/buildinfo.h", "w") as f:
    f.write("\n".join(lines))
