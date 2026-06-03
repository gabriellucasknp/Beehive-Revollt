# 4-Phase Campaign Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Replace the old seven-stage loop with a four-phase campaign that varies enemy behavior, introduces a hive memory phase with C-logic puzzles, and improves background presentation.

**Architecture:** Keep the existing raylib shooter loop, but move campaign metadata into a small `campaign` module. Combat phases remain formation-based, phase two adds rush attackers, phase three becomes a dedicated hive trial state machine with mixed world/overlay puzzles, and phase four reuses the boss structure as the queen finale.

**Tech Stack:** C99, raylib, Make

---

### Task 1: Campaign Metadata And Tests

**Files:**
- Modify: `Makefile`
- Create: `src/campaign.h`
- Create: `src/campaign.c`
- Create: `tests/campaign_test.c`

- [x] Add a failing test target for four-phase metadata and hive memory progression.
- [x] Implement campaign phase definitions and the three-room hive memory progression API.
- [x] Run `make test` to verify both the existing player test and the new campaign test pass.

### Task 2: Combat Phase Variation

**Files:**
- Modify: `src/enemy.h`
- Modify: `src/enemy.c`
- Modify: `src/stage.h`
- Modify: `src/stage.c`

- [x] Extend enemy state with a rushing motion mode.
- [x] Update stage loading so phase definitions come from `campaign.c`.
- [x] Make phase two launch kamikaze-style attackers from the formation.

### Task 3: Hive Trial Gameplay

**Files:**
- Modify: `src/types.h`
- Modify: `src/quick_quiz.h`
- Modify: `src/quick_quiz.c`
- Modify: `src/game_state.h`
- Modify: `src/game_state.c`

- [x] Add explicit memory quiz and memory reveal states.
- [x] Support custom quick-quiz prompts for hive overlay questions.
- [x] Build the phase-three room loop with two in-world puzzle rooms, one overlay room, and three memory reveals.
- [x] Transition directly from the third memory reveal into the queen phase.

### Task 4: Background And Memory Visual Polish

**Files:**
- Modify: `src/assets.h`
- Modify: `src/assets.c`
- Modify: `src/game_state.c`

- [x] Generate a procedural memory shard sprite in code.
- [x] Rework background drawing to use scene-cover composition plus parallax hex layers.
- [x] Add hive-room UI panels so the phase reads like an environment instead of a flat repeating backdrop.

### Task 5: Verification

**Files:**
- Modify: `src/quick_quiz.h`
- Modify: `src/quick_quiz.c`

- [x] Clean up compile warnings introduced by the new quiz API.
- [x] Run `make`.
- [x] Run `make test`.
- [x] Review `git diff --stat` for final scope.
