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

- **Sneaking bypasses the check** — by default a single interaction takes the item, since sneaking is treated as intent to steal. This can be turned off with `bDoubleTapWhileSneaking`, or made mandatory with `bRequireSneakToSteal`.
- **Unread books** pass straight through, so reading is never interrupted.
- **Empty containers** pass straight through.
- The item must be **under your crosshair**, so the check never fires on something you are not looking at.

## Requirements

- Skyrim Special Edition, Anniversary Edition, or Skyrim VR
- [SKSE64](https://skse.silverlock.org/) (or SKSEVR for Skyrim VR)
- [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444) (VR users: the VR variant)

Optionally, [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352) adds an in-game settings menu. It is not required: without it the mod behaves identically and settings are read from the ini as usual.

Built with [CommonLibSSE-NG](https://github.com/alandtse/CommonLibVR), so a single DLL supports SE, AE and VR.

## Installation

Install with a mod manager, or extract `SureOfStealing.dll` and `SureOfStealing.ini` to `Data/SKSE/Plugins/`.

## Configuration

Settings live in `Data/SKSE/Plugins/SureOfStealing.ini`:

```ini
[General]
bChairsAndBenches = true
bRequireSneakToSteal = false
bDoubleTapWhileSneaking = false

[Log]
Debug = true
```

| Setting | Default | Description |
| --- | --- | --- |
| `bChairsAndBenches` | `true` | Also require two interactions before sitting on a chair or bench. |
| `bRequireSneakToSteal` | `false` | Refuse to take owned items at all unless you are sneaking, rather than asking for a second interaction. Applies to stealing only, never to sitting. |
| `bDoubleTapWhileSneaking` | `false` | Require the second interaction while sneaking as well, so sneaking no longer bypasses confirmation. Unlike the setting above, this also covers chairs and benches. |
| `Debug` | `true` | Write verbose logging to the SKSE log. Useful when reporting a problem. |

### Sneak behaviour

The two sneak settings control standing and sneaking independently, giving four modes:

| `bRequireSneakToSteal` | `bDoubleTapWhileSneaking` | Standing | Sneaking |
| --- | --- | --- | --- |
| `false` | `false` | Two interactions | One interaction *(default)* |
| `true` | `false` | Cannot steal at all | One interaction |
| `false` | `true` | Two interactions | Two interactions |
| `true` | `true` | Cannot steal at all | Two interactions |

Empty containers open on the first interaction in every mode, since there is nothing to steal.

### In-game settings menu

With **SKSE Menu Framework** installed, everything above can be changed while playing. Open the framework's menu and look for the **Sure of Stealing** section.

Changes apply immediately, with no reload, and are written to `SureOfStealingCustom.ini` so they persist. The ini shipped with the mod is never modified.

Without the framework there is simply no menu, and the mod is configured by editing the ini as described below.

### Overriding settings without editing the ini

The plugin never writes to `SureOfStealing.ini`, and you do not have to either. Create this file instead:

> `Data/SKSE/Plugins/SureOfStealingCustom.ini`

Any key it defines overrides the primary; anything it leaves out falls through. Include only the settings you want to change:

```ini
[General]
bDoubleTapWhileSneaking = true
```

This keeps your preferences separate from the file the mod ships. A mod update replaces the primary cleanly without touching your settings, and your mod manager does not report the mod's own files as modified. It follows the same convention as Engine Fixes and its `EngineFixesCustom.toml`.

Logs are written to `Documents/My Games/Skyrim Special Edition/SKSE/SureOfStealing.log` (or the VR equivalent).

## Compatibility

### Immersive Interactions

If **Immersive Interactions** is installed, this plugin **requires** a patch:

> `Sure of Stealing - Immersive Interactions Patch.esp`

The patch supplies a global variable the plugin uses to signal when it is holding an interaction back, so the two mods do not fight over the same activation. Without the patch, the game closes during startup with a message box naming it. If you do not use Immersive Interactions, no patch is needed.

**Two different versions of this patch exist, and they share the same filename.**

| If you use | Take the patch from |
| --- | --- |
| Immersive Interactions | the Sure of Stealing mod page |
| Immersive Interactions **and** First Person Interactions | First Person Interactions, which bundles its own version |

Because the filenames are identical, installing both means one silently overwrites the other. This plugin only checks the filename, so it loads whichever won the conflict and reports no error. If you use First Person Interactions, make sure its version is the one that wins, or its handling is lost with no warning.

### Container loot menus

Mods that let you take items directly from a container's loot preview, such as QuickLoot and its variants, bypass the container check. Nothing activates the container in that case: items move straight out of its inventory, so this plugin never sees the interaction.

Opening the container itself still requires confirmation as normal, including when opened from one of those mods. It is only taking items without opening it that is unaffected.

### Skyrim VR and Physical Sneak

Because sneaking bypasses the check by default, VR users who enable the game's **Physical Sneak** setting should be aware that physically crouching — for example bending down to reach a low shelf — puts you in sneak, which allows an item to be taken on a single grab.

If that is a problem for you, set `bDoubleTapWhileSneaking = true`. Confirmation is then required whether you are standing or crouched, so an incidental crouch no longer lets anything be taken without it.

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
