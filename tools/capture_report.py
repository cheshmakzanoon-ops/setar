#!/usr/bin/env python3
"""Capture a *real* offline report or SVG wireframe in a local Chromium browser.

Requires separately installed Playwright and Chromium. This is optional visual QA,
not part of the dependency-free headless numerical core. The capture is not
evidence of a JUCE editor or of measured-setar fidelity.
"""

from __future__ import annotations

import argparse
import hashlib
import json
import pathlib
import struct
import sys


def digest(path: pathlib.Path) -> str:
    hasher = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            hasher.update(chunk)
    return hasher.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", required=True, type=pathlib.Path,
                        help="Existing local, fully generated HTML report or SVG wireframe")
    parser.add_argument("--output", required=True, type=pathlib.Path,
                        help="New PNG screenshot path; an existing file is never overwritten")
    parser.add_argument("--browser-executable", type=pathlib.Path,
                        help="Optional path to an explicitly installed Chromium binary")
    parser.add_argument("--viewport-width", type=int, default=1280)
    parser.add_argument("--viewport-height", type=int, default=900)
    args = parser.parse_args()

    source, output = args.input.resolve(), args.output.resolve()
    manifest_path = output.with_suffix(".json")
    if not source.is_file() or source.suffix.lower() not in {".html", ".svg"}:
        parser.error("input must be an existing local .html or .svg file")
    if output.suffix.lower() != ".png" or output.exists() or manifest_path.exists():
        parser.error("output must be a new .png path with no existing screenshot or manifest")
    if source == output or not 640 <= args.viewport_width <= 3840 or not 400 <= args.viewport_height <= 2160:
        parser.error("invalid path or viewport dimensions")
    if args.browser_executable is not None and not args.browser_executable.is_file():
        parser.error("browser executable does not exist")

    try:
        from playwright.sync_api import sync_playwright
    except ImportError as error:
        parser.error("optional Playwright is unavailable; install it separately: " + str(error))

    output.parent.mkdir(parents=True, exist_ok=True)
    with sync_playwright() as playwright:
        launch = {"headless": True}
        if args.browser_executable is not None:
            launch["executable_path"] = str(args.browser_executable.resolve())
        browser = playwright.chromium.launch(**launch)
        try:
            context = browser.new_context(
                viewport={"width": args.viewport_width, "height": args.viewport_height},
                device_scale_factor=1, java_script_enabled=False, locale="en-US")

            # The report and wireframe need only file:// and embedded data assets.
            # A visual QA script should not make external HTTP requests.
            def restrict(route):
                if route.request.url.startswith(("file:", "data:")):
                    route.continue_()
                else:
                    route.abort()

            context.route("**/*", restrict)
            page = context.new_page()
            page.goto(source.as_uri(), wait_until="load")
            if source.suffix.lower() == ".html":
                if page.locator("img").count() < 3:
                    raise ValueError("report does not contain the expected computed graphs")
                if "Not a validated setar" not in page.locator("body").inner_text():
                    raise ValueError("missing synthetic/validation warning")
            else:
                if page.locator("svg").count() != 1:
                    raise ValueError("wireframe SVG was not displayed")
            page.screenshot(path=str(output), full_page=True, animations="disabled")
            browser_version = browser.version
            context.close()
        finally:
            browser.close()

    with output.open("rb") as stream:
        header = stream.read(24)
    if header[:8] != b"\x89PNG\r\n\x1a\n" or header[12:16] != b"IHDR":
        raise ValueError("capture did not produce a valid PNG header")
    width, height = struct.unpack(">II", header[16:24])
    manifest = {
        "schema_version": 1,
        "evidence_kind": "actual_chromium_capture_of_local_html_report" if source.suffix == ".html"
                         else "actual_chromium_capture_of_proposed_svg_wireframe",
        "not_a_setar_validation_or_juce_editor_screenshot": True,
        "source_filename": source.name,
        "source_sha256": digest(source),
        "output_filename": output.name,
        "output_sha256": digest(output),
        "browser": "Chromium",
        "browser_version": browser_version,
        "viewport_width_px": args.viewport_width,
        "viewport_height_px": args.viewport_height,
        "device_scale_factor": 1,
        "image_width_px": width,
        "image_height_px": height,
        "javascript_disabled": True,
        "external_requests_blocked": True,
        "limitations": "Offline report/wireframe capture; not host-plugin UI, acoustic or musician validation"
    }
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8")
    print(f"Captured {output.name}: {width} × {height} pixels; {manifest_path.name} has SHA-256 provenance")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as error:
        print(f"Capture failed: {error}", file=sys.stderr)
        raise SystemExit(2)
