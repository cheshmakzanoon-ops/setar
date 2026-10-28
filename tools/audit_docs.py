#!/usr/bin/env python3
"""Offline links, phase dependencies and prose counts; NOT scientific validation."""
from __future__ import annotations
import argparse
from collections import Counter
import json
from pathlib import Path
import re
import subprocess
import sys
from urllib.parse import unquote, urlsplit


def strip_fences(text: str) -> str:
    output, marker, width = [], None, 0
    for line in text.splitlines():
        match = re.match(r"^\s{0,3}(`{3,}|~{3,})", line)
        if match:
            token = match.group(1)
            if marker is None:
                marker, width = token[0], len(token)
                continue
            if token[0] == marker and len(token) >= width:
                marker = None
                continue
        if marker is None:
            output.append(line)
    return "\n".join(output)


def prose_words(text: str) -> int:
    text = re.sub(r"<!--.*?-->", "", strip_fences(text), flags=re.S)
    text = re.sub(r"!?\[([^\]]*)\]\([^\n]*?\)", r"\1", text)
    text = re.sub(r"https?://\S+", "", text)
    return len(re.findall(r"[^\W_]+(?:['’\-][^\W_]+)*", text, flags=re.UNICODE))


def repository_markdown(root: Path) -> list[Path]:
    try:
        result = subprocess.run(["git", "ls-files", "--cached", "--others", "--exclude-standard", "-z"],
                                cwd=root, check=True, capture_output=True)
        names = set(result.stdout.decode("utf-8").split("\0"))
        return sorted(root / name for name in names if name.lower().endswith(".md") and (root / name).is_file())
    except (OSError, subprocess.CalledProcessError, UnicodeError):
        excluded = {".git", "build", "artifacts", "node_modules", ".venv", "vendor"}
        return sorted(path for path in root.rglob("*.md") if not excluded.intersection(path.relative_to(root).parts))


def local_links(text: str) -> list[str]:
    text = re.sub(r"<!--.*?-->", "", strip_fences(text), flags=re.S)
    values = re.findall(r"!?\[[^\]]*\]\(<?([^\s)>]+)>?(?:\s+\"[^\"]*\")?\)", text)
    return values + re.findall(r"^\s*\[[^\]]+\]:\s*<?([^\s>]+)>?", text, flags=re.M)


def phase_errors(root: Path) -> list[str]:
    manifest = root / "roadmap/phase-register.json"
    if not manifest.exists():
        return []
    try:
        phases = json.loads(manifest.read_text(encoding="utf-8"))["phases"]
        errors, by_id = [], {}
        for phase in phases:
            identifier = phase["id"]
            if not re.fullmatch(r"P\d{2}", identifier):
                errors.append(f"invalid phase id: {identifier}")
            if identifier in by_id:
                errors.append(f"duplicate phase id: {identifier}")
            by_id[identifier] = phase
            if not (root / phase["document"]).is_file():
                errors.append(f"{identifier}: missing document {phase['document']}")
            if any(key in phase for key in ("deadline", "due_date", "duration", "estimate")):
                errors.append(f"{identifier}: use evidence gates, not calendar targets")
        visiting, complete = set(), set()
        def visit(identifier: str) -> None:
            if identifier in visiting:
                raise ValueError(f"phase dependency cycle through {identifier}")
            if identifier in complete:
                return
            if identifier not in by_id:
                raise ValueError(f"missing phase dependency {identifier}")
            visiting.add(identifier)
            for parent in by_id[identifier].get("requires", []):
                visit(parent)
            visiting.remove(identifier)
            complete.add(identifier)
        for identifier in by_id:
            visit(identifier)
        return errors
    except (OSError, ValueError, TypeError, KeyError) as exc:
        return [f"phase register: {exc}"]


def audit(root: Path) -> dict:
    root = root.resolve()
    errors, counts, paragraphs = [], {}, Counter()
    for path in repository_markdown(root):
        relative = path.relative_to(root).as_posix()
        try:
            text = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as exc:
            errors.append(f"{relative}: cannot read UTF-8: {exc}")
            continue
        counts[relative] = prose_words(text)
        for link in local_links(text):
            parsed = urlsplit(link)
            if parsed.scheme or parsed.netloc or not parsed.path:
                continue
            target = (path.parent / unquote(parsed.path)).resolve()
            if not target.is_relative_to(root):
                errors.append(f"{relative}: local link escapes repository: {link}")
            elif not target.exists():
                errors.append(f"{relative}: missing local link: {link}")
        for paragraph in re.split(r"\n\s*\n", strip_fences(text)):
            normalized = " ".join(paragraph.split())
            if len(normalized) >= 300 and not normalized.startswith(("|", "#")):
                paragraphs[normalized] += 1
    errors.extend(phase_errors(root))
    duplicates = [{"count": count, "excerpt": text[:150]}
                  for text, count in paragraphs.most_common() if count > 1][:20]
    return {"schema_version": 1,
            "scope": "Markdown prose; fenced source excluded; not scientific correctness certification",
            "markdown_files": len(counts), "prose_words": sum(counts.values()),
            "requested_depth_target_words": 200000, "word_target_is_not_acceptance_gate": True,
            "files": counts, "errors": errors, "repeated_long_paragraphs_for_review": duplicates}


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=Path(__file__).resolve().parents[1])
    parser.add_argument("--json", type=Path, dest="json_path")
    options = parser.parse_args()
    report = audit(options.root)
    if options.json_path:
        options.json_path.parent.mkdir(parents=True, exist_ok=True)
        options.json_path.write_text(json.dumps(report, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print(f"Markdown files: {report['markdown_files']}")
    print(f"Prose words (fenced code excluded): {report['prose_words']:,}")
    print("Word count is coverage information, not a scientific acceptance test.")
    for error in report["errors"]:
        print(f"ERROR: {error}", file=sys.stderr)
    print(f"Repeated long paragraphs to review: {len(report['repeated_long_paragraphs_for_review'])}")
    print(f"Local-link/phase errors: {len(report['errors'])}")
    return 1 if report["errors"] else 0


if __name__ == "__main__":
    raise SystemExit(main())
