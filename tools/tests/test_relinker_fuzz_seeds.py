import contextlib
import io
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path
from unittest import mock

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import relinker_fuzz_seeds


class WithoutArgumentsTests(unittest.TestCase):
    def test_no_parameters(self):
        def sample():
            return 1

        self.assertTrue(relinker_fuzz_seeds.without_arguments(sample))

    def test_positional_without_default(self):
        def sample(a):
            return a

        self.assertFalse(relinker_fuzz_seeds.without_arguments(sample))

    def test_positional_only_without_default(self):
        def sample(a, /):
            return a

        self.assertFalse(relinker_fuzz_seeds.without_arguments(sample))

    def test_positional_with_default(self):
        def sample(a=1):
            return a

        self.assertTrue(relinker_fuzz_seeds.without_arguments(sample))

    def test_positional_only_with_default(self):
        def sample(a=1, /):
            return a

        self.assertTrue(relinker_fuzz_seeds.without_arguments(sample))

    def test_mixed_positional_parameters(self):
        def sample_required_first(a, b=2):
            return a + b

        def sample_defaults_only(a=1, b=2):
            return a + b

        self.assertFalse(relinker_fuzz_seeds.without_arguments(sample_required_first))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(sample_defaults_only))

    def test_varargs_and_varkwargs(self):
        def varargs_only(*args):
            return args

        def kwargs_only(**kwargs):
            return kwargs

        def varargs_and_kwargs(*args, **kwargs):
            return args, kwargs

        def positional_and_varargs(a, *args):
            return a, args

        def default_and_varargs(a=1, *args):
            return a, args

        self.assertTrue(relinker_fuzz_seeds.without_arguments(varargs_only))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(kwargs_only))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(varargs_and_kwargs))
        self.assertFalse(relinker_fuzz_seeds.without_arguments(positional_and_varargs))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(default_and_varargs))

    def test_keyword_only_parameters(self):
        def keyword_only(*, key=1):
            return key

        def required_keyword_only(*, key):
            return key

        self.assertTrue(relinker_fuzz_seeds.without_arguments(keyword_only))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(required_keyword_only))

    def test_lambdas(self):
        self.assertTrue(relinker_fuzz_seeds.without_arguments(lambda: None))
        self.assertFalse(relinker_fuzz_seeds.without_arguments(lambda x: x))
        self.assertTrue(relinker_fuzz_seeds.without_arguments(lambda x=1: x))


class ImagesDiscoveryTests(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.root = Path(self.temp_dir.name)

    def test_discovery_in_directory(self):
        module_code = "\n".join([
            "def fixture():",
            "    return b'A' * 64",
            "",
            "def image():",
            "    return bytearray(b'B' * 70)",
            "",
            "make_image = 'not_callable'",
            "",
            "def tls_image(required_arg):",
            "    return b'C' * 64",
            "",
            "def padded_image():",
            "    return b'too_short'",
            "",
            "def executable_image():",
            "    raise RuntimeError('failure')",
            "",
            "def empty_tls_image():",
            "    return 'not_bytes'",
            "",
            "def make_eof_segment_prefix_image():",
            "    return b'E' * 65",
            "",
            "def unlisted_function():",
            "    return b'Z' * 100",
        ])
        (self.root / "test_sample.py").write_text(module_code, encoding="utf-8")
        (self.root / "ignored_file.py").write_text(module_code, encoding="utf-8")
        (self.root / "test_broken.py").write_text("invalid syntax :::", encoding="utf-8")

        initial_path = list(sys.path)
        self.addCleanup(lambda: setattr(sys, "path", initial_path))
        self.addCleanup(lambda: sys.modules.pop("test_sample", None))
        self.addCleanup(lambda: sys.modules.pop("test_broken", None))

        with mock.patch.object(relinker_fuzz_seeds, "TESTS", self.root):
            discovered = list(relinker_fuzz_seeds.images())

        expected = [
            ("test_sample-fixture", b"A" * 64),
            ("test_sample-image", b"B" * 70),
            ("test_sample-make_eof_segment_prefix_image", b"E" * 65),
        ]
        self.assertEqual(discovered, expected)
        self.assertIn(str(self.root), sys.path)

    def test_boundary_length_63_and_64_bytes(self):
        path_63 = Path("test_sub64.py")
        path_64 = Path("test_at64.py")

        module_63 = mock.Mock(spec=["fixture"], fixture=lambda: b"X" * 63)
        module_64 = mock.Mock(spec=["fixture"], fixture=lambda: b"Y" * 64)

        def mock_import(name):
            if name == "test_sub64":
                return module_63
            if name == "test_at64":
                return module_64
            raise ImportError(name)

        mock_tests = mock.Mock()
        mock_tests.glob.return_value = [path_64, path_63]

        with mock.patch.object(relinker_fuzz_seeds, "TESTS", mock_tests), \
                mock.patch.object(relinker_fuzz_seeds.importlib, "import_module", side_effect=mock_import):
            discovered = list(relinker_fuzz_seeds.images())

        self.assertEqual(discovered, [("test_at64-fixture", b"Y" * 64)])

    def test_import_exception_handling(self):
        path = Path("test_raising.py")

        mock_tests = mock.Mock()
        mock_tests.glob.return_value = [path]

        with mock.patch.object(relinker_fuzz_seeds, "TESTS", mock_tests), \
                mock.patch.object(relinker_fuzz_seeds.importlib, "import_module", side_effect=ImportError("missing")):
            discovered = list(relinker_fuzz_seeds.images())

        self.assertEqual(discovered, [])


class RelinkerFuzzSeedsCliTests(unittest.TestCase):
    def setUp(self):
        self.temp_dir = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp_dir.cleanup)
        self.root = Path(self.temp_dir.name)

    def test_main_creates_seeds_and_directory(self):
        output_dir = self.root / "nested" / "sub" / "output_seeds"
        fake_images = [
            ("alpha-fixture", b"A" * 64),
            ("beta-image", b"B" * 64),
        ]

        stdout_buffer = io.StringIO()
        with mock.patch.object(sys, "argv", ["relinker_fuzz_seeds.py", str(output_dir)]), \
                mock.patch.object(relinker_fuzz_seeds, "images", return_value=fake_images), \
                contextlib.redirect_stdout(stdout_buffer):
            relinker_fuzz_seeds.main()

        self.assertTrue(output_dir.is_dir())
        expected_count = len(fake_images) * len(relinker_fuzz_seeds.SELECTORS)
        written_files = list(output_dir.iterdir())
        self.assertEqual(len(written_files), expected_count)

        for name, data in fake_images:
            for selector in relinker_fuzz_seeds.SELECTORS:
                seed_file = output_dir / f"{name}-{selector}"
                self.assertTrue(seed_file.exists())
                content = seed_file.read_bytes()
                self.assertEqual(content[0], selector)
                self.assertEqual(content[1:], data)
                self.assertEqual(len(content), len(data) + 1)

        self.assertIn(f"{expected_count} seeds written to {output_dir}", stdout_buffer.getvalue())

    def test_main_empty_images(self):
        output_dir = self.root / "empty_output"
        stdout_buffer = io.StringIO()

        with mock.patch.object(sys, "argv", ["relinker_fuzz_seeds.py", str(output_dir)]), \
                mock.patch.object(relinker_fuzz_seeds, "images", return_value=[]), \
                contextlib.redirect_stdout(stdout_buffer):
            relinker_fuzz_seeds.main()

        self.assertTrue(output_dir.is_dir())
        self.assertEqual(list(output_dir.iterdir()), [])
        self.assertIn(f"0 seeds written to {output_dir}", stdout_buffer.getvalue())

    def test_constants(self):
        self.assertEqual(relinker_fuzz_seeds.SELECTORS, (0, 1, 2, 4))
        self.assertIn("fixture", relinker_fuzz_seeds.BUILDERS)
        self.assertIn("image", relinker_fuzz_seeds.BUILDERS)

    def test_subprocess_cli(self):
        output_dir = self.root / "subproc_output"
        result = subprocess.run(
            [sys.executable, str(Path(relinker_fuzz_seeds.__file__).resolve()), str(output_dir)],
            capture_output=True, text=True, check=True
        )
        self.assertEqual(result.returncode, 0)
        self.assertTrue(output_dir.is_dir())
        self.assertIn("seeds written to", result.stdout)


if __name__ == "__main__":
    unittest.main()
