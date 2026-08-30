/* CORRECTION_FACTOR: Feed-forward gain on recoil disturbance
 *
 * Value: Higher = stronger compensation per frame
 *        Lower  = weaker compensation, more residual drift
 *
 * Tuned empirically to match the weapon's recoil impulse.
 */
const float CORRECTION_FACTOR = 3.37f;

/* YAW_PITCH_FACTOR: Game engine degrees-to-mouse scaling
 *
 * Value: 0.022 = Source engine default (CS2/CS:GO)
 *
 * Converts angular punch delta into raw mouse movement units.
 */
const float YAW_PITCH_FACTOR = 0.022f;

/* EMA_ALPHA: How fast the filtered target tracks the raw measurement
 *
 * Value: 1.0 = No smoothing, raw signal passes straight through
 *        0.0 = Heavy smoothing, estimate drags behind input,
 *              ignoring spikes and tracking only sustained trends
 *
 */
const float EMA_ALPHA = 0.25f;

/* MAX_ACCEL_X: Horizontal slew rate limit
 *
 * Value: Infinite = Instant snap to target
 *        0.40     = Max 0.40 units of horizontal change per frame
 *
 * Controls how aggressively the X correction can ramp.
 */
const float MAX_ACCEL_X = 0.40f;

/* MAX_ACCEL_Y: Vertical slew rate limit
 *
 * Value: Infinite = Instant snap to target
 *        0.35     = Max 0.35 units of vertical change per frame
 *
 */
const float MAX_ACCEL_Y = 0.35f;

/* NOISE_SCALE: Amplitude of hand micro-drift
 *
 * Value: 0.12 = Obvious jitter (newbie hands)
 *        0.035 = Barely perceptible drift (expert hands)
 *
 * Higher values look like tremor; lower values look like
 * steady aim with minor fatigue.
 */
const float NOISE_SCALE = 0.015f;

/* NOISE_CORRELATION: Temporal memory of hand drift
 *
 * Value: 0.0 = Independent white noise every frame (static)
 *        1.0 = Perfectly smooth fluid motion (frozen drift)
 *
 * High values create slow, natural wandering rather than
 * frame-to-frame twitching.
 */
const float NOISE_CORRELATION = 0.96f;

/* NOISE_MEAN_REVERSION: Per-frame pull back toward zero drift
 *
 * Value: 0.0 = Drift can wander indefinitely
 *        0.02 = Gentle centering force keeps drift bounded
 *
 * Prevents the correlated noise from meandering too far
 * from the intended aim point over long sprays.
 */
const float NOISE_MEAN_REVERSION = 0.01f;

/* RECOVERY_DECAY: Muscle tension release rate when not firing
 *
 * Value: 1.0 = No decay, filters hold state forever
 *        0.0 = Instant zeroing
 *
 * Each idle frame, control states lose (1 - decay) percent
 * of their magnitude. 0.82 gives a natural ~15-frame relaxation.
 */
const float RECOVERY_DECAY = 0.82f;

/* SETTLED_THRESHOLD: Hard-reset residual threshold
 *
 * Value: 0.001 = Fully decayed to near-zero
 *
 * Once total residual correction drops below this,
 * resetState() zeroes everything to prevent micro-drift.
 */
const float SETTLED_THRESHOLD = 0.001f;

/* RECALL_FREQ: Pattern mis-calibration oscillation speed
 *
 * Value: High = Rapid wobble along the spray pattern
 *        Low  = Slow, subtle deviation from true pattern
 *
 * Lower frequencies look like imperfect recall of the
 * recoil curve rather than uncontrolled shaking.
 */
const float RECALL_FREQ = 0.1f;

/* RECALL_AMP: Pattern mis-calibration strength
 *
 * Value: 0.0 = Perfect pattern recall
 *        0.04 = Slight under/over-pull on specific bullets
 *
 * Rhythmic error tied to shot index.
 */
const float RECALL_AMP = 0.04f;

/* SMOOTHSTEP_A / SMOOTHSTEP_B: S-curve coefficients
 *
 * Formula: t² * (A - B * t)
 * Result:  3t² - 2t³ (standard smoothstep)
 *
 * Softens the acceleration limit so small corrections
 * are slightly suppressed and large ones ramp naturally.
 */
const float SMOOTHSTEP_A = 3.0f;
const float SMOOTHSTEP_B = 2.0f;