# Accessibility, localization, and actionable feedback

An expressive instrument should not require perfect vision, precise mouse
movement, or familiarity with engineering jargon. Accessibility is part of
control correctness, not an optional visual theme. This chapter defines proposed
native-plugin requirements and tests. It does not claim formal conformance or
that every assistive technology is already supported.

## Accessible semantics

Every interactive control needs a meaningful role, name, value, unit, state,
and supported actions. A custom-painted knob without an accessibility value
interface can look finished while being unusable to a screen reader. Expose
the same authoritative state used by audio and host automation, not a separate
string assembled from stale UI values.

JUCE's [AccessibilityHandler reference](https://docs.juce.com/master/classjuce_1_1AccessibilityHandler.html)
describes its bridge for accessible components and platform clients. Inspect the
pinned version and test actual supported screen readers in standalone and host
contexts. Framework support is a starting point, not evidence that custom
controls have correct semantics.

Names should identify purpose and target: “String 2 tuning, cents relative to
setup” is more useful than “Slider 7.” Do not announce every live meter frame.
Use concise state changes and appropriately prioritized messages. A repeated
solver warning must not monopolize speech while a performer tries to reach
the panic control.

## Keyboard operation and focus

All essential tasks should have keyboard paths: setup selection, parameter
adjustment, precise entry, cancellation, navigation, import error recovery,
diagnostics, and panic. Define arrow, page, home/end, fine-adjustment, and reset
behavior consistently where applicable. Avoid single-letter global shortcuts
that conflict with typing or the host's transport unless they are scoped and
configurable.

Focus must be visible and ordered logically. Opening a dialog moves focus into
it; closing restores focus to a sensible origin. Do not trap keyboard users in
a custom graph or invisible control. A tab change should not leave focus on a
hidden widget. Test with the mouse untouched and the screen reader active where
supported.

Native plugin embedding complicates focus: hosts may intercept keys or provide
their own accessibility container. Test the declared host/format combinations.
A standalone pass does not establish keyboard behavior inside a DAW. Document
host limitations precisely and provide alternative interaction where feasible.

## Contrast, color, and scale

Use text, shape, and position as well as color to communicate states. Red/green
alone is insufficient for failure/success or active/pending distinctions. Ensure
focus, selection, and disabled states remain understandable in each theme.
Do not use very faint labels merely to make the interface appear minimal.

The W3C [WCAG 2.2 recommendation](https://www.w3.org/TR/WCAG22/) provides useful
testable contrast guidance: its minimum text criteria use 4.5:1 for ordinary
text and 3:1 for qualifying large text, with separate non-text contrast criteria.
These are useful design references for this native interface, not a claim that
applying those ratios alone certifies a plugin's accessibility. Read applicable
criteria and test the complete interaction.

Support declared UI scaling and verify that larger text does not hide controls.
Do not assume scaling a bitmap preserves readability. Test long labels, small
editor sizes, high-density displays, and movement between displays where
supported. Keep minimum useful control sizes in the design contract and verify
hit regions independently from drawing bounds.

## Motion and visual load

Spectra, energy traces, and meters can be informative but distracting. Allow
reduced animation or paused diagnostics without changing synthesis. A paused
plot retains its timestamp and clearly indicates that it is not live. Avoid
flashing status indicators for routine events. Use persistent concise messages
for important incidents rather than requiring the user to catch a brief animation.

Do not require color or motion perception to identify the active string or
selected gesture. Provide text and accessible state. Engineering displays should
offer a table or summary of essential values so a graph is not the only route
to information. A visual waveform is not a substitute for an accessible error
message or a clear parameter value.

## Persian text and bidirectional layout

The Iranian setar context can require Persian names and right-to-left text.
Treat localization as a semantic design task. Use practitioner-reviewed wording
and preserve Unicode correctly. Test mixed Persian text, Latin abbreviations,
numbers, signs, units, and file names. Bidirectional punctuation and numeric
placement can become confusing even when all glyphs render.

Do not translate stable parameter IDs or serialization keys. Display labels may
change by locale while host identity and stored meaning remain stable. Decide
whether numbers use locale-specific formatting and how numeric entry handles
decimal separators. Parse unambiguously and reject unsupported forms with a
clear message; never silently truncate a value because punctuation differs.

Test font coverage and fallback. Missing glyph boxes, broken joining, or clipped
diacritics are defects, not acceptable platform differences. Include localized
screenshots and semantic text assertions. Do not infer that a correct screenshot
proves a screen reader pronounces the terms intelligibly; actual assistive-
technology review is needed.

## Error and recovery language

State what failed, what remains active, and what the user can do next. “The new
body configuration could not be prepared; your previous setup is still active”
is actionable. “Invalid model” is not enough. Pair musician wording with a stable
diagnostic identifier that can be copied for support without exposing personal
paths or raw private data.

Distinguish warning, rejected action, protection incident, and unavailable
capability. A proposed feature should be absent or clearly unavailable rather
than represented by a control that silently does nothing. If a parameter is
clamped or a note is rejected under a declared capacity policy, provide the
appropriate indication without flooding performance with modal dialogs.

Never use a modal dialog from processing. Route messages through bounded
telemetry to a non-real-time presentation path. If the editor is closed, preserve
important incident state for later inspection. The engine must remain safe
without waiting for a user to acknowledge an alert.

## Audio interaction and safe audition

Do not start loud or unexpected audio merely because a preset is selected or a
window opens. If audition is supported, make the action explicit and use the
declared output/protection policy. A player should be able to stop it through
keyboard and visible controls. Preserve current output gain during setup changes
unless the user deliberately loads gain as part of the preset contract.

Distinguish measured amplitude, normalized output, and artistic loudness
compensation. A “safe” label cannot certify a listening level without knowing
device gain and playback equipment. Use clear output metering and avoid
unsupported assurances. Audio fault containment should be tested at the engine
and wrapper levels, not delegated to the UI.

## Evaluation and evidence

Create task scripts for keyboard-only use, screen-reader navigation, high-scale
layout, reduced animation, localized numeric entry, and error recovery. Include
participants or reviewers with relevant access needs when available, and record
the scope honestly. Automated checks cover names, roles, bounds, and contrast
calculations; they cannot establish the entire experience.

Capture the accessibility tree or equivalent semantic evidence where supported,
alongside authentic screenshots and interaction logs. Record OS, assistive
technology, host, format, build, locale, and scale. Mark unavailable combinations
unrun. Do not claim accessibility for every platform based on one native API
inspection.

## Acceptance and next action

Acceptance requires essential tasks without a mouse, meaningful accessible
semantics, tested contrast/scale, understandable errors, and localized behavior
within the declared support domain. Remaining gaps are user-facing limitations,
not hidden engineering notes. The next action is to test the smallest implemented
control set with keyboard and screen reader before extending custom widgets.
