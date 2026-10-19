"""Optional unprivileged localhost sshd fixture, with temporary keys and HOME."""
import getpass
from pathlib import Path
import shlex
import shutil
import socket
import subprocess
import time


def install(test):
    sshd = shutil.which("sshd")
    ssh = shutil.which("ssh")
    keygen = shutil.which("ssh-keygen")
    if not all((sshd, ssh, keygen)):
        raise RuntimeError("Real SSH tests require sshd, ssh, and ssh-keygen")
    directory = test.base / "ssh-fixture"
    directory.mkdir()
    for name in ("host", "client"):
        subprocess.run([keygen, "-q", "-t", "ed25519", "-N", "", "-f", str(directory / name)], check=True)
    with socket.socket() as probe:
        probe.bind(("127.0.0.1", 0))
        port = probe.getsockname()[1]
    # Avoid spaces in the SetEnv assignment; the ordinary shim tests cover them.
    test.remote_home = test.base / "ssh-home"
    test.remote_home.mkdir()
    server_config = directory / "sshd_config"
    server_config.write_text(f'''Port {port}
ListenAddress 127.0.0.1
HostKey {directory}/host
PidFile {directory}/pid
AuthorizedKeysFile {directory}/client.pub
StrictModes no
PasswordAuthentication no
KbdInteractiveAuthentication no
UsePAM no
SetEnv HOME={test.remote_home}
LogLevel ERROR
''')
    log = (directory / "server.log").open("w+")
    test.addCleanup(log.close)
    server = subprocess.Popen([sshd, "-D", "-e", "-f", str(server_config)], stdout=log, stderr=log)
    def stop():
        server.terminate()
        try:
            server.wait(timeout=5)
        except subprocess.TimeoutExpired:
            server.kill()
            server.wait()
    test.addCleanup(stop)
    for _ in range(100):
        if server.poll() is not None:
            log.seek(0)
            raise RuntimeError("Test sshd failed: " + log.read())
        try:
            with socket.create_connection(("127.0.0.1", port), timeout=0.1):
                break
        except OSError:
            time.sleep(0.02)
    else:
        raise RuntimeError("Test sshd did not start")
    known_hosts = directory / "known_hosts"
    known_hosts.write_text(f"[127.0.0.1]:{port} " + (directory / "host.pub").read_text())
    client_config = directory / "config"
    client_config.write_text(f'''Host test-board
    HostName 127.0.0.1
    Port {port}
    User {getpass.getuser()}
    IdentityFile {directory}/client
    IdentitiesOnly yes
    UserKnownHostsFile {known_hosts}
    StrictHostKeyChecking yes
    BatchMode yes
    ConnectTimeout 5
''')
    (test.bin / "ssh").write_text("#!/bin/sh\nexec " + shlex.join([ssh, "-F", str(client_config)]) + ' "$@"\n')
