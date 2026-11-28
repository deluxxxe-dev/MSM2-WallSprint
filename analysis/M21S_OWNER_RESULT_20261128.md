# M21s owner result for M21r (2026-11-28)

Inspected: log `MSM2-WS-v0.1c48-20261128-210055.log` (all 6254 lines). Owner: camera basically smooth.

## Result (VERIFIED-RUNTIME)
- 16 ground entries, all started as ground entries (no float). Block found by `fast=1` after the load and the reload;
  `fast=2` once at the menu/first hero (epoch 1).
- Camera: entries with no live manager (1, 2, 9) selected the latest caller at hold start (`distance=-1.00`) and engaged
  0.1 s after the entry start. Every entry engaged the WS camera.
- After the reload (hero_epoch=5) entry 10 first re-engaged the manager from before the reload (0x126a6560780, it still
  passed the live check) and switched to the new caller 0x126a4ee5550 0.22 s later.

## M21s change
A new hero pointer drops the previous camera manager (restore if engaged), so an object from before a reload is never
written. Behaviour otherwise unchanged.

## State
First-WS-after-load float and first-entry camera are solved for load, fast travel/hero change and reload.
Remaining open items (FEATURE_ROADMAP): corner wrap (target look = the M21n ground -> wall switch), camera distance
jump on air entries, falling through other buildings under rotated gravity.
