# 安全控件管理服务

## 简介

安全控件是OpenHarmony提供的一组系统实现的ArkUI基础组件。当用户点击了这类组件后，应用将被授予临时授权，应用开发者仅需要像使用Button等组件那样集成到应用自己的页面即可。
安全控件管理服务主要提供如下功能：

-  提供应用安全控件的注册、临时授权、取消注册等Native接口实现。
-  负责安全控件的合法性检查，只有合法的安全控件才能授权成功。
-  提供其他SA查询临时授权的Native接口实现。提供安全增强相关接口及适配层，供厂商定制安全增强功能使用。

## 目录

```
/base/security/security_component
├── frameworks                  # 框架层，基础功能代码存放目录
│   ├── common                  # 框架公共代码存放目录
│   ├── enhance_adapter         # 能力增强适配代码存放目录
│   └── security_component      # 安全控件服务框架代码存放目录
├── interfaces                  # 接口层
│   └── inner_api               # 内部接口层
│       ├── enhance_kits        # 安全控件配置接口代码存放目录
│       └── security_component  # 安全控件服务接口层代码存放目录
└── services                    # 服务层
    └── security_component_service/sa
        └── sa_main             # 安全控件服务侧代码存放目录

```

## 使用
### 接口说明

| **接口申明** | **接口描述** |
| --- | --- |
| int32_t RegisterSecurityComponent(SecCompType type, const std::string& componentInfo, int32_t& scId); | 注册安全控件 |
| int32_t UpdateSecurityComponent(int32_t scId, const std::string& componentInfo); | 更新安全控件信息 |
| int32_t UnregisterSecurityComponent(int32_t scId); | 取消注册安全控件 |
| int32_t ReportSecurityComponentClickEvent(int32_t scId, const std::string& componentInfo, const SecCompClickEvent& touchInfo, sptr&lt;IRemoteObject&gt; callerToken); | 上报点击事件，申请临时授权 |
| int32_t SetEnhanceCfg(uint8_t* cfg, uint32_t cfgLen); | 设置安全控件增强的配置，供多模服务使用 |
| int32_t GetPointerEventEnhanceData(void* data, uint32_t dataLen, uint8_t* enhanceData, uint32_t& enHancedataLen); | 获取点击事件的安全增强数据，供多模服务使用 |
| bool ReduceAfterVerifySavePermission(AccessToken::AccessTokenID tokenId); | 校验后取消保存控件权限 |

## 相关仓

**[arkui\_ace\_engine](https://gitee.com/openharmony/arkui_ace_engine/blob/master/README_zh.md)**

**[multimodalinput\_input](https://gitee.com/openharmony/multimodalinput_input/blob/master/README_zh.md)**
