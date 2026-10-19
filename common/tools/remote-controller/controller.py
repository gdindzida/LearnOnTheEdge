"""Small, stateless SSH and rsync commands driven by a JSON manifest."""
import argparse
import json
from pathlib import Path, PurePosixPath
import re
import shlex
import shutil
import subprocess
import sys


class ControllerError(Exception):
    pass


def string(value, name):
    if not isinstance(value, str) or not value or "\x00" in value or "\n" in value:
        raise ControllerError(f"{name} must be a nonempty string without NUL or newline characters.")
    return value


def remote_path(value):
    """Return a safely quoted shell expression; expand only the remote home."""
    value = string(value, "Remote path")
    if ".." in value.split("/"):
        raise ControllerError("Remote paths cannot contain '..' components.")
    if value.startswith("/"):
        return shlex.quote(value)
    if value == "~":
        return '"$HOME"'
    if value.startswith("~") and not value.startswith("~/"):
        raise ControllerError("Use '~/' for remote home paths; '~user' is not supported.")
    relative = value[2:] if value.startswith("~/") else value
    return '"$HOME"/' + shlex.quote(relative)


def clean_path(value):
    expression = remote_path(value)
    relative = value[2:] if value.startswith("~/") else value
    if value == "~" or str(PurePosixPath(relative)) in ("/", "//", "."):
        raise ControllerError("clean cannot remove the remote root or home directory.")
    return expression


def load_manifest(path, action):
    try:
        manifest = json.loads(path.read_text())
    except (OSError, ValueError) as exc:
        raise ControllerError(f"Cannot read manifest {path}: {exc}") from exc
    if not isinstance(manifest, dict):
        raise ControllerError("Manifest must be a JSON object.")
    allowed = {"ssh_target", "type", "upload", "run", "download", "clean"}
    unknown = manifest.keys() - allowed
    if unknown:
        raise ControllerError("Unknown manifest fields: " + ", ".join(sorted(unknown)))
    target = manifest.get("ssh_target")
    if not isinstance(target, str) or not re.fullmatch(r"[A-Za-z0-9_][A-Za-z0-9_.@:%\[\]-]*", target):
        raise ControllerError("ssh_target must be a host alias or user@host; configure ports in ~/.ssh/config.")
    if manifest.get("type", "radxa-cpu") != "radxa-cpu":
        raise ControllerError("Only type 'radxa-cpu' is currently supported.")
    section = manifest.get(action)
    if action in ("upload", "download"):
        if not isinstance(section, list) or not section:
            raise ControllerError(f"{action} must be a nonempty list of source/destination mappings.")
        for entry in section:
            if not isinstance(entry, dict) or set(entry) != {"source", "destination"}:
                raise ControllerError(f"Each {action} entry must contain source and destination.")
            string(entry["source"], "source")
            string(entry["destination"], "destination")
            remote_path(entry["destination" if action == "upload" else "source"])
    elif action == "run":
        if not isinstance(section, dict) or set(section) - {"executable", "args", "cwd"}:
            raise ControllerError("run must be an object with executable, optional args, and optional cwd.")
        remote_path(section.get("executable"))
        if "cwd" in section:
            remote_path(section["cwd"])
        args = section.get("args", [])
        if not isinstance(args, list) or any(not isinstance(arg, str) or "\x00" in arg for arg in args):
            raise ControllerError("run.args must be a list of strings without NUL characters.")
    elif action == "clean":
        if not isinstance(section, list) or not section:
            raise ControllerError("clean must be a nonempty list of remote paths.")
        for value in section:
            clean_path(value)
    return manifest


def local_path(base, value):
    path = Path(value).expanduser()
    return (base / path).absolute() if not path.is_absolute() else path


class Remote:
    def __init__(self, target):
        self.target = target

    def ssh(self, command, capture=False):
        return subprocess.run(["ssh", "-T", self.target, command],
                              stdout=subprocess.PIPE if capture else None, text=capture)

    def checked(self, command, capture=False):
        result = self.ssh(command, capture)
        if result.returncode:
            raise ControllerError(f"Remote operation failed (SSH exit {result.returncode}).")
        return result.stdout if capture else None

    def inspect_path(self, path):
        # Resolve HOME on the remote side; protected rsync args then carry the literal path.
        expression = remote_path(path)
        script = (f'p={expression}; printf "%s\\n" "$p"; '
                  'if [ -d "$p" ]; then printf "directory\\n"; '
                  'elif [ -f "$p" ]; then printf "file\\n"; '
                  'elif [ -e "$p" ] || [ -L "$p" ]; then printf "other\\n"; '
                  'else printf "missing\\n"; fi')
        lines = self.checked(script, capture=True).splitlines()
        if len(lines) != 2 or not lines[0].startswith("/"):
            raise ControllerError("Unexpected remote path response (check SSH login output and remote HOME).")
        return lines[0], lines[1]

    def copy(self, source, destination):
        result = subprocess.run(["rsync", "-rltp", "--checksum", "--safe-links", "--protect-args",
                                 "-e", "ssh", "--", source, destination])
        if result.returncode:
            raise ControllerError(f"rsync failed (exit {result.returncode}).")

    def upload(self, entries, base):
        # Validate all local inputs before changing the remote side.
        sources = [local_path(base, entry["source"]) for entry in entries]
        for source in sources:
            if not source.exists() or source.is_symlink() or not (source.is_file() or source.is_dir()):
                raise ControllerError(f"Upload source must be an existing regular file or directory: {source}")
        for entry, source in zip(entries, sources):
            destination, kind = self.inspect_path(entry["destination"])
            if source.is_file() and kind not in ("missing", "file"):
                raise ControllerError(f"File destination is not a regular file: {destination}")
            if source.is_dir() and kind not in ("missing", "directory"):
                raise ControllerError(f"Directory destination is not a directory: {destination}")
            parent = destination if source.is_dir() else str(PurePosixPath(destination).parent)
            self.checked("mkdir -p -- " + shlex.quote(parent))
            suffix = "/" if source.is_dir() else ""
            self.copy(str(source) + suffix, f"{self.target}:{destination}" + suffix)

    def download(self, entries, base):
        for entry in entries:
            source, kind = self.inspect_path(entry["source"])
            if kind not in ("file", "directory"):
                raise ControllerError(f"Download source is not a regular file or directory: {source}")
            destination = local_path(base, entry["destination"])
            if destination.is_symlink():
                raise ControllerError(f"Download destination cannot be a symlink: {destination}")
            if kind == "file" and destination.is_dir():
                raise ControllerError(f"File destination is a directory: {destination}")
            (destination if kind == "directory" else destination.parent).mkdir(parents=True, exist_ok=True)
            suffix = "/" if kind == "directory" else ""
            self.copy(f"{self.target}:{source}" + suffix, str(destination) + suffix)

    def run(self, specification):
        # Resolve executable before cd, so relative remote paths always mean HOME-relative.
        executable = remote_path(specification["executable"])
        cwd = remote_path(specification.get("cwd", "~"))
        args = shlex.join(specification.get("args", []))
        return self.ssh(f'executable={executable}; cd -- {cwd} && exec "$executable" {args}').returncode

    def clean(self, paths):
        for path in paths:
            # Reject home/root even when an absolute path names them. Resolve the parent
            # to reject ancestor symlinks; a final symlink is removed without following it.
            expression = clean_path(path)
            script = f'''p={expression}
while [ "${{p%/}}" != "$p" ]; do p=${{p%/}}; done
parent=$(dirname -- "$p")
name=$(basename -- "$p")
if [ ! -d "$parent" ]; then
    printf 'Already absent: %s\\n' "$p"
    exit 0
fi
resolved=$(cd -- "$parent" && pwd -P) || exit 1
home=$(cd -- "$HOME" && pwd -P) || exit 1
candidate="$resolved/$name"
if [ "$name" = . ] || [ "$name" = .. ] || [ "$candidate" = "$home" ] || [ "$resolved" != "$parent" ]; then
    printf '%s\\n' 'Refusing unsafe cleanup path' >&2
    exit 1
fi
if [ -e "$p" ] || [ -L "$p" ]; then
    rm -rf -- "$p" || exit $?
    printf 'Removed: %s\\n' "$p"
else
    printf 'Already absent: %s\\n' "$p"
fi'''
            self.checked(script)


def main(argv=None):
    parser = argparse.ArgumentParser(description="Copy files and run an executable over SSH using a JSON manifest.")
    commands = parser.add_subparsers(dest="command", required=True)
    for command in ("upload", "run", "download", "clean"):
        commands.add_parser(command).add_argument("manifest", type=Path)
    args = parser.parse_args(argv)
    try:
        manifest_path = args.manifest.absolute()
        manifest = load_manifest(manifest_path, args.command)
        required = ("ssh", "rsync") if args.command in ("upload", "download") else ("ssh",)
        for tool in required:
            if not shutil.which(tool):
                raise ControllerError(f"Required local tool is missing: {tool}")
        remote = Remote(manifest["ssh_target"])
        if args.command in ("upload", "download"):
            getattr(remote, args.command)(manifest[args.command], manifest_path.parent)
        elif args.command == "run":
            return remote.run(manifest["run"])
        else:
            remote.clean(manifest["clean"])
        return 0
    except (ControllerError, OSError) as exc:
        print(f"remote-controller: {exc}", file=sys.stderr)
        return 1
    except KeyboardInterrupt:
        print("remote-controller: interrupted", file=sys.stderr)
        return 130
