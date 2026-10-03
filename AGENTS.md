# AGENTS.md — Mandatory AI\_WORKFLOW.md Documentation

> **Read this before you start any task.** It is binding for every AI agent, coding assistant, and human contributor working on this repository.

## 1. Purpose

This repository is a HackYeah 2026 Huawei Challenge submission. The challenge rules **require** an `AI_WORKFLOW.md` file at the repository root documenting all AI-assisted development. This is a graded criterion (*reproducibility and transparency of the development workflow — 10%*) and an explicit submission deliverable. Every agent must contribute to it continuously — **not at the end**.

## 2. The rule

**If you used an AI tool (yourself, a coding agent, an MCP server, a model, a Skill, a prompt, or generated output from any of these) for any part of the work, you must document it in `AI_WORKFLOW.md` at the repository root before you consider the task done.**

- The file lives at the **repository root** and is named exactly `AI_WORKFLOW.md`.
- Write in **English** (challenge requirement — no exceptions).
- Append or update in your own commit/PR — never overwrite entries written by others.
- Commit the file together with the code it describes, in the same commit where possible.

## 3. What must be documented — required sections

`AI_WORKFLOW.md` must contain these top-level sections at all times. Create the file with this skeleton if it does not exist:

```markdown
# AI Workflow

## 1. Tools and Models Used
<!-- Every AI model, coding agent, MCP server, Agent Skill, assistant; version and role of each -->

## 2. Prompts and Reusable Instructions
<!-- Main prompts, reusable instruction snippets, relevant configuration -->

## 3. Development Workflow
<!-- How AI was used from ideation → architecture → implementation → testing → debugging -->

## 4. Review, Testing and Validation
<!-- How generated output was verified: builds, tests, manual checks, code review -->

## 5. Known Limitations, Failed Approaches and Lessons Learned
<!-- What did not work, what was discarded, caveats of generated code -->

## 6. AI Features in the Product (only if the submission includes an AI feature)
<!-- Model/service used, inference flow, data handling, limitations, validation approach, privacy considerations -->
```

## 4. Per-contribution entry format

For each task you complete with AI assistance, append an entry like:

```markdown
### <YYYY-MM-DD HH:MM UTC> — <short task title>
- **Tool/Model:** <name + version>
- **Task:** <what was built/changed, with file paths>
- **Prompt(s):** <the actual prompt or a faithful summary; include reusable snippets verbatim>
- **Output handling:** <what you accepted, what you edited, what you rejected and why>
- **Validation:** <how you verified it: build result, tests run (with commands), manual check>
- **Limitations:** <known issues, tech debt, unverified claims in generated code>
```

## 5. Standards for *reproducible* documentation

A judge or a new contributor must be able to reconstruct your environment and your work from the file alone. Therefore every entry and every instruction in `AI_WORKFLOW.md` must be:

1. **Exact** — real tool names and versions (`gpt-5 via GitHub Copilot 4.x`, not "an AI assistant"); the actual prompt, not "asked it to fix a bug".
2. **Reproducible** — include the exact commands used (build, test, install, run), environment details (OS, SDK/API level, DevEco Studio version), and configuration needed to repeat the step.
3. **Honest** — document failures and rejected AI output too. Failed attempts are explicitly valued by the jury. Never present generated code as hand-written or vice versa.
4. **Up to date** — update the file in the same commit as the change it describes. Stale documentation is treated as missing documentation.
5. **Complete but safe** — document prompts and workflows as fully as reasonably possible, but **never** include API keys, credentials, tokens, personal data or other confidential information. Redact, don't omit the entry.

## 6. Also disclose (rules requirement)

Material pre-existing or third-party components (templates, boilerplate, libraries) used in the submission must be identified in the submission documentation — list them in `AI_WORKFLOW.md` under a **Third-party and Pre-existing Components** section (name, source, licence), unless a separate `NOTICE`/`THIRD_PARTY.md` exists.

## 7. Checklist before you finish any task

- [ ] Did I (or the agent invoking me) use any AI tool for this task?
- [ ] Is there an entry for it in root `AI_WORKFLOW.md` following the format in §4?
- [ ] Does the entry include exact tools/versions, prompts, commands and validation evidence?
- [ ] Are all secrets and personal data removed?
- [ ] Is the file committed together with the code it describes?
- [ ] Is everything written in English?

If any answer is "no", the task is **not done**.