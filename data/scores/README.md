# Demonstration scores for the playable prototype

**Status: synthetic demonstrations.** These files exercise the score grammar in
[docs/performance/07-score-format.md](../../docs/performance/07-score-format.md).
They are not transcriptions of a performance, not repertoire, and not validated
setar gestures; stroke and ornament names are engineering placeholders pending
practitioner review (REQ-PRF-002).

| File | Content | Length |
| --- | --- | --- |
| `01-open-strings.score` | each physical string open, then course and four-string strums | 8 s |
| `02-fret-scale.score` | ascending/descending on string 0 through a subset of the default 17-step Pythagorean fret map, beats at 200 bpm | 6.6 s |
| `03-phrase.score` | drone strokes on course {2,3}, fretted notes, tremolo, slide, damping, a light-touch harmonic | 8 s |
| `04-dynamics.score` | open string 0 at pp, p, mp, mf, f, ff | 8 s |

The `integration.setar_play` CTest renders every `*.score` file in this directory
with the default synthetic preset. Scores reference frets up to 17, so they need a
fret map with at least 17 frets.
