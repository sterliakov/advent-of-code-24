from pathlib import Path


def _solve(edges, vertices):
    ans = 0
    for a, b in edges:
        if not a.startswith("t"):
            continue
        for c in vertices:
            if c>b and (a, c) in edges and (b, c) in edges:
                ans += [6,3,1][(b[0]=='t')+(c[0]=='t')]
    return ans//6


def parse(inp):
    edges = {tuple(ln.split("-")) for ln in inp.splitlines()}
    edges |= {(b, a) for a, b in edges}
    return edges


def part1(inp):
    edges = parse(inp)
    vertices = {vertices for p in edges for vertices in p}
    return _solve(edges, vertices)


def part2(inp):
    edges = parse(inp)
    nodes = {}
    for p, q in edges:
        nodes.setdefault(p, set()).add(q)
    seen = set()
    best = set()
    curr = set()
    for n1, reachable in nodes.items():
        if n1 in seen:
            continue
        seen.add(n1)
        curr = {n1}
        for n2 in reachable:
            if curr <= nodes[n2]:
                seen.add(n2)
                curr.add(n2)
        if len(curr) > len(best):
            best = curr
    return ",".join(sorted(best))


test = Path("./inputs/day23/test.txt").read_text()
real = Path("./inputs/day23/real.txt").read_text()
assert part1(test) == 7, part1(test)
assert part2(test) == "co,de,ka,ta"

print("Part 1: ", part1(real))
print("Part 2: ", part2(real))
