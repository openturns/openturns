---
name: openturns-review
description: 'OpenTURNS review guidelines.'
---

# OpenTURNS review guidelines

## When to use
- When asked to review a specific pull-request, branch, commit or code changes.

## What to review
- Fetch pull requests from the main repository https://github.com/openturns/openturns/pulls
- Check each commit contain distinct, atomic work
- Code
- Tests
- Documentation

## Code
- Check codings conventions using **`.agents/skills/openturns-conventions/SKILL.md`**
- Verify API consistency with similar code
- Check for logic errors, math errors, algorithmic cost, linear algebra, bugs, ...
- Scan for memory errors (out-of bound access, uninitialized values, ...)
- Look out for for performance bottlenecks

## Documentation
- Check documentation (or lack of) using **`.agents/skills/openturns-doc/SKILL.md`**

## Tests
- Check tests (or lack of) using **`.agents/skills/openturns-test/SKILL.md`**
