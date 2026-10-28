import contextlib
import io
import json
import os
import stat
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import prepare_release_notes


class ReleaseBodyTests(unittest.TestCase):
    def test_notes_under_limit_are_unchanged(self):
        cases = (
            "",
            "Short release notes without changes section",
            "## What's Changed\n* First change\n* Second change\n",
            "## What's Changed\r\n* First change\r\n* Second change\r\n",
            "Unicode accents: \u00e9\u00e0\u00e7\u00fc and emoji: \U0001f389\U0001f680",
            "x" * (prepare_release_notes.BODY_LIMIT - 1),
            "x" * prepare_release_notes.BODY_LIMIT,
            "## What's Changed\n* Fix\n" + "y" * (prepare_release_notes.BODY_LIMIT - 30),
        )
        for notes in cases:
            with self.subTest(length=len(notes)):
                self.assertEqual(prepare_release_notes.release_body(notes), notes)

    def test_notes_over_limit_removes_whats_changed_until_next_h2(self):
        for newline in ("\n", "\r\n"):
            with self.subTest(newline=repr(newline)):
                prefix = f"# Release Title{newline}{newline}Introductory text{newline}{newline}"
                tail = f"## New Contributors{newline}* @contributor made their first contribution{newline}"
                padding = "x" * (prepare_release_notes.BODY_LIMIT - len(prefix) - len(tail) + 100)
                changes = f"## What's Changed{newline}* Change 1{newline}* Change 2{newline}"
                notes = prefix + changes + padding + newline + tail
                result = prepare_release_notes.release_body(notes)
                self.assertEqual(result, prefix + tail)

    def test_notes_over_limit_removes_whats_changed_until_next_h1(self):
        prefix = "Release introduction\n\n"
        tail = "# Important Notice\n* High severity update\n"
        padding = "y" * prepare_release_notes.BODY_LIMIT
        notes = prefix + "## What's Changed\n* Many changes\n" + padding + "\n" + tail
        result = prepare_release_notes.release_body(notes)
        self.assertEqual(result, prefix + tail)

    def test_notes_over_limit_removes_whats_changed_until_full_changelog(self):
        for newline in ("\n", "\r\n"):
            with self.subTest(newline=repr(newline)):
                prefix = f"Summary{newline}"
                tail = f"**Full Changelog**: https://github.com/example/compare/v1...v2{newline}"
                padding = "z" * prepare_release_notes.BODY_LIMIT
                notes = prefix + f"## What's Changed{newline}* Commits{newline}" + padding + newline + tail
                result = prepare_release_notes.release_body(notes)
                self.assertEqual(result, prefix + tail)

    def test_notes_over_limit_removes_whats_changed_until_end_of_string(self):
        prefix = "Release Overview\n\n"
        padding = "a" * prepare_release_notes.BODY_LIMIT
        notes = prefix + "## What's Changed\n* Commit 1\n* Commit 2\n" + padding
        result = prepare_release_notes.release_body(notes)
        self.assertEqual(result, prefix)

    def test_notes_over_limit_with_trailing_spaces_on_header(self):
        prefix = "Heading\n"
        tail = "## Next Section\nContent\n"
        padding = "b" * prepare_release_notes.BODY_LIMIT
        notes = prefix + "## What's Changed  \t  \n* Line\n" + padding + "\n" + tail
        result = prepare_release_notes.release_body(notes)
        self.assertEqual(result, prefix + tail)

    def test_notes_over_limit_without_whats_changed_remains_unchanged(self):
        notes = "# Large Release\n\n## Details\n\n" + "w" * (prepare_release_notes.BODY_LIMIT + 50)
        self.assertEqual(prepare_release_notes.release_body(notes), notes)


class PrepareNotesTests(unittest.TestCase):
    def test_rejects_identical_and_aliased_paths(self):
        pairs = ((0, 1), (0, 2), (1, 2))
        for first_idx, second_idx in pairs:
            with tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                paths = [root / "source.json", root / "body.md", root / "full.md"]
                paths[0].write_text(json.dumps({"body": "valid notes"}), encoding="utf-8")
                paths[1].write_bytes(b"initial body")
                paths[2].write_bytes(b"initial full")
                paths[second_idx] = paths[first_idx]
                with self.assertRaisesRegex(ValueError, "Release note input and outputs must be distinct files"):
                    prepare_release_notes.prepare_notes(*paths)
                self.assertEqual((root / "body.md").read_bytes(), b"initial body")
                self.assertEqual((root / "full.md").read_bytes(), b"initial full")

    def test_rejects_equivalent_resolved_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            sub = root / "sub"
            sub.mkdir()
            source = root / "source.json"
            body = root / "body.md"
            full = sub / ".." / "body.md"
            source.write_text(json.dumps({"body": "valid notes"}), encoding="utf-8")
            with self.assertRaisesRegex(ValueError, "Release note input and outputs must be distinct files"):
                prepare_release_notes.prepare_notes(source, body, full)

    def test_rejects_hardlinked_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "valid notes"}), encoding="utf-8")
            body.write_bytes(b"existing")
            try:
                os.link(body, full)
            except OSError as error:
                self.skipTest(str(error))
            with self.assertRaisesRegex(ValueError, "Release note input and outputs must be distinct files"):
                prepare_release_notes.prepare_notes(source, body, full)

    def test_rejects_symlinked_paths(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "valid notes"}), encoding="utf-8")
            body.write_bytes(b"existing")
            try:
                full.symlink_to(body)
            except OSError as error:
                self.skipTest(str(error))
            with self.assertRaisesRegex(ValueError, "Release note input and outputs must be distinct files"):
                prepare_release_notes.prepare_notes(source, body, full)

    def test_payload_must_be_json_object_with_body_field(self):
        invalid_shapes = (
            "[]",
            "[1, 2, 3]",
            '"simple string"',
            "12345",
            "true",
            "null",
            "{}",
            '{"not_body": "value"}',
        )
        for content in invalid_shapes:
            with self.subTest(content=content), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / "source.json"
                body = root / "body.md"
                full = root / "full.md"
                source.write_text(content, encoding="utf-8")
                body.write_bytes(b"old body")
                full.write_bytes(b"old full")
                with self.assertRaisesRegex(ValueError, "Generated release notes response must be a JSON object with a body field"):
                    prepare_release_notes.prepare_notes(source, body, full)
                self.assertEqual(body.read_bytes(), b"old body")
                self.assertEqual(full.read_bytes(), b"old full")

    def test_invalid_json_syntax_raises_json_decode_error(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text("{not valid json", encoding="utf-8")
            with self.assertRaises(json.JSONDecodeError):
                prepare_release_notes.prepare_notes(source, body, full)

    def test_deeply_nested_json_raises_value_error(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "notes"}), encoding="utf-8")
            with patch.object(prepare_release_notes.json, "loads", side_effect=RecursionError("deeply nested")):
                with self.assertRaisesRegex(ValueError, "Generated release notes response is too deeply nested"):
                    prepare_release_notes.prepare_notes(source, body, full)

    def test_payload_body_must_be_string(self):
        invalid_bodies = (
            123,
            True,
            None,
            ["list", "of", "strings"],
            {"nested": "object"},
        )
        for invalid_body in invalid_bodies:
            with self.subTest(invalid_body=type(invalid_body)), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / "source.json"
                body = root / "body.md"
                full = root / "full.md"
                source.write_text(json.dumps({"body": invalid_body}), encoding="utf-8")
                with self.assertRaisesRegex(ValueError, "Generated release notes body must be a string"):
                    prepare_release_notes.prepare_notes(source, body, full)

    def test_accepts_utf8_with_bom(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            notes = "# UTF-8 BOM test\n\nContent \u00e9\u00e8\n"
            source.write_text("\ufeff" + json.dumps({"body": notes}), encoding="utf-8")
            prepare_release_notes.prepare_notes(source, body, full)
            self.assertEqual(full.read_text(encoding="utf-8"), notes)
            self.assertEqual(body.read_text(encoding="utf-8"), notes)

    def test_body_over_limit_after_stripping_raises_value_error(self):
        excessive = "## Remaining\n" + "x" * (prepare_release_notes.BODY_LIMIT + 1)
        cases = (
            excessive,
            "## What's Changed\n* Change\n\n" + excessive,
        )
        for notes in cases:
            with self.subTest(has_whats_changed="What's Changed" in notes), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / "source.json"
                body = root / "out/body.md"
                full = root / "out/full.md"
                source.write_text(json.dumps({"body": notes}), encoding="utf-8")
                with self.assertRaisesRegex(ValueError, f"Release notes body exceeds {prepare_release_notes.BODY_LIMIT} characters after removing What's Changed"):
                    prepare_release_notes.prepare_notes(source, body, full)
                self.assertFalse(body.exists())
                self.assertFalse(full.exists())

    def test_body_over_limit_trimmed_successfully_under_limit(self):
        prefix = "Release v1.2.3\n\n"
        tail = "## Highlights\nImportant notes\n"
        padding = "c" * (prepare_release_notes.BODY_LIMIT + 500)
        notes = prefix + "## What's Changed\n" + padding + "\n" + tail
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "source.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": notes}), encoding="utf-8")
            prepare_release_notes.prepare_notes(source, body, full)
            self.assertEqual(full.read_text(encoding="utf-8"), notes)
            self.assertEqual(body.read_text(encoding="utf-8"), prefix + tail)

    def test_destination_is_directory_raises_is_a_directory_error(self):
        for target in ("body", "full"):
            with self.subTest(target=target), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / "source.json"
                body = root / "body"
                full = root / "full"
                source.write_text(json.dumps({"body": "notes"}), encoding="utf-8")
                if target == "body":
                    body.mkdir()
                    full = root / "full.md"
                    full.write_bytes(b"previous full")
                else:
                    body = root / "body.md"
                    body.write_bytes(b"previous body")
                    full.mkdir()
                with self.assertRaises(IsADirectoryError):
                    prepare_release_notes.prepare_notes(source, body, full)
                if target == "body":
                    self.assertEqual(full.read_bytes(), b"previous full")
                else:
                    self.assertEqual(body.read_bytes(), b"previous body")

    @unittest.skipUnless(os.name == "nt", "Windows read-only destination test")
    def test_destination_readonly_on_windows_raises_permission_error(self):
        for target in ("body", "full"):
            with self.subTest(target=target), tempfile.TemporaryDirectory() as directory:
                root = Path(directory)
                source = root / "source.json"
                body = root / "body.md"
                full = root / "full.md"
                source.write_text(json.dumps({"body": "updated notes"}), encoding="utf-8")
                body.write_bytes(b"saved body")
                full.write_bytes(b"saved full")
                readonly_file = body if target == "body" else full
                readonly_file.chmod(stat.S_IREAD)
                try:
                    with self.assertRaises(PermissionError) as context:
                        prepare_release_notes.prepare_notes(source, body, full)
                    self.assertIn("Release note destination is read-only", str(context.exception))
                    self.assertEqual(body.read_bytes(), b"saved body")
                    self.assertEqual(full.read_bytes(), b"saved full")
                finally:
                    readonly_file.chmod(stat.S_IREAD | stat.S_IWRITE)

    def test_nested_parent_directories_are_created_and_files_written(self):
        notes = "# Full Release Notes\n\nSpecial characters: \u00e9\u00f1\u4e16\u754c\n"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "in" / "source.json"
            source.parent.mkdir()
            source.write_text(json.dumps({"body": notes}), encoding="utf-8")
            body = root / "nested" / "dir" / "body.md"
            full = root / "other" / "nested" / "full.md"
            prepare_release_notes.prepare_notes(source, body, full)
            self.assertEqual(full.read_bytes(), notes.encode("utf-8"))
            self.assertEqual(body.read_bytes(), notes.encode("utf-8"))


class MainCliTests(unittest.TestCase):
    def test_main_with_explicit_argv(self):
        notes = "# Release Notes\n\n* Fix bug\n"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": notes}), encoding="utf-8")
            status = prepare_release_notes.main(["--input", str(source), "--body", str(body), "--full", str(full)])
            self.assertEqual(status, 0)
            self.assertEqual(body.read_text(encoding="utf-8"), notes)
            self.assertEqual(full.read_text(encoding="utf-8"), notes)

    def test_main_with_sys_argv(self):
        notes = "# Release Notes\n\n* New feature\n"
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": notes}), encoding="utf-8")
            argv = ["prepare_release_notes.py", "--input", str(source), "--body", str(body), "--full", str(full)]
            with patch.object(sys, "argv", argv):
                status = prepare_release_notes.main()
            self.assertEqual(status, 0)
            self.assertEqual(body.read_text(encoding="utf-8"), notes)
            self.assertEqual(full.read_text(encoding="utf-8"), notes)

    def test_main_missing_required_arguments(self):
        incomplete_argvs = (
            [],
            ["--input", "in.json"],
            ["--body", "b.md"],
            ["--full", "f.md"],
            ["--input", "in.json", "--body", "b.md"],
            ["--input", "in.json", "--full", "f.md"],
            ["--body", "b.md", "--full", "f.md"],
        )
        for argv in incomplete_argvs:
            with self.subTest(argv=argv), contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit) as error:
                    prepare_release_notes.main(argv)
                self.assertEqual(error.exception.code, 2)

    def test_main_positional_arguments_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "notes"}), encoding="utf-8")
            with contextlib.redirect_stderr(io.StringIO()):
                with self.assertRaises(SystemExit) as error:
                    prepare_release_notes.main([str(source), str(body), str(full)])
                self.assertEqual(error.exception.code, 2)

    def test_main_reports_value_error_without_traceback(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": 12345}), encoding="utf-8")
            stderr = io.StringIO()
            with contextlib.redirect_stderr(stderr), self.assertRaises(SystemExit) as error:
                prepare_release_notes.main(["--input", str(source), "--body", str(body), "--full", str(full)])
            self.assertEqual(error.exception.code, 2)
            self.assertIn("must be a string", stderr.getvalue())
            self.assertNotIn("Traceback", stderr.getvalue())

    def test_main_reports_os_error_without_traceback(self):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input_dir"
            source.mkdir()
            body = root / "body.md"
            full = root / "full.md"
            stderr = io.StringIO()
            with contextlib.redirect_stderr(stderr), self.assertRaises(SystemExit) as error:
                prepare_release_notes.main(["--input", str(source), "--body", str(body), "--full", str(full)])
            self.assertEqual(error.exception.code, 2)
            self.assertIn(source.name, stderr.getvalue())
            self.assertNotIn("Traceback", stderr.getvalue())

    def test_main_reconfigures_streams_when_available(self):
        class MockStream:
            def __init__(self):
                self.encoding = None
                self.errors = None

            def reconfigure(self, encoding=None, errors=None):
                self.encoding = encoding
                self.errors = errors

        out_stream = MockStream()
        err_stream = MockStream()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "stream test"}), encoding="utf-8")
            with patch.object(sys, "stdout", out_stream), patch.object(sys, "stderr", err_stream):
                status = prepare_release_notes.main(["--input", str(source), "--body", str(body), "--full", str(full)])
                self.assertEqual(status, 0)
        self.assertEqual(out_stream.encoding, "utf-8")
        self.assertEqual(out_stream.errors, "backslashreplace")
        self.assertEqual(err_stream.encoding, "utf-8")
        self.assertEqual(err_stream.errors, "backslashreplace")

    def test_main_succeeds_when_streams_lack_reconfigure(self):
        class PlainStream:
            def write(self, s):
                pass

            def flush(self):
                pass

        out_stream = PlainStream()
        err_stream = PlainStream()
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "input.json"
            body = root / "body.md"
            full = root / "full.md"
            source.write_text(json.dumps({"body": "plain streams"}), encoding="utf-8")
            with patch.object(sys, "stdout", out_stream), patch.object(sys, "stderr", err_stream):
                status = prepare_release_notes.main(["--input", str(source), "--body", str(body), "--full", str(full)])
                self.assertEqual(status, 0)


if __name__ == "__main__":
    unittest.main()

