# Continuous stiff-string mechanics and boundary work

## Purpose, assumptions, and source scope

This chapter derives the baseline transverse equation and its energy balance
before a grid is introduced. Its inputs are reference geometry, positive linear
density, nonnegative tension and rigidity, loss parameters, boundary conditions,
initial displacement and velocity, and prescribed force density. Its outputs are
a precise PDE, admissible boundary ports, and analytic checks for implementation.

Assume a slender straight string, small transverse slope, constant coefficients,
and a reference interval \(0\le x\le L\). The baseline ignores longitudinal
inertia, torsion, rotary inertia, shear deformation, plasticity, and material
memory. Those exclusions are model limits, not claims that the physical instrument
has none of these effects. A wrapped, composite, damaged, or strongly curved string
may require an effective model whose fitted coefficients cannot be interpreted as
those of a homogeneous circular wire.

The linear PDE agrees in content with equation (2) of Bilbao and Torin,
*Numerical Simulation of String/Barrier Collisions: The Fretboard*:
<https://www.pure.ed.ac.uk/ws/portalfiles/portal/17485791/dafx14_final.pdf>.
The following derivation is explicit so signs, units, and endpoint work do not
depend on transcription. Smith's author-written section *The Stiff String* gives
independent background:
<https://www.dsprelated.com/freebooks/pasp/Stiff_String.html>.

## Deriving each term

For a small segment of reference length \(dx\), mass is \(\mu\,dx\).
The transverse component of a constant tensile force is approximately \(T u_x\).
The difference between the forces at the segment ends is \(T u_{xx}\,dx\).
This approximation uses small slope and neglects the tension change caused by
transverse stretching. Thus tension contributes \(T u_{xx}\), in N/m.

Euler–Bernoulli bending contributes the restoring term \(-EI u_{xxxx}\).
It can be derived without a beam sign convention by varying the nonnegative
bending energy

\[
V_b=\frac12\int_0^L EI\,u_{xx}^2\,dx.
\]

The interior functional derivative is \(EI u_{xxxx}\); force is its negative.
This distinction avoids memorizing competing conventions for the sign of bending
moment. A fourth spatial derivative requires two boundary conditions at each end,
not merely the endpoint displacement conditions of an ideal string.

Use a local velocity loss \(-2\mu\sigma_0u_t\), and a gradient-of-velocity loss
\(2\mu\sigma_1u_{txx}\). The second term has a positive differential sign but is
dissipative under appropriate boundaries because integration by parts produces
the negative square of \(u_{tx}\). The complete equation is

\[
\boxed{\mu u_{tt}
=T u_{xx}-EI u_{xxxx}
-2\mu\sigma_0u_t+2\mu\sigma_1u_{txx}+f(x,t).}
\tag{C1}
\]

Dividing by \(\mu\) gives

\[
u_{tt}=c^2u_{xx}-\kappa^2u_{xxxx}
-2\sigma_0u_t+2\sigma_1u_{txx}+f/\mu,
\quad c^2=T/\mu,\quad\kappa^2=EI/\mu.
\tag{C2}
\]

Do not confuse \(\kappa\) with \(EI/\mu\). Its square has units m⁴/s².
For a homogeneous circular wire of radius \(r\),
\(A=\pi r^2\), \(I=\pi r^4/4\), and \(\mu=\rho A\).
These geometrical identities do not prove that a measured setar string has
homogeneous material, a circular cross-section, or a freely bending termination.

## Power balance, step by step

Define velocity \(v=u_t\). Multiply C1 by \(v\), then integrate over the reference
interval. The inertia term is the derivative of kinetic energy:

\[
\int_0^L\mu u_{tt}u_t\,dx
=\frac{d}{dt}\frac12\int_0^L\mu u_t^2\,dx.
\]

Integrating the tension term once gives

\[
\int_0^L T u_{xx}u_t\,dx
=[T u_xu_t]_0^L
-\frac{d}{dt}\frac12\int_0^L T u_x^2\,dx.
\]

Integrating bending twice gives

\[
-\int_0^L EI u_{xxxx}u_t\,dx
=[-EI u_{xxx}u_t+EI u_{xx}u_{tx}]_0^L
-\frac{d}{dt}\frac12\int_0^L EI u_{xx}^2\,dx.
\]

The gradient loss gives

\[
\int_0^L2\mu\sigma_1u_{txx}u_t\,dx
=[2\mu\sigma_1u_{tx}u_t]_0^L
-2\mu\sigma_1\int_0^L u_{tx}^2\,dx.
\]

Consequently,

\[
\frac{dH}{dt}=P_{\mathrm{vol}}+P_{\mathrm{boundary}}-Q,
\tag{C3}
\]
\[
H=\frac12\int_0^L
\left(\mu u_t^2+T u_x^2+EI u_{xx}^2\right)dx,
\tag{C4}
\]
\[
Q=2\mu\sigma_0\int_0^L u_t^2dx+
2\mu\sigma_1\int_0^L u_{tx}^2dx,\qquad
P_{\mathrm{vol}}=\int_0^L f u_t\,dx,
\tag{C5}
\]
\[
P_{\mathrm{boundary}}
=\left[(T u_x-EIu_{xxx}+2\mu\sigma_1u_{tx})u_t
+EIu_{xx}u_{tx}\right]_0^L.
\tag{C6}
\]

The positive sign in the boundary contribution of the gradient loss follows
directly from C1. If a source, OCR extraction, or existing program uses another
sign, compare the definitions of traction and outward normal before changing
anything. Only the complete power expression is meaningful. An isolated plus
or minus sign without endpoint orientation is not an interface convention.

For nonnegative coefficients, \(H\ge0\) and \(Q\ge0\). In an unforced, closed
system, energy therefore cannot increase. Strict positive energy controls the
states only after rigid motions or unconstrained nullspaces have been addressed.
A free string with zero tension and zero rigidity is not controlled in position
by this energy.

## Endpoint port variables

Let
\[
\tau=T u_x-EIu_{xxx}+2\mu\sigma_1u_{tx},\qquad
m=EIu_{xx}.
\]
The bracket in C6 is \(\tau v+m\omega\), where \(\omega=u_{tx}\) is the
small-angle endpoint angular velocity. At the right end the supplied power is
\(\tau(L)v(L)+m(L)\omega(L)\). At the left end the bracket is subtracted.
Thus outward-orientation signs must be included when defining endpoint force
vectors. Store the orientation explicitly in the port transform.

An ideal translational bridge port may neglect rotational motion only as a
declared reduction. It must still supply an additional boundary condition for
the bending model, for example zero moment. A compliant bridge that carries a
moment requires both force/velocity and moment/angular-velocity work pairs.
Adding a moment feedback signal to a displacement-only port without a matching
rotation coordinate is dimensionally and energetically incomplete.

Common homogeneous choices are:

| Boundary type | Conditions at each endpoint | Why boundary power vanishes |
| --- | --- | --- |
| Simply supported | \(u=0,\ u_{xx}=0\) | \(u_t=0\) and bending moment is zero |
| Clamped | \(u=0,\ u_x=0\) | Both translational and angular velocities vanish |
| Natural free | \(\tau=0,\ m=0\) | Both generalized tractions vanish |
| Prescribed motion | Given \(u\), optionally \(u_x\) | Power is generally nonzero |
| Dynamic attachment | Coupled endpoint force and motion | Power cancels only in joint system |

For a lossy string, a natural free traction contains the gradient-loss term.
Using only \(T u_x-EIu_{xxx}=0\) while allowing nonzero endpoint velocity is a
different boundary model. The resulting energy balance must be recomputed.

## Modal solution for simply supported ends

With \(u(0)=u(L)=u_{xx}(0)=u_{xx}(L)=0\), the spatial functions
\(\sin(\xi_jx)\), \(\xi_j=j\pi/L\), diagonalize the constant-coefficient model.
Set \(u=a_j(t)\sin(\xi_jx)\). Then

\[
\ddot a_j+2d_j\dot a_j+\Omega_j^2a_j=F_j(t),
\quad d_j=\sigma_0+\sigma_1\xi_j^2,\quad
\Omega_j^2=c^2\xi_j^2+\kappa^2\xi_j^4.
\tag{C7}
\]

For an underdamped unforced mode, \(d_j<\Omega_j\),

\[
a_j(t)=e^{-d_jt}
\left[a_j(0)\cos(\omega_{dj}t)
+\frac{\dot a_j(0)+d_ja_j(0)}{\omega_{dj}}
\sin(\omega_{dj}t)\right],
\quad\omega_{dj}=\sqrt{\Omega_j^2-d_j^2}.
\tag{C8}
\]

The critical and overdamped cases need their own numerically stable evaluation.
Do not evaluate C8 with an imaginary frequency in a real-valued test harness.
Near critical damping use limiting formulas or a stable matrix exponential; the
formula can lose accuracy through subtraction and division by a small frequency.

The amplitude decay time for a factor of one thousand is
\(\ln(1000)/d_j\), when a single underdamped exponential envelope applies.
The same elapsed time corresponds to a factor of one million in modal energy.
Calling both quantities a “60 dB decay” is consistent only when the amplitude
uses \(20\log_{10}\) and energy uses \(10\log_{10}\). A multimode waveform is
not a single exponential and should not be forced into this test.

## Parameter variation adds work

If \(T=T(t)\) is changed externally with fixed \(\mu,EI,L\), differentiating C4
adds

\[
P_T=\frac{\dot T}{2}\int_0^L u_x^2\,dx.
\tag{C9}
\]

This is work by the tuning mechanism in the reduced model. Positive tension
variation may increase stored transverse energy without violating physics.
The correct audit is energy change minus accounted work, not simply whether
energy increased. Changing \(EI\), mass density, reference length, or equilibrium
shape similarly changes the energy expression and may require additional
mechanics. Treating all parameter changes as passive is incorrect.

A moving force distribution also has geometric work implications. If a contact
coordinate is \(s(t)=G(t)u(t)-b(t)\), then
\(\dot s=G\dot u+\dot G u-\dot b\). The term \(\dot G u\) is not captured by
interpolating the local velocity alone. This term becomes central for sliding
fingers and frets; the moving-domain chapter gives a discrete accounting method.

## Verification tasks

First, perform a symbolic dimensional audit of every C1 term; each must be N/m.
Second, substitute a single sinusoidal mode into C1 and recover C7 without using
the numerical solver. Third, evaluate C3 for a smooth manufactured field with
nonzero endpoint velocities and compare the volume and boundary terms. This
third check catches signs that a fixed-end test cannot expose.

For a synthetic no-loss fixture, use one mode with known displacement amplitude
and velocity. Compute initial energy by analytic integration and separately by
numerical quadrature. Refine quadrature independently from time integration.
For a damped fixture, compare displacement, velocity, and integrated dissipation;
matching displacement at one time is too weak because phase and amplitude errors
can cancel.

Reject any implementation whose energy expression omits stiffness, whose point
force is treated as a force density without spatial scaling, or whose endpoint
work disappears only because diagnostic code uses the wrong velocity. Also reject
a reported modal decay constant that uses the undamped frequency where the
specified measurement actually estimates the damped one.

## Limits and next action

The loss law is a compact model, not a universal description of material damping,
air drag, radiation, or contact loss. Fitting it to one string does not validate
it across gauges and tunings. Radiation should not be counted once in the string
decay coefficients and again in a coupled body without understanding the overlap.

The next action is to build discrete operators that preserve the integration-by-
parts structure of this derivation. A stable stencil in the interior is not enough;
endpoint closures and forcing maps determine whether the discrete system has the
same power pathways.
