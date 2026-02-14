# howling-moon Architecture

## Design Philosophy

howling-moon is a bridge, nothing more.

- Does not pollute nginx internals
- Does not know Moonquakes implementation details
- Does not know Lua
- Speaks nginx's language, calls runtime's abstract API

## Layer Structure

```
┌─────────────────────────────────────────┐
│              nginx layer                │
├─────────────────────────────────────────┤
│           runtime abstraction           │
├─────────────────────────────────────────┤
│            runtime backend              │
└─────────────────────────────────────────┘
```

## File Structure

```
howling-moon/
├── config
├── include/
│   └── mq.h                           # Moonquakes ABI (external contract)
├── src/
│   ├── nginx/                         # nginx layer (ngx_http_hm_*)
│   │   ├── ngx_http_hm_module.c
│   │   ├── ngx_http_hm_handler.c
│   │   └── ngx_http_hm_lifecycle.c
│   └── runtime/                       # runtime layer (hm_*)
│       ├── hm_runtime_api.h
│       └── hm_runtime_stub.c
└── docs/
    └── howling-moon-architecture.md
```

## Naming Convention

| Layer | Prefix | Example |
|-------|--------|---------|
| nginx | `ngx_http_hm_` | `ngx_http_hm_handler.c` |
| runtime API | `hm_runtime_api` | `hm_runtime_api.h` |
| runtime backend | `hm_runtime_<name>` | `hm_runtime_stub.c` |

## Component Responsibilities

### ngx_http_hm_module.c (nginx layer)

- Defines `ngx_module_t`
- Registers directives (`hm`)
- Sets up module context
- Registers lifecycle hooks

### ngx_http_hm_handler.c (nginx layer)

- Content phase handler
- Extracts request context from `ngx_http_request_t`
- Delegates execution to runtime
- Converts runtime result to nginx response

### ngx_http_hm_lifecycle.c (nginx layer)

- `init_process`: Creates runtime instance per worker
- `exit_process`: Destroys runtime instance
- Manages global runtime pointer

### hm_runtime_api.h (abstraction layer)

- Defines `hm_runtime` opaque structure
- Defines `hm_runtime_ops` vtable
- Backend-agnostic interface

### hm_runtime_stub.c (backend layer)

- Stub implementation for testing
- Returns fixed response
- No external dependencies

## Runtime Interface (vtable)

```c
typedef struct hm_runtime hm_runtime;

typedef struct hm_runtime_ops {
    ngx_int_t (*init)(hm_runtime *rt, ngx_log_t *log);
    void (*shutdown)(hm_runtime *rt);
    ngx_int_t (*execute)(hm_runtime *rt,
                         ngx_http_request_t *r,
                         ngx_chain_t **out);
} hm_runtime_ops;

struct hm_runtime {
    const hm_runtime_ops *ops;
    void *backend_data;
    ngx_log_t *log;
};
```

## Dependency Graph

```
ngx_http_hm_module.c
         │
         ├────────────────────────┐
         │                        │
         ▼                        ▼
ngx_http_hm_lifecycle.c    ngx_http_hm_handler.c
         │                        │
         └───────────┬────────────┘
                     │
                     ▼
            hm_runtime_api.h
                     │
                     ▼
          ┌──────────┴──────────┐
          │                     │
          ▼                     ▼
  hm_runtime_stub.c    hm_runtime_mq.c (future)
```

## mq.h Position

`include/mq.h` defines the frozen ABI for Moonquakes embedding.

- **Current status**: Not used by howling-moon core
- **Future**: `hm_runtime_mq.c` will include `mq.h`
- **Rule**: nginx layer never includes `mq.h`

This separation ensures howling-moon remains runtime-agnostic.

## Evolution Path

### v1 (Current)

- module + handler + lifecycle + runtime abstraction
- stub backend only
- Fixed response

### v2 (Moonquakes Integration)

- Add `hm_runtime_mq.c`
- Implement `mq.h` calls
- Script execution

### v3 (Production Features)

- Configuration options
- Script caching
- Request/response binding
- Error handling

## Key Principles

1. **nginx code never sees runtime internals**
   - Handler calls `rt->ops->execute()`, nothing else

2. **Runtime backend is swappable**
   - vtable allows multiple implementations
   - Build-time or runtime selection

3. **One runtime per worker**
   - Created in `init_process`
   - Destroyed in `exit_process`
   - No cross-worker sharing

4. **Minimal surface area**
   - Only expose what's needed
   - Grow interface incrementally

5. **Naming reflects boundary**
   - `ngx_http_hm_*` = nginx transport layer
   - `hm_*` = howling-moon runtime layer
   - `mq_*` = Moonquakes external API
