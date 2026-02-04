# AGENTS.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Basic Information

| Attribute | Value |
| -------- | -------- |
| Repository | security_component_manager |
| Subsystem | security |
| Language | C/C++ |
| Last Modified | 2026-01-31 |

## Overview

This is the security_component_manager repository providing a service called **Security Component Manager** - a system ability (SA ID: 3506) that allows applications to access temporary permissions through security components (e.g., PasteButton, SaveButton) without permanent authorization. When a user clicks a security component, the app receives temporary permission to access sensitive data.

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

## API Reference

- [Public API for PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton-sys.md)
- [Public API for SaveButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton.md)
- [System API for SaveButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-security-components-savebutton.md)
- [Common public API for SaveButton & PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-securitycomponent-attributes.md)
- [Common system API for SaveButton & PasteButton](../../../docs/zh-cn/application-dev/reference/apis-arkui/arkui-ts/ts-securitycomponent-attributes-sys.md)

## Constraints and Limitations

Due to the click-to-authorize nature of security components, the system imposes many restrictions to protect user privacy from malicious applications. App developers must ensure that security components are clearly visible on the app interface and can be clearly identified by users, to prevent authorization failures due to overlay, obfuscation, or other factors.

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

### Performance-Sensitive Interfaces

Security components are performance-sensitive. Avoid adding time-consuming logic to the following interfaces during development:

| Performance-Sensitive Interface | Purpose |
|-------------------------------|---------|
| `SecCompEnhanceKit::InitClientEnhance()` | Initializes relevant key information and probe callbacks during application spawning |
| `SecCompKit::RegisterSecurityComponent()` | Used for security component information registration |

**Note:** These interfaces are called during critical paths (app startup, component registration) and must complete quickly to avoid degrading user experience.

## History Record

| Version | Date | Content | Author |
| -------- | -------- | -------- | -------- |
| v1.0 | 2026-01-31 | Init AGENTS.md | harylee |
