# Sure of Stealing

An [SKSE](https://skse.silverlock.org/) plugin for Skyrim that stops you stealing things by accident.

Reaching for a plate on a table in someone's house, or grabbing loot next to an owned barrel, is an easy way to pick up a bounty you never intended. Sure of Stealing makes taking an owned item deliberate: the first interaction is ignored, and a second interaction on the same item goes through.

> **This is a maintained fork** of [clayne/SureOfStealing](https://github.com/clayne/SureOfStealing), with crash fixes and updated Skyrim support. See [Changes in this fork](#changes-in-this-fork).

**[Download on Nexus Mods](https://www.nexusmods.com/skyrimspecialedition/mods/178413)**

## What it does

When you try to take something it would be a crime to take, the first interaction is blocked and the second one — on the same object — succeeds.

This applies to:

- **Loose items** you pick up
- **Containers** (chests, barrels, sacks, wardrobes)
- **Coin purses**
- **Chairs and benches** — optional, and about sitting rather than stealing: it stops you dropping into a chair when you meant to interact with something near it

There are some deliberate exemptions, so the plugin stays out of the way:

- **Sneaking bypasses the check entirely** — if you are sneaking, a single interaction takes the item. Sneaking is treated as intent to steal.
- **Unread books** pass straight through, so reading is never interrupted.
- **Empty containers** pass straight through.
- The item must be **under your crosshair**, so the check never fires on something you are not looking at.

## Requirements

- Skyrim Special Edition, Anniversary Edition, or Skyrim VR
- [SKSE64](https://skse.silverlock.org/) (or SKSEVR for Skyrim VR)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) (VR users: the VR variant)

Built with [CommonLibSSE-NG](https://github.com/alandtse/CommonLibVR), so a single DLL supports SE, AE and VR.

## Installation

Install with a mod manager, or extract `SureOfStealing.dll` and `SureOfStealing.ini` to `Data/SKSE/Plugins/`.

## Configuration

Settings live in `Data/SKSE/Plugins/SureOfStealing.ini`:

```ini
[General]
bChairsAndBenches = true

[Log]
Debug = true
```

| Setting | Default | Description |
| --- | --- | --- |
| `bChairsAndBenches` | `true` | Also require two interactions before sitting on a chair or bench. |
| `Debug` | `true` | Write verbose logging to the SKSE log. Useful when reporting a problem. |

Logs are written to `Documents/My Games/Skyrim Special Edition/SKSE/SureOfStealing.log` (or the VR equivalent).

## Compatibility

### Immersive Interactions

If **Immersive Interactions** is installed, this plugin **requires** the accompanying patch:

> `Sure of Stealing - Immersive Interactions Patch.esp`

Without it the plugin will refuse to load and report an error, because it needs the patch's global to coordinate with Immersive Interactions. If you do not use Immersive Interactions, the patch is not needed.

### Skyrim VR and Physical Sneak

Because sneaking bypasses the check, VR users who enable the game's **Physical Sneak** setting should be aware that physically crouching — for example bending down to reach a low shelf — puts you in sneak, which allows an item to be taken on a single grab. If you rely on this plugin to prevent accidental theft, consider that interaction before enabling Physical Sneak.

## Changes in this fork

- **Fixed a crash** when an activation had no valid activator reference, which could occur with scripted NPC pickups (seen with Nether's Follower Framework outfit handling and with some custom followers).
- **Hardened reference handling** — null-guarded incoming objects, and switched cached references to `ObjectRefHandle` / `NiPointer` so unloaded references cannot be dereferenced.
- **Updated CommonLibSSE-NG to 6.7.1** for the Anniversary Edition 1.7.99 update, which introduced Address Library format v5. Verified in game on runtime 1.7.104.
- **Modernised the build and release pipeline** — pull request validation, no deprecated GitHub Actions, and automated release packaging.

## Building

Builds run in GitHub Actions on every pull request and on pushes to `main`; artifacts are attached to each run. To build locally you need Visual Studio with the C++ desktop workload, CMake, and [vcpkg](https://github.com/microsoft/vcpkg) with `VCPKG_ROOT` set.

```sh
git clone --recurse-submodules https://github.com/AirWolf359/SureOfStealing
cd SureOfStealing
cmake --preset build-release-msvc
cmake --build --preset release-msvc
```

The built DLL, PDB and ini are copied to `contrib/PluginRelease/skse/plugins/`.

To cut a release, push a tag beginning with `v` (for example `v1.2.0`), or publish a release from the GitHub UI — the workflow builds it, packages the mod archive and debug symbols, and attaches them to the release.

## License

[GPL-3.0-or-later](LICENSE), with the modding linking exceptions described in [EXCEPTIONS](EXCEPTIONS).
