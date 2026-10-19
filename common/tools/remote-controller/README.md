# remote-controller

Four commands, each taking a JSON manifest:

```sh
remote-controller upload ./remote.json
remote-controller run ./remote.json
remote-controller download ./remote.json
remote-controller clean ./remote.json
```

The manifest contains the SSH target and the operations. There is no saved
configuration, application ID, deployment registry, or environment override.
Each command reads the manifest again and performs only its named operation.

## Setup

Local requirements: Linux, Python 3.8+, OpenSSH client, and rsync.
Remote requirements: a POSIX shell, SSH server, and rsync. Remote Python is not
needed. Configure authentication, custom ports, and aliases in `~/.ssh/config`.

From the repository root:

```sh
export PATH="$PWD/common/tools/remote-controller:$PATH"
```

Keep the executable and `controller.py` together. Add the equivalent absolute
path to your shell configuration if desired.

## Manifest

Save this as `remote.json` in your project directory:

```json
{
  "ssh_target": "radxa@192.168.1.50",
  "upload": [
    {"source": "build/linux-aarch64/basic_app", "destination": "~/demo/basic_app"},
    {"source": "resources", "destination": "~/demo/resources"}
  ],
  "run": {
    "executable": "~/demo/basic_app",
    "cwd": "~/demo",
    "args": []
  },
  "download": [
    {"source": "~/demo/results", "destination": "results"}
  ],
  "clean": ["~/demo"]
}
```

- `ssh_target`: SSH alias or `user@host`.
- `upload`: local source → remote destination mappings.
- `run`: remote executable, optional argument array, and optional working directory.
- `download`: remote source → local destination mappings.
- `clean`: explicit remote files or directories to remove. Directories are removed recursively.

Only the section for the command being invoked is required. For example, a
run-only manifest needs just `ssh_target` and `run.executable`. The optional
`type` field accepts `"radxa-cpu"` (the default); other types are not implemented.

Local relative paths resolve against the manifest's directory, regardless of
where you invoke the command. Local absolute paths and `~/` are also accepted.
Remote paths can be absolute or start with `~/`; other relative remote paths
also resolve against the remote user's home. `run.cwd` defaults to that home.
The executable path is resolved independently of `cwd`.

A file mapping names the exact destination file, allowing renames. A directory
mapping copies the directory's **contents** into the destination directory; a
trailing slash does not change this behavior. Missing destination parents are
created. Matching files are overwritten, but unrelated destination files are
retained. Transfers use rsync checksums and preserve executable permissions.
Internal symlinks are transferred by rsync; links pointing outside the copied
tree are skipped. A top-level upload source must be a regular file or directory.

Paths and arguments are literal: there is no glob, variable, or shell command
expansion, except the documented home-directory handling. Remote paths cannot
contain `..` components. Cleanup refuses root, the remote home itself, and paths
traversing symlink parents; a final symlink is removed without following it.

## What each command does

`upload` copies the listed files. It does not build the application, install
libraries, remove stale files, or run anything. Application library lookup is
your build's responsibility.

`run` launches the executable in the foreground and forwards its stdout/stderr
through SSH. It returns the SSH/application exit code. It does not upload files,
create results directories, capture logs, or clear previous results. Your
application must create and write any files you intend to download. Arguments
are passed literally. Standard SSH behavior applies to input and interruptions;
there is no custom job supervision, TTY allocation, or background job manager.
SSH uses exit 255 for transport errors, which can also be an application exit code.

`download` copies exactly the listed remote sources into their local destinations.
A missing source is an error. It can download arbitrary files, including logs
written by your application. Repeated downloads retain unrelated local files.

`clean` deletes exactly the listed remote paths and reports each as removed or already absent. Missing paths are harmless.
It does not infer cleanup from upload or download mappings or alter local files.
Use a dedicated remote directory such as `~/demo` when you want a single cleanup
entry for the whole application.

Operations are sequential and stop at the first error; earlier copies or removals
are not rolled back. There are no locks or upload-completion markers. Run these
commands sequentially and avoid uploading or cleaning an application while it runs.

## Previous interface

`set-*`, `show-config`, `update`, and `get-output` have been removed. Existing
`~/.config/remote-controller` files and `REMOTE_CONTROLLER_SSH_TARGET` are ignored.
Old manifests must be rewritten using the format above. Existing remote workspaces
are left intact; include their specific paths in `clean` if you want to remove them.

## Tests

```sh
python3 -m unittest discover -s common/tools/remote-controller/tests -v
```

Tests use temporary directories, real rsync, and a local SSH transport shim.
To use an isolated localhost SSH server instead (requires `sshd`, `ssh-keygen`,
and a host configured to permit an unprivileged test SSH server):

```sh
REMOTE_CONTROLLER_TEST_REAL_SSH=1 python3 -m unittest discover -s common/tools/remote-controller/tests -v
```

The tests use temporary keys and strict host-key checking. They do not connect
to your board or modify your normal SSH configuration.
