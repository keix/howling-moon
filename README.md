# Howling-Moon

An embeddable Nginx module for Moonquakes.

## Overview

Howling-Moon is a minimal Nginx module that embeds the Moonquakes Lua 5.4 compatible runtime. If it cannot be embedded, it is not Lua. This project exists to prove that Moonquakes is truly an embeddable runtime.

Howl at the moon. It answers.

## Design Principles

- Nginx remains untouched.
- The module is a thin integration layer.
- The runtime is replaceable (Moonquakes or Lua).
- No hidden event loop.
- Worker-level runtime lifecycle.

## Architecture

```
Client
↓
Nginx
↓
Howling-Moon (module)
↓
Runtime Interface
↓
Moonquakes
```

## Non-Goals

- Not a web framework.
- Not an async runtime.
- Not a fork of Nginx.
- Not tied exclusively to Moonquakes.

## Status

Draft design phase.

## License

Copyright (c) 2026 Kei Sawamura, Released under the MIT License.