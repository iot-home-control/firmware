#!/usr/bin/env python3

import datetime
import subprocess
from pathlib import Path
import shutil

cwd = Path(File('sources.lib').srcnode().abspath).parent
git_dir = cwd / ".git"

if git_dir.exists():
    if shutil.which("git") is None:
        with open(git_dir / "HEAD", "r") as f:
            head_ref = f.read().strip()
        refname = head_ref.split(":")[1].strip()
        with open(git_dir / refname, "r") as f:
            git_version = f.read().strip()[:7] + "?"
    else:
        git_version = subprocess.check_output(["git", "describe", "--always", "--dirty=*"]).strip()
else:
    git_version = "unknown"

lines = [
    f'const char *build_date = "{datetime.datetime.now(tz=datetime.timezone.utc).strftime("%Y-%m-%dT%H:%MZ")}";',
    f'const char *git_version = "{git_version}";',
    '',
]

with open("src/buildinfo.c", "w") as f:
    f.write("\n".join(lines))
