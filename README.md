# 🛑 Checkpoint

Checkpoint is a tiny reverse proxy that attempts to block AI by serving a cryptographic challenge.

_This project is a work-in-progress. It works, but has only basic functionality._

## Why?

AI scrapers are everywhere. This will stop them. `robots.txt` won't.

## Features

- Protect your endpoint from AI bots with a cryptographic challenge
- Easy configuration in jsonc
- Support for cloudflare
- Support for IP-Range based rules (both ipv4 and ipv6)
- Support for async (multithreaded) request handling
- Minimal. The waiting page is tiny and light on network usage.
- Support for verbose traffic logging for later inspection or statistics

### Planned features

- Dynamic challenge amount (aka difficulty) based on traffic
- Detection of token overuse
- Better wait screen

## Caveats

If you are using this, it's almost certain search engines will stop indexing your site. Keep this in mind.

## Setup guide

1. Clone and build this repo. You will need `openssl`, `g++>=12`, `re2`, `libmagic`, and deps for `pistache`, `fmt` and `tinylates`.
   - Debug (learning) build with sanitizers and strict warnings:
     - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCHECKPOINT_ENABLE_SANITIZERS=ON -DCHECKPOINT_STRICT_WARNINGS=ON`
     - `cmake --build build`
   - Release build:
     - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`
     - `cmake --build build`

## Static analysis (clang-tidy)

- A starter `.clang-tidy` is provided to catch common pitfalls (bugprone, clang-analyzer, modernize, readability, performance).
- Run against the build compilation database:
  - `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`
  - `cmake --build build` (generates `build/compile_commands.json`)
  - `clang-tidy -p build $(git ls-files 'src/**/*.cpp')`

Tip: you can also integrate clang-tidy with your editor or CMake presets; start with Debug to keep feedback focused while learning.
2. Create a `config.jsonc`. An example one is in `example/`.
3. Adjust the config to your needs. Options are documented with comments in the example config.
4. Set up your IP rules if you want. These allow you to set up IPs that are automatically blocked, or allowed to access without a challenge. This is useful for e.g. search engine scrapers. Some IP ranges can be found in `example/index_bots.jsonc`.
5. Run checkpoint with your config: `./build/checkpoint -c config.jsonc`. How you run it long-term as a service is up to you.

## FAQ / Setup info

See [docs/FAQ](./docs/FAQ.md).
