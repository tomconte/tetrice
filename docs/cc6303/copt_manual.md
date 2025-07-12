# User Manual for `copt` - Peephole Optimizer

`copt` is a peephole optimizer originally by Christopher W. Fraser, designed for optimizing assembly code for 6800 series processors (and possibly others). It reads optimization rules and applies them to input code in multiple passes, supporting pattern matching, substitution, variables, regular expressions, and conditional rule activation.

---

## Usage

```
copt [options] <pattern-file> ...
```

### Standard Workflow

1. **Provide one or more pattern files** (text files containing optimization rules).
2. **Pipe assembly code into `copt`'s standard input** (or use a test file if `_TESTING` is enabled).
3. **Optimized code is written to standard output**.

---

## Command-Line Options

- `-D`
  - Enable debug mode. Prints detailed information about rule matching and substitutions to `stderr`.

- `-m<cpu>`
  - Set target CPU type (default: `6800`). Used for conditional rules with `%cpu` and `%notcpu`.

**Example:**
```
copt -D -m6809 optimizations.txt < input.asm > output.asm
```

---

## Pattern File Format

Pattern files define rules in the following form:

```
<pattern>
=
<substitution>
```
- Lines before `=` define the pattern to match.
- Lines after `=` define the code to substitute if the pattern matches.

### Special Pattern Syntax

- `%0`–`%9` : Capture variables from matched lines.
- `%L`–`%N`: Unique label variables.
- `%eval(expr)`: Evaluate an arithmetic expression using RPN (reverse Polish notation), can use variables.
- `%check min <= %n <= max`: Condition for rule application.
- `%check_eval result = expr`: Apply rule only if `expr` evaluates to `result`.
- `%cpu <type>`: Rule applies only to selected CPU.
- `%notcpu <type>`: Rule does not apply to selected CPU.
- `%activate`: Activates new rules dynamically when the current rule matches.
- `%once`: Rule can only fire once.
- Regular expressions: You can use `%\"...\"n` to match parts of a line with regex and assign to variable `n`.

---

## How It Works

1. **Rule Initialization:** Reads pattern files, parses rules.
2. **Input Loading:** Loads input assembly code.
3. **Optimization Passes:** Applies rules repeatedly (up to 16 passes or until no further changes).
4. **Rule Matching:** Each rule may include pattern lines, conditions, CPU checks, variable capturing, and substitutions.
5. **Rule Firing:** When a rule matches, it replaces code as specified; rules can activate new rules or be limited to a single use.
6. **Output:** Optimized code is printed to standard output.

---

## Example Rule

```
LDA %0
STA %0
=
NOP
```

This rule matches a load/store to the same address and replaces it with `NOP`.

---

## Debugging

- Use `-D` to enable debug output. This will print matched rules, substitutions, and activated rules to `stderr`.

---

## Error Handling

- If a pattern or substitution is invalid, or memory runs out, `copt` will print an error and exit.

---

## Advanced Features

- **Dynamic rule activation:** `%activate` can inject new rules during optimization.
- **Multiple variables:** `%0`–`%9` can capture and substitute multiple values per rule.
- **Conditional rules:** Use `%check`, `%check_eval`, `%cpu`, `%notcpu` for fine-grained control.
- **Regular expressions:** For complex pattern matching.

---

## Notes

- Up to 16 optimization passes are performed; if exceeded, the tool will warn and exit.
- Patterns and substitutions must end with newline characters.
- All processing is line-oriented.

---

## Summary Table

| Option         | Description                                    |
|----------------|------------------------------------------------|
| `-D`           | Enable debug output                            |
| `-m<cpu>`      | Set target CPU type                            |
| `<pattern-file>` | File containing optimization rules           |

