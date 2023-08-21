from pathlib import Path

base = Path(__file__).parent
p1 = base.joinpath("alice.md")
p2 = base.joinpath("alice2.md")

with open(p1, "r") as f:
    t1 = f.read()
with open(p2, "r") as f:
    t2 = f.read()
print(len(t1), len(t2))

t1 = "".join(c for c in t1 if c.isprintable() or c in ("\n",))
t2 = "".join(c for c in t2 if c.isprintable() or c in ("\n",))

with open(p1, "w") as f:
    f.write(t1)
with open(p2, "w") as f:
    f.write(t2)

for i in range(len(t1)):
    if t1[i] != t2[i] and t1[i] in ("'", '"'):
        t1 = t1[:i] + t2[i] + t1[i + 1 :]

with open(p2, "w") as f:
    f.write(t1)
