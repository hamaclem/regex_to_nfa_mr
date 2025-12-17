import re
from collections import defaultdict

input_text = """
Start state: 0
Accept state: 9
State 0: 'R' -> 1
State 1: EPSILON -> 2, EPSILON -> 4
State 2: 'Z' -> 3
State 3: EPSILON -> 2, EPSILON -> 4
State 4: 'B' -> 5
State 5: EPSILON -> 6, EPSILON -> 8
State 6: 'Z' -> 7
State 7: EPSILON -> 6, EPSILON -> 8
State 8: 'M' -> 9
"""

start_state = None
accept_states = set()
transitions = defaultdict(lambda: defaultdict(list))  # transitions[src][symbol] = [targets]
alphabet = set()
states = set()

# --- Parse the text ---
for line in input_text.splitlines():
    line = line.strip()
    if not line:
        continue

    # Start state
    m = re.match(r"Start state:\s*(\d+)", line)
    if m:
        start_state = int(m.group(1))
        states.add(start_state)
        continue

    # Accept state(s) (here only one, but this allows more)
    m = re.match(r"Accept state:\s*(\d+)", line)
    if m:
        acc = int(m.group(1))
        accept_states.add(acc)
        states.add(acc)
        continue

    # State X: ...
    m = re.match(r"State\s+(\d+):\s*(.*)", line)
    if m:
        src = int(m.group(1))
        states.add(src)
        rest = m.group(2)

        # Split by commas: "EPSILON -> 1, EPSILON -> 4"
        parts = [p.strip() for p in rest.split(",") if p.strip()]

        for p in parts:
            m2 = re.match(r"(EPSILON|'[^']')\s*->\s*(\d+)", p)
            if not m2:
                continue
            symbol_raw, target_raw = m2.groups()
            target = int(target_raw)
            states.add(target)

            # Convert symbol
            if symbol_raw == "EPSILON":
                symbol = "$"
            else:
                # symbol like 'A' → A
                symbol = symbol_raw.strip("'")

            # Add to structures
            transitions[src][symbol].append(target)
            if symbol != "$":
                alphabet.add(symbol)

# --- produce output ---

# states
print("#states")
for s in sorted(states):
    print(f"s{s}")
print()

# initial
print("#initial")
print(f"s{start_state}")
print()

# accepting
print("#accepting")
for a in sorted(accept_states):
    print(f"s{a}")
print()

#  alphabet
print("#alphabet")
for sym in sorted(alphabet):
    print(sym)
print()

# transitions
print("#transitions")
for src in sorted(transitions.keys()):
    for symbol, targets in transitions[src].items():
        # preserve order, but remove duplicates
        seen = set()
        uniq_targets = []
        for t in targets:
            if t not in seen:
                seen.add(t)
                uniq_targets.append(t)
        target_str = ",".join(f"s{t}" for t in uniq_targets)
        print(f"s{src}:{symbol}>{target_str}")