# Visual tests, authentic screenshots, and numeric evidence

Visual evidence should show actual behavior, not suggest that a proposed
interface has been implemented. This chapter defines a proposed deterministic
capture and review workflow. Design wireframes are useful and must be labeled
as such. Screenshots used as implementation evidence must come from the running
candidate or its declared component-rendering harness, with provenance and
numeric assertions alongside them.

## Three kinds of image

A design wireframe describes intended organization. A component snapshot shows
what a particular widget tree rendered in a controlled harness. A native-window
screenshot shows the running application/plugin under an actual windowing system
and host. These answer different questions. A component snapshot can be highly
deterministic but misses host embedding, native menus, focus, scaling, and some
accessibility behavior.

An offline report image is another distinct artifact: it verifies report
presentation, not the plugin editor. A browser capture of a generated HTML report
must not be called a JUCE screenshot. An AI-generated mockup is never evidence
of implemented controls or measured acoustic data. Do not alter captured images
to remove defects before review; annotation belongs in a separate derivative.

## Deterministic fixture state

Create an explicit UI fixture containing parameter values, tuning/mapping state,
active/pending status, selected tab, focus target, viewport, theme, locale, and
telemetry snapshot. Use known synthetic diagnostic values when testing graph
layout, and label them synthetic. Do not capture live random meter values and
then widen image thresholds until the test stops failing.

Freeze clocks and animations through a test-only deterministic presentation
input, not by changing synthesis behavior. Record the sample time represented by
the telemetry fixture. Supply empty, valid, extreme, stale, failed, and overflow
states. A default-screen screenshot alone misses the states most likely to
truncate text or overlap controls.

Specify font files or a platform font policy, display scale, renderer, operating
system, and host. Fonts and rasterizers can differ across platforms. Maintain
platform-specific baselines where necessary rather than pretending every native
pixel must match Linux. A font substitution should be visible in the manifest
and investigated if it affects layout.

## Capture protocol

Build the candidate and record binary/source hashes. Launch the declared harness
or application, apply the fixture through public test interfaces, wait for the
specific ready condition, verify semantic state, and capture. “Wait a little”
is not a readiness condition. Use a deterministic completion signal for layout,
asset preparation, or telemetry installation.

Capture the intended region without cropping away errors. Preserve original
dimensions and scale. If a window border is excluded, state that choice and
test native embedding elsewhere. Store the raw capture, manifest, numeric state,
and any difference image. Use stable filenames containing case identity rather
than an ambiguous `latest.png` that later runs overwrite.

For a native plugin editor, record host, plugin format, architecture, display
configuration, and editor size. Include open/close and resize sequences. The
editor may look correct in a standalone window and fail when embedded in a DAW.
Native capture should be performed only through an authorized test environment;
do not inspect unrelated user windows or files.

## Numeric layout assertions

Assert that required controls exist, bounds are finite and inside the content
area, interactive targets have the intended minimum size, text is not clipped,
and prohibited overlaps do not occur. Test labels with long translations and
large values. A pixel comparison can miss a control whose hit region is displaced
while its drawing remains unchanged.

Use semantic IDs independent of translated labels. Assert focus order, enabled
state, displayed unit, value mapping, and active/pending distinction. For graph
panels, verify axis ranges, tick values, units, source revision, and plotted
numeric samples before inspecting pixels. A beautifully rendered graph of the
wrong channel is a failed test.

Check resizing with multiple aspect ratios and supported scale factors. Example
test scales such as 100%, 125%, 150%, and 200% are coverage candidates, not a
claim of current support. The actual support policy and minimum editor size
must be declared. Include transitions between scales where the platform permits,
not only launching at each scale.

## Image comparison policy

Use exact comparison only where the rendering path is deliberately deterministic.
For native text and platform rendering, use reviewed tolerances and separate
baselines. Threshold records name the image region, color space, alpha treatment,
metric, allowed deviation, and rationale. Every new threshold starts proposed;
do not adopt arbitrary percentages because they make a noisy test green.

A useful staged comparison checks dimensions and gross layout first, then
localized pixel differences, then reviewed structural or perceptual measures
where justified. A global similarity score can hide a missing small but critical
button. Give essential labels, focus rings, error messages, and status indicators
their own assertions or regions.

Handle alpha and color consistently. Comparing premultiplied pixels with straight
alpha values can produce false differences. Scaling screenshots after capture
can conceal one-pixel clipping. Keep originals and document any derivative used
for review. Do not mask dynamic regions unless the fixture cannot reasonably
control them, and explain what behavior the mask leaves untested.

## Interaction tests accompany images

Drive keyboard, mouse, text entry, reset, and automation routes, then assert
authoritative parameter state and core events. Capture before/after states to
support review, not to replace assertions. Test that a drag emits the correct
host gesture and that cancel restores the prior value. Test that opening a
diagnostic panel does not change rendered audio for a canonical event stream.

Exercise error paths: invalid tuning import, missing calibration asset,
preparation canceled, unsupported configuration, telemetry stale, and overflow
recovery. Verify that the old valid state remains active where promised. The
screen should not claim the new preset loaded when installation failed.

Test window closure during worker completion, rapid tab changes, and repeated
editor creation. Pair these with race/memory instrumentation in an appropriate
harness. A screenshot cannot detect a dangling pointer that only fails after
the editor closes.

## Baseline review

A baseline update is a code-review decision. Show old, new, and difference
images with manifests and semantic changes. Reviewers should know whether the
change is intended layout, a new feature, a font/rendering change, or an unexplained
regression. Do not batch-approve all images produced by a new build.

Retain historical baselines with their environment identity. If a runner image
changes, establish whether differences are cosmetic or reveal a real portability
problem. Changing the baseline is not a fix for clipped Persian text or a lost
focus ring. Add a semantic regression so the same defect cannot recur under a
different rasterizer.

## Artifact evidence and CI

Upload raw captures, manifests, state assertions, and difference images for
failed cases. Preserve successful reference captures needed for review. Native
GUI jobs may require a supported display setup or dedicated machine; mark
unavailable captures as unrun. Do not generate placeholder “screenshots” to satisfy
an artifact-presence check.

The initial synthetic report pipeline can provide real plots from numeric
fixtures. Those plots must retain their synthetic label and source data. Plugin
editor captures require an actually built and launched editor. If optional JUCE
has not been built, the evidence packet should say that rather than show the
wireframe as a substitute.

## Acceptance and next action

Acceptance combines numeric layout, semantic interaction, authentic capture,
reviewed threshold policy, and cross-platform evidence appropriate to the claimed
support. The next action is a small deterministic fixture suite for the first
implemented editor, with empty and failure states included from the beginning.
A screenshot is strongest when it illustrates a tested fact whose numeric and
behavioral evidence are available beside it.
