# Simple Shell with Pipes (CSC415)

A minimalist Bourne‑style shell written in C for my **Operating Systems (CSC415)** course.  It supports basic command execution, pipelines, and a few built‑in utilities while keeping the codebase small and readable.

---

## Features
| Capability | Details |
|------------|---------|
| **Command parsing** | Tokenizer respects quotes (`"..."`, `'...'`) & escaped spaces | 
| **Process creation** | Uses `fork()` / `execvp()` / `waitpid()` to run external programs |
| **Pipelines** | Builds `n‑1` pipes for `n` commands (e.g., `ls -l | grep .c | wc -l`) |
| **Built‑ins** | `cd`, `exit`, `help`, prompt customization |
| **Error handling** | Clear messages for file/dir errors, malformed pipelines, or signals |
| **ANSI prompt** | Shows current dir and last exit code in color (configurable) |

---

## Architecture Overview
```
user input → lexer  → parser → exec_loop
                       │            │
                       │            ├─ built‑in handler
                       │            └─ pipeline builder → fork/exec chain
```
Key modules (≈300 LOC total):
- **lexer.c** – whitespace & quoting rules → token list
- **parser.c** – converts tokens to command structs & detects pipelines
- **shell.c**  – REPL loop, built‑ins, pipeline execution & prompt

---

## Build & Run
### Dependencies
* GCC (C11)
* POSIX‑compatible OS (Linux, macOS, WSL)

```bash
# clone repo
git clone https://github.com/<your‑handle>/simple-shell.git
cd simple-shell

# build
make            # produces ./sfshell

# run
./sfshell       # custom prompt shows up
```
Example session:
```sh
sfshell$ cd src
sfshell[src]$ ls -1 | grep "\.c$" | wc -l
3
sfshell[src]$ exit
```

---

## Configuration
| Env Var | Default | Description |
|---------|---------|-------------|
| `SHELL_PROMPT_COLOR` | `34` (blue) | ANSI color code for prompt |
| `SHELL_LOG_ERRORS`   | `0`         | Set `1` to append errors to `~/.sfshell.log` |

---

## Testing
Run unit tests with:
```bash
make test      # uses criterion (optional) – see test/ directory
```
Integration tests live in `scripts/` and validate pipelines, quotes, and built‑ins.

---

## Roadmap / Possible Extensions
* I/O redirection (`>`, `>>`, `<`)
* Background jobs (`&`) with `SIGCHLD` handling
* Command history using GNU Readline

---

## License
MIT – see `LICENSE` file.

---

## Author
**Eduardo Muñoz**  •  [LinkedIn](https://www.linkedin.com/in/eduardo-munoz-93b09523a)  •  [GitHub](https://github.com/smvckerz)
