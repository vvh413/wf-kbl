# wf-kbl
Keyboard Layout Plugin for Wayfire

## TODO
 * Get layout list from XKBlayout from Wayfire config 
 * Print layout on layout change

## Build

~~~bash
meson build
ninja -C build && sudo ninja -C build install
~~~

## Usage

Enable plugin and run `wf-kbl` command
~~~bash
wf-kbl
~~~
It will print the current keyboard layout when pressing or releasing the spacebar.

### Waybar module

Can be used as Waybar module

~~~json
"custom/wf-kbl": {
    "format": "{}",
    "exec": "wf-kbl"
}
~~~