# Typedefs

|Type|Def|Description|
|---|---|---|
|`s_base_t`|<ul><li>*ARM*: `int`<li>*AVR*: `char`</ul>|Base Type for signed variables.| 
|`u_base_t`|<ul><li>*ARM*: `unsigned int`<li>*AVR*: `unsigned char`</ul>|Base Type for unsigned variables.| 
|`stackpointer_t`| `unsigned int*` | Pointer to stack memory.|
|`taskpointer_t`| `void(*)(void)` | Function pointer to thread task.|