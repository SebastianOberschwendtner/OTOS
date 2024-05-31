# Typedefs

|Type|Def|Description|
|---|---|---|
|`s_base_t`|<ul><li>*ARM*: `int32_t`<li>*AVR*: `uint8_t`</ul>|Base Type for signed variables.| 
|`u_base_t`|<ul><li>*ARM*: `uint32_t`<li>*AVR*: `uint8_t`</ul>|Base Type for unsigned variables.| 
|`stackpointer_t`| `uint32_t*` | Pointer to stack memory.|
|`taskpointer_t`| `void(*)(void)` | Function pointer to thread task.|