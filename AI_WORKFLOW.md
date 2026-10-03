# AI Workflow

This file documents all AI-assisted development for this submission, as required by the HackYeah 2026 Huawei Challenge rules. It is maintained continuously by every contributor (human and AI) — see `AGENTS.md` for the documentation rules. It is written in English and contains no credentials, API keys or personal data.

## 1. Tools and Models Used




| Tool / Model                 | Version | Role in the project              |
| ---------------------------- | ------- | -------------------------------- |
| e.g. Mistral Le Chat (agent) | —       | ideation, architecture           |
| e.g. GitHub Copilot          | —       | code generation in DevEco Studio |


*(Remove the example rows when adding real ones.)*

## 2. Prompts and Reusable Instructions



### Reusable snippets



## 3. Development Workflow



### Ideation

### Architecture

### Implementation

### Testing

### Debugging

## 4. Review, Testing and Validation



### Environment

- DevEco Studio: `<version>`
- SDK / API level: `<e.g. API 20>`
- Build system: `hvigor <version>`
- Device/emulator: `<name>`
- Build command: `hvigorw assembleHap` (adjust to actual)

### Validation log



## 5. Known Limitations, Failed Approaches and Lessons Learned

### Failed approaches



### Known limitations



### Lessons learned

## 6. AI Features in the Product

> **Delete this section if the submission contains no AI feature.**

- **Model / service used:** `<name, version, local/remote>`
- **Inference flow:** `<where it runs, what it receives/produces, on-device vs cloud>`
- **Data handling:** `<what user data is processed, where it stays, what is sent externally>`
- **Validation approach:** `<how correctness of model output is checked and handled>`
- **Limitations:** `<failure modes, edge cases, error handling for incorrect output>`
- **Privacy considerations:** `<consent, storage, anonymization>`

## Third-party and Pre-existing Components




| Component | Source | Licence | Pre-existing / developed during hackathon |
| --------- | ------ | ------- | ----------------------------------------- |
|           |        |         |                                           |


---

## Contribution Log

&lt;!-- Append per-contribution entries below. Format (see AGENTS.md §4):

### &lt;2026-10-03 13:11 UTC&gt; — &lt;Basic C++ engine&gt;
- \*\*Tool/Model: Claude sonnet 5.5\*\*
- \*\*Task: Creating a basic C++ engine for sound recognition\*\*
- \*\*Prompt(s): Create a C++ project, which contains an engine for recognising sounds essential for wellbeing of deaf people. Use CMake and include tests. The engine will be used in a mobile application for HarmonyOS.\*\*
- \*\*Output handling: The code has been throughoutly analysed and changes in effectiveness were applied\*\*
- \*\*Validation: The code has been analysed and checked, and the test was conducted sucessfully\*\*
- \*\*Limitations: The amount of sounds implementet on this stage was limited\*\*
--&gt;
### &lt;2026-10-03 14:19 UTC&gt; — &lt;Further development of engine&gt;
- \*\*Tool/Model: Claude sonnet 5.5\*\*
- \*\*Task: Further development of the engine by adding custom sounds feature\*\*
- \*\*Prompt(s): Add an external sound adding module for saving and future recognition. Include tests.\*\*
- \*\*Output handling: The code has been throughoutly analysed and changes in effectiveness were applied\*\*
- \*\*Validation: The code has been analysed and checked, and the test was conducted sucessfully. The imperfect sound files were sucessfully used as well\*\*
- \*\*Limitations: - \*\*
--&gt;
### &lt;2026-10-03 18:08 UTC&gt; — &lt;Adding speech recognition&gt;
- \*\*Tool/Model: Claude sonnet 5.5\*\*
- \*\*Task: Adding speech recognition as a substitute for sign language and special, warning keywords like "help , "watch out", etc.\*\*
- \*\*Prompt(s): Add a speech recognition module, which converts spoken language to the text understandable for the deaf. Add special functions to keywords like "help" or "watch out", which would cause the phone to vibrate. Include tests.\*\*
- \*\*Output handling: The code has been added to the directory, cmake file was configured and integrity was checked.\*\*
- \*\*Validation: The code has been analysed and checked, and the test was conducted sucessfully. Further validation will be possible while connecting the engine to the application\*\*
- \*\*Limitations: Small base of languages, which the program can recognise. Adding more options would be too difficult for the LLM to handle.\*\*
--&gt;
### &lt;2026-10-03 19:29 UTC&gt; — &lt;Polishing the reliability&gt;
- \*\*Tool/Model: Claude sonnet 5.5\*\*
- \*\*Task: Verification of the safety and validity of the tests, adding changes to improve the performance \*\*
- \*\*Prompt(s): Verify the reliability of the system and make sure to include the edge cases of tests. Suggest a modification of the code to provide maximum safety of the user.\*\*
- \*\*Output handling: The code has been verified by a human and the useful changes were applied. An additional test of safety was included in the tests directory.\*\*
- \*\*Validation: The code has been analysed and checked, and the new, more reliable tests were conducted.\*\*
- \*\*Limitations: - \*\*
--&gt;