import contextlib
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

TOOL = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(TOOL))
import controller as c


class Fixture:
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(prefix="remote-controller-test-")
        self.addCleanup(self.temp.cleanup)
        self.base = Path(self.temp.name)
        self.build = self.base / "build with spaces"
        self.build.mkdir()
        self.app = self.build / "app"
        self.app.write_text("#!/bin/sh\nprintf 'hello\\n'\n")
        self.app.chmod(0o755)
        (self.build / "resources").mkdir()
        (self.build / "resources/data").write_text("resource")
        self.path = self.base / "deploy.json"
        self.manifest = {
            "ssh_target": "test-board",
            "upload": [
                {"source": "build with spaces/app", "destination": "~/demo/app"},
                {"source": "build with spaces/resources", "destination": "~/demo/resources"}],
            "run": {"executable": "~/demo/app", "cwd": "~/demo", "args": []},
            "download": [{"source": "~/demo/results", "destination": "collected"}],
            "clean": ["~/demo"]}
        self.write_manifest()

    def write_manifest(self):
        self.path.write_text(json.dumps(self.manifest))


class ControllerTests(Fixture, unittest.TestCase):
    def test_minimal_manifest_and_command_specific_validation(self):
        self.path.write_text(json.dumps({"ssh_target": "board", "run": {"executable": "~/app"}}))
        self.assertEqual(c.load_manifest(self.path, "run")["ssh_target"], "board")
        with self.assertRaises(c.ControllerError):
            c.load_manifest(self.path, "upload")

    def test_invalid_manifest(self):
        for value in ([], {"ssh_target": "-bad"}, {"ssh_target": "board; touch x"},
                      {**self.manifest, "type": "radxa-htp"},
                      {**self.manifest, "app_id": "old"},
                      {**self.manifest, "upload": [{"source": "x"}]},
                      {**self.manifest, "upload": [{"source": "x", "destination": "../x"}]}):
            with self.subTest(value=value):
                self.path.write_text(json.dumps(value))
                with self.assertRaises(c.ControllerError):
                    c.load_manifest(self.path, "upload")
        self.path.write_text('{broken')
        with self.assertRaises(c.ControllerError):
            c.load_manifest(self.path, "run")

    def test_cleanup_path_guards(self):
        for value in ("", "/", "////", "~", "~/", ".", "~/.", "../x", "~/demo/../.."):
            with self.subTest(value=value), self.assertRaises(c.ControllerError):
                c.clean_path(value)

    def test_removed_commands_and_missing_manifest(self):
        for args in (["set-type", "radxa-cpu"], ["show-config"], ["update"], ["get-output"], ["run"]):
            with contextlib.redirect_stderr(io.StringIO()), self.assertRaises(SystemExit) as error:
                c.main(args)
            self.assertEqual(error.exception.code, 2)


@unittest.skipUnless(shutil.which("rsync"), "rsync required")
class WorkflowTests(Fixture, unittest.TestCase):
    def setUp(self):
        super().setUp()
        self.remote_home = self.base / "remote home"
        self.remote_home.mkdir()
        self.bin = self.base / "bin"
        self.bin.mkdir()
        ssh = self.bin / "ssh"
        ssh.write_text('''#!/usr/bin/env python3
import os, sys
args = sys.argv[1:]
if args[0] == "-T": args.pop(0)
if args.pop(0) != "test-board": sys.exit(255)
os.environ["HOME"] = os.environ["TEST_REMOTE_HOME"]
os.execv("/bin/sh", ["sh", "-c", " ".join(args)])
''')
        ssh.chmod(0o755)
        self.env = {**os.environ, "PATH": str(self.bin) + os.pathsep + os.environ["PATH"],
                    "TEST_REMOTE_HOME": str(self.remote_home), "XDG_CONFIG_HOME": str(self.base / "config"),
                    "REMOTE_CONTROLLER_SSH_TARGET": "ignored-old-override"}
        if os.environ.get("REMOTE_CONTROLLER_TEST_REAL_SSH") == "1":
            from ssh_fixture import install
            install(self)
        self.root = self.remote_home / "demo"

    def cli(self, command, expected=0):
        result = subprocess.run([str(TOOL / "remote-controller"), command, str(self.path)],
                                cwd="/tmp", env=self.env, capture_output=True, timeout=15)
        self.assertEqual(result.returncode, expected, result.stderr.decode())
        return result

    def test_workflow(self):
        self.app.write_text('''#!/usr/bin/env python3
import pathlib, sys
print("hello " + sys.argv[1])
print("stderr", file=sys.stderr)
pathlib.Path("results").mkdir(exist_ok=True)
pathlib.Path("results/result").write_text(pathlib.Path("resources/data").read_text())
sys.exit(7)
''')
        self.manifest["run"]["args"] = ["literal ' ; $(touch SHOULD_NOT_EXIST)"]
        self.write_manifest()
        self.cli("upload")
        result = self.cli("run", expected=7)
        self.assertIn(self.manifest["run"]["args"][0].encode(), result.stdout)
        self.assertEqual(result.stderr, b"stderr\n")
        self.assertFalse((self.root / "SHOULD_NOT_EXIST").exists())
        self.cli("download")
        self.assertEqual((self.base / "collected/result").read_text(), "resource")
        self.assertFalse((self.base / "config").exists())
        self.assertFalse((self.root / "logs").exists())
        resource = self.build / "resources/data"
        timestamp = resource.stat().st_mtime_ns
        resource.write_text("modified")
        os.utime(resource, ns=(timestamp, timestamp))
        (self.root / "resources/extra").write_text("keep")
        (self.base / "collected/extra").write_text("keep")
        self.cli("upload")
        self.assertTrue((self.root / "resources/extra").exists())
        self.assertEqual((self.root / "resources/data").read_text(), "modified")
        self.app.unlink()
        self.cli("run", expected=7)
        self.cli("download")
        self.assertEqual((self.base / "collected/result").read_text(), "modified")
        self.assertTrue((self.base / "collected/extra").exists())
        sibling = self.remote_home / "other"
        sibling.mkdir()
        self.cli("clean")
        self.cli("clean")
        self.assertFalse(self.root.exists())
        self.assertTrue(sibling.exists())
        self.assertTrue(self.path.exists())

    def test_file_rename_and_literal_paths(self):
        destination = "~/space ' ; $(echo nope)/renamed"
        self.manifest["upload"] = [{"source": "build with spaces/app", "destination": destination}]
        self.manifest["download"] = [{"source": destination, "destination": "saved dir/new-name"}]
        self.manifest["clean"] = [destination]
        self.write_manifest()
        self.cli("upload")
        self.cli("download")
        self.assertEqual((self.base / "saved dir/new-name").read_bytes(), self.app.read_bytes())
        self.cli("clean")
        self.assertFalse((self.remote_home / destination[2:]).exists())

    def test_run_does_not_require_upload_or_clear_results(self):
        self.root.mkdir()
        app = self.root / "app"
        app.write_text("#!/bin/sh\nexit 255\n")
        app.chmod(0o755)
        (self.root / "keep").write_text("data")
        self.manifest = {"ssh_target": "test-board", "run": {"executable": str(app)}}
        self.write_manifest()
        self.cli("run", expected=255)
        self.assertEqual((self.root / "keep").read_text(), "data")

    def test_missing_source_and_transfer_failure(self):
        self.app.unlink()
        self.assertIn(b"Upload source", self.cli("upload", expected=1).stderr)
        self.assertFalse(self.root.exists())
        self.app.write_text("x")
        rsync = self.bin / "rsync"
        rsync.write_text("#!/bin/sh\nexit 23\n")
        rsync.chmod(0o755)
        self.assertIn(b"rsync failed", self.cli("upload", expected=1).stderr)

    def test_clean_explicit_paths_and_guards(self):
        self.cli("upload")
        self.manifest["clean"] = ["~/demo/app"]
        self.write_manifest()
        self.cli("clean")
        self.assertFalse((self.root / "app").exists())
        self.assertTrue((self.root / "resources/data").exists())
        self.manifest["clean"] = [str(self.remote_home)]
        self.write_manifest()
        self.assertIn(b"unsafe cleanup", self.cli("clean", expected=1).stderr)
        alias = self.remote_home / "alias"
        alias.symlink_to(self.root, target_is_directory=True)
        self.manifest["clean"] = ["~/alias/resources"]
        self.write_manifest()
        self.cli("clean", expected=1)
        self.assertTrue((self.root / "resources/data").exists())
        self.manifest["clean"] = ["~/alias"]
        self.write_manifest()
        self.cli("clean")
        self.assertFalse(alias.is_symlink())
        self.assertTrue(self.root.exists())


if __name__ == "__main__":
    unittest.main()
