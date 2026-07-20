# AGENTS.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Basic Information

| Attribute | Value |
| -------- | -------- |
| Repository | security_component_manager |
| Subsystem | security |
| Language | C/C++ |
| Last Modified | 2026-07-10 |

## Scope

This instruction file applies to the repository root unless a deeper `AGENTS.md` overrides it for a subdirectory.

## Overview

This is the security_component_manager repository providing a service called **Security Component Manager** - a system ability (SA ID: 3506) that allows applications to access temporary permissions through security components (e.g., PasteButton, SaveButton) without permanent authorization. When a user clicks a security component, the app receives temporary permission to access sensitive data.

## Working Agreement For Agents

Before editing code, state:

1. Task category: API / framework / service / enhance adapter / test / build config / documentation.
2. Files or directories you plan to modify.
3. Constraints discovered from this file and any routed documents.

If the task touches permissions, authorization lifecycle, public headers, SA profile, or enhancement callbacks, read the matching routing section below before editing.

## Build System

This project uses **GN (Generate Ninja)** with the OpenHarmony build framework.

### Building

Build commands are typically run from the OpenHarmony root directory:

```bash
# Build the entire security_component_manager module
./build.sh --product-name <product> --build-variant root --build-target security_component_manager

# Build specific module
./build.sh --product-name <product> --build-variant root --build-target security_component_build_module

# Build module by part compile. -i indicates compiling function code
hb build security_component_manager -i

# Build tests by part compile. -t indicates compiling test cases
hb build security_component_manager -t --gn-args use_cfi=false use_thin_lto=false
```

Common product names: rk3568, ohos-sdk etc.

### Testing

```bash
# Build unit tests
./build.sh --product-name <product> --build-variant root --build-target security_component_build_module_test

# Build fuzz tests
./build.sh --product-name <product> --build-variant root --build-target security_component_build_fuzz_test --gn-args use_thin_lto=false --gn-args use_cfi=false
```

Test source locations:
- Unit tests: `*/test/unittest/`
- Fuzz tests: `test/fuzztest/`

Unit test executables include:
- `sec_comp_enhance_adapter_test` - Enhancement feature adapter layer tests
- `sec_comp_enhance_sdk_test` - Enhancement feature kit tests
- `sec_comp_register_callback_test` - Security Component register and callback check tests
- `sec_comp_sdk_test` - Security Component sdk tests
- `sec_comp_service_mock_test` - Security Component service mock tests
- `sec_comp_service_test` - Security Component service tests

Compiled unit test executables above are located at: `out/rk3568/tests/unittest/security_component_manager`

Fuzz test executables include:
- `GetPointerEventEnhanceDataFuzzTest` - test SecCompEnhanceKit::GetPointerEventEnhanceData() interface
- `RegisterSecurityComponentFuzzTest` - test SecCompKit::RegisterSecurityComponent() interface
- `ReportSecurityComponentClickEventFuzzTest` - test SecCompKit::ReportSecurityComponentClickEvent() interface
- `SetEnhanceCfgFuzzTest` - test SecCompEnhanceKit::SetEnhanceCfg() interface
- `UnregisterSecurityComponentFuzzTest` - test SecCompKit::UnregisterSecurityComponent() interface
- `UpdateSecurityComponentFuzzTest` - test SecCompKit::UpdateSecurityComponent() interface
- `VerifySavePermissionFuzzTest` - test SecCompKit::VerifySavePermission() interface
- `PreRegisterSecCompProcessStubFuzzTest` - test SecCompService::PreRegisterSecCompProcess() interface
- `RegisterSecurityComponentStubFuzzTest` - test SecCompService::RegisterSecurityComponent() interface
- `ReportSecurityComponentClickEventStubFuzzTest` - test SecCompService::ReportSecurityComponentClickEvent() interface
- `UnregisterSecurityComponentStubFuzzTest` - test SecCompService::UnregisterSecurityComponent() interface
- `UpdateSecurityComponentStubFuzzTest` - test SecCompService::UpdateSecurityComponent() interface
- `VerifySavePermissionStubFuzzTest` - test SecCompService::VerifySavePermission() interface

Compiled fuzz test executables above are located at: `out/rk3568/tests/fuzztest/security_component_manager`

## Architecture

### System Architecture

Security Components are a set of advanced components built on ArkUI basic components provided by OpenHarmony, including the Save Button and Paste Button. Compared to the dynamic permission request approach, the main advantages are:

- Users control when to authorize, with minimized authorization scope
- Reduced interruption from permission dialogs

#### Security Component List

The system currently provides two types of security components:

- Paste Button (PasteButton)

This component corresponds to clipboard read privilege. After an app integrates the Paste Button, when users click it, the app can read clipboard data without a dialog prompt. The app's permission is revoked after going to the background for 10 seconds.

- Save Button (SaveButton)

This component corresponds to media library write privilege. After an app integrates the Save Button, when users use it for the first time, a confirmation dialog is displayed. After clicking "Allow", authorization is automatically granted, and the app obtains temporary access to media library.

#### Operation Mechanism

This repository implements the **Security Component Management Service** in a three-layer architecture for OpenHarmony (C++).

```
┌───────────────────────────────────────────────────────────────┐
│ LAYER 1: Client Application (ArkUI UI Components)             │
│ • PasteButton, SaveButton components                          │
│ • User clicks component                                       │
└───────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌───────────────────────────────────────────────────────────────┐
│ LAYER 2: Security Component Management Service (THIS REPO)    │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ Security Component Management Service                   │  │
│  │ • Component registration and management                 │  │
│  │ • Temporary authorization mechanism                     │  │
│  │ • Authorization lifecycle management                    │  │
│  │ • System Ability (SA ID: 3506)                          │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                               │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ Enhancement Framework (Extensible)                      │  │
│  └─────────────────────────────────────────────────────────┘  │
└───────────────────────────────────────────────────────────────┘
                              │
                              ▼ (request permission dialog)
                              │
                              ▲ (user authorization result)
                              │
┌───────────────────────────────────────────────────────────────┐
│ LAYER 3: Permission Management Application                    │
│ Dialog-based user authorization confirmation                  │
│ • Shows authorization dialog                                  │
│ • User confirms/rejects permission                            │
└───────────────────────────────────────────────────────────────┘
```

**Key Points:**
- Layer 1 (ArkUI components) sends registration/update/click events to Layer 2
- Layer 2 (this repo) consists of:
  - **Management Service**: Processes requests and manages authorization lifecycle
  - **Enhancement Framework**: Provides security validations to prevent abuse
- Layer 2 communicates with Layer 3 (Permission Manager) only for first-time SaveButton authorization

#### Enhancement Framework (Extensible)

The Security Component Service communicates with the Enhancement Framework through an adapter layer located at `frameworks/enhance_adapter`. Developers need to provide the following two shared libraries, which the Security Component Framework can flexibly load using dlopen to utilize their enhanced capabilities. For specific interfaces that need to be adapted, refer to the source code files in the `frameworks/enhance_adapter` directory:

- `libsecurity_component_client_enhance.z.so`
- `libsecurity_component_service_enhance.z.so`

Developers can customize security capabilities in the Enhancement Framework, including but not limited to:
- Address randomization
- Challenge value verification
- UI framework callback verification
- Caller address verification
- Component anti-overlay protection
- Genuine click event validation

### Directory Structure

```
security_component_manager/
├── frameworks/                         # Core framework implementation layer
│   ├── common/                         # Common utilities and data definitions
│   ├── enhance_adapter/                # Enhancement Adapter (feature extension layer)
│   ├── inner_api/                      # Internal API implementations
│   │   ├── enhance_kits/               # Enhancement feature kit implementation
│   │   └── security_component/         # Security component client implementation
│   └── security_component/             # Specific component implementations (paste button, save button)
├── interfaces/                         # Public Interface Definition Layer (exposed APIs)
│   └── inner_api/
│       ├── security_component/         # Public headers (component base class, Kit interfaces, etc.)
│       └── security_component_common/  # Common interface definitions
├── services/                           # Service implementation layer
│   └── security_component_service/     # Security component service
│       └── sa/                         # System Ability (system capability service)
│           ├── sa_main/                # Service main implementation
│           └── sa_profile/             # Service configuration files (JSON, BUILD config)
└── test/                               # Tests
    └── fuzztest/                       # Fuzz tests
```

The codebase follows OpenHarmony's System Ability pattern with three main layers:

### 1. Interface Layer (`interfaces/`)
- `SecCompKit` - Main API class for security component operations
- `SecCompBase` - Base class for all security components
- `SecCompInfo` - Component info structure and common data types
- `SecCompEnhanceKit` - Enhancement framework API
- `ISecCompProbe` - Probe interface for component info

### 2. Framework Layer (`frameworks/`)
- `SecCompClient` - IPC client singleton for communicating with service
- `SecCompDeathRecipient` - Service death recipient
- `SecCompDialogCallback` - Dialog callback interfaces
- Enhancement adapter interfaces for vendor customization

### 3. Service Layer (`services/`)
- `SecCompService` - Main SystemAbility class, inherits from `SystemAbility`
- `SecCompManager` - Core business logic manager (singleton pattern)
- `SecCompEntity` - Represents individual security components
- `AppStateObserver` - Monitors app lifecycle (foreground/background transitions)

## Where To Look

| Task | Primary paths | Notes |
| -------- | -------- | -------- |
| Change public component API or data types | `interfaces/inner_api/security_component/`, `interfaces/inner_api/security_component_common/` | Treat as compatibility-sensitive. Check API references before editing. |
| Change client-side kit or IPC entry | `frameworks/inner_api/security_component/`, `frameworks/common/` | Verify IPC call flow still matches service behavior. |
| Change enhancement capability loading or callbacks | `frameworks/enhance_adapter/`, `frameworks/inner_api/enhance_kits/` | Do not weaken validation or add heavy logic on critical paths. |
| Change service authorization logic | `services/security_component_service/` | Check authorization lifecycle, app state handling, and dialog interaction assumptions. |
| Change SA startup or profile | `services/security_component_service/sa/sa_main/`, `services/security_component_service/sa/sa_profile/` | Confirm SA ID, profile fields, and startup behavior remain compatible. |
| Add or update tests | `*/test/unittest/`, `test/fuzztest/` | Prefer adding coverage near the touched interface or service flow. |

## Knowledge Routing

### Task-based routing

- If changing `PasteButton`, `SaveButton`, or shared component attributes, read the matching files in `## API Reference` before editing interfaces or behavior.
- If changing code under `frameworks/enhance_adapter/`, read the adapter source there first and preserve the contract expected by `libsecurity_component_client_enhance.z.so` and `libsecurity_component_service_enhance.z.so`.
- If changing service authorization flow, app background behavior, or dialog interaction, review `## Architecture`, `## Constraints and Limitations`, and the related repository `applications/standard/permission_manager`.
- If changing ArkUI-facing semantics or component registration flow, review the related repository `foundation/arkui/ace_engine` to confirm the caller-side expectation.

### Path-based routing

- `interfaces/`: treat as public contract surface; read all relevant API reference documents before editing.
- `frameworks/inner_api/security_component/`: inspect both the client entry and the service-side stub/proxy path before changing request or reply fields.
- `services/security_component_service/`: inspect manager, entity, and app state observer together when changing lifecycle logic.

### Vocabulary-based routing

- If the task mentions `PasteButton`, `SaveButton`, `SecCompKit`, or `SecCompBase`, inspect `interfaces/inner_api/security_component/` and the matching API reference documents.
- If the task mentions `enhance`, `probe`, `challenge`, `anti-overlay`, or `genuine click`, inspect `frameworks/enhance_adapter/` and `frameworks/inner_api/enhance_kits/`.
- If the task mentions `dialog`, `permission_manager`, `access_token`, or temporary authorization, inspect service authorization logic and related repository references.

## API Reference

- [Public API for PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton-sys.md)
- [Public API for SaveButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton.md)
- [System API for SaveButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton.md)
- [Common public API for SaveButton & PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-securitycomponent-attributes.md)
- [Common system API for SaveButton & PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-securitycomponent-attributes-sys.md)

## Constraints and Limitations

When modifying this repository, preserve the following development constraints:

- Keep null checks and parameter validation intact on all IPC, dialog callback, registration, and click-report paths. Do not remove defensive handling for null pointers, invalid tokens, malformed parcel data, or invalid component info.
- Preserve the behavioral difference between `PasteButton` and `SaveButton`. `PasteButton` maps to clipboard read privilege and does not use the first-use confirmation dialog flow, while `SaveButton` maps to media library write privilege and includes first-use dialog handling.
- Preserve temporary authorization semantics and lifecycle checks. Do not weaken click validation, background-state handling, or the logic that limits authorization to the expected user interaction flow.
- Preserve anti-abuse checks around overlay detection, challenge or extra-info validation, caller verification, and genuine click verification.
- Keep service, framework, and interface layering stable. Business logic changes usually need related checks in service manager, entity validation, dialog flow, and client IPC handling together rather than isolated edits in a single file.
- Maintain compatibility of public structs, IPC field meanings, and SA-facing behavior unless the task explicitly requires a reviewed compatibility change.

## Do Not

- Do not change public header signatures, exported data structures, callback semantics, error behavior, or authorization semantics under `interfaces/` without explicit compatibility review.
- Do not change the temporary authorization lifecycle for security components unless the task explicitly requires it and the impact is reviewed end-to-end. This includes first-use dialog behavior, background revocation timing, and click-to-authorize assumptions.
- Do not bypass, weaken, stub out, or silently reorder enhancement checks related to challenge verification, callback verification, anti-overlay protection, caller verification, or genuine click validation.
- Do not add blocking or expensive logic to performance-sensitive paths such as `SecCompEnhanceKit::InitClientEnhance()` and `SecCompKit::RegisterSecurityComponent()`.
- Do not change SA identity, profile fields, startup behavior, or IPC-facing request/response fields casually. These may affect cross-process compatibility.
- Do not edit generated artifacts or derived outputs directly. Modify the source definition, profile input, or build input first, then regenerate when the workflow requires it.
- Do not modify dependency direction to make service code depend on UI-layer repositories directly.
- Do not remove, repurpose, or silently weaken existing diagnostic logs, fault attribution, or security-related events without checking downstream observability impact.
- Do not treat README-style architecture text as sufficient proof. Verify behavior in the relevant source files before changing control flow.

## Ask Before

- Ask before changing any file under `interfaces/` if the change alters observable API behavior or compatibility.
- Ask before changing authorization timing, permission scope, dialog trigger conditions, or lifecycle semantics.
- Ask before changing `services/security_component_service/sa/sa_profile/` or anything that may affect SA registration, deployment, or service identity.
- Ask before introducing a new dependency, changing build targets, or altering cross-repository interaction assumptions with `ace_engine` or `permission_manager`.
- Ask before changing log levels, event names, event fields, or other DFX behavior used for security diagnosis, compatibility tracking, or fault attribution.
- Ask before removing tests, fuzz targets, or security validations.

## Dependencies

### External Dependencies
- `samgr` - System Ability Manager
- `ipc_core` - IPC communication
- `access_token` - Permission manager
- `ability_runtime` - Application lifecycle
- `window_manager` - Window management
- `hilog` - Logging
- `hisysevent` - System events

### Related Repositories
- `foundation/arkui/ace_engine` - Ace engine for UI components (corresponds to **LAYER 1** in Operation Mechanism architecture diagram)
- `applications/standard/permission_manager` - Implementation of application for Security components dialog (corresponds to **LAYER 3** in Operation Mechanism architecture diagram)

## Common issues

### Registration-Time Information And Validation

- Do not treat registration-time validation and click-time authorization validation as equivalent. Security-component registration does not need to perform every security check that depends on live runtime state; checks that are strongly tied to click authorization, window state, interaction timing, or dynamic environment state should remain on the click path unless the task explicitly requires a reviewed behavior change.
- Before adding information retrieval, state queries, or security checks to the registration flow, first determine whether that data is truly required at registration time. Do not move data collection or validation from the click or authorization path into the registration path by default only because the same shared validation logic can be reused.
- Registration may happen when the component has just become visible, while window properties, UI state, or related system state are still updating and have not yet converged. In that timing window, retrieved window information, display information, scaling state, coverage relationship, or other runtime security-relevant state may be stale, incomplete, or unstable, causing false validation failures or incorrect component state updates.
- If new information retrieval or security validation must run during registration, verify end-to-end that the required data is already stable at registration time, and review the data acquisition path, timing assumptions, fallback behavior on retrieval failure, and the responsibility boundary between registration-time checks and click-time checks.

### Performance-Sensitive Interfaces

Security components are performance-sensitive. Avoid adding time-consuming logic to the following interfaces during development:

| Performance-Sensitive Interface | Purpose |
|-------------------------------|---------|
| `SecCompEnhanceKit::InitClientEnhance()` | Initializes relevant key information and probe callbacks during application spawning |
| `SecCompKit::RegisterSecurityComponent()` | Used for security component information registration |

**Note:** These interfaces are called during critical paths (app startup, component registration) and must complete quickly to avoid degrading user experience.

## Minimum Validation

- For changes in `interfaces/` or client kits, build at least the module target and the affected unit tests.
- For changes in service logic, build the module and the service-related unit tests. If authorization flow changes, prefer running the relevant service tests and callback tests.
- For changes in enhancement adapter logic, build the module and the enhancement adapter or enhancement sdk unit tests.
- For changes in fuzz entry points or IPC parsing, build the affected fuzz targets when possible.
- If only documentation is changed, state that no code validation was required.
- This repository does not currently define a dedicated lint or standalone API compatibility check in this file. If such checks are unavailable for the task, state that explicitly in the final response.

Use the commands in `## Build System` for module builds, unit test builds, and fuzz test builds.

If you cannot run validation, report:

1. What was not run.
2. Why it was not run.
3. The resulting risk.

## Done Definition

A task is done only when:

1. The changed files match the requested scope.
2. Routed documents and code paths were checked for the touched behavior.
3. Relevant build or test validation was run, or the gap is explicitly reported.
4. Public API, authorization semantics, and enhancement/security constraints were either preserved or explicitly escalated.

## Final Response Expectations

In the final response, include:

1. What changed.
2. Which constraints were considered.
3. What validation was run.
4. What could not be validated, if anything.

## History Record

| Version | Date | Content | Author |
| -------- | -------- | -------- | -------- |
| v1.1 | 2026-07-10 | Add agent routing, constraints, and validation guidance | harylee |
| v1.0 | 2026-01-31 | Init AGENTS.md | harylee |
