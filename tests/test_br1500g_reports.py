"""Host-side regression tests using the BR1500G's captured USB payloads."""

from pathlib import Path
import subprocess
import sys

import yaml


def test_captured_reports_and_descriptor_bounds(tmp_path):
    root = Path(__file__).resolve().parents[1]
    binary = tmp_path / "br1500g_reports"
    subprocess.run(
        ["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
         "-I", str(root),
         str(root / "tests/br1500g_reports.cpp"), "-o", str(binary)],
        check=True,
    )
    subprocess.run([str(binary)], check=True)


def test_example_validation(tmp_path):
    root = Path(__file__).resolve().parents[1]

    class TestLoader(yaml.SafeLoader):
        pass

    TestLoader.add_constructor("!secret", lambda loader, node: "test-password")
    example = root / "configs/examples/br1500g-xiao.yaml"
    config = yaml.load(example.read_text(), Loader=TestLoader)
    config["external_components"] = [{
        "source": {"type": "local", "path": str(root / "components")},
        "components": ["ups_hid"],
    }]
    config["esphome"]["build_path"] = str(tmp_path / "build")
    local_config = tmp_path / "br1500g.yaml"
    local_config.write_text(yaml.safe_dump(config))
    result = subprocess.run(
        [sys.executable, "-m", "esphome", "config", str(local_config)],
        capture_output=True, text=True, timeout=60,
    )
    assert result.returncode == 0, result.stdout + result.stderr
