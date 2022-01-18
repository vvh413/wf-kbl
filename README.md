# wf-kbl
Keyboard Layout Plugin for Wayfire

Based on simple IPC sockets. The plugin creates a socket `/tmp/kbl_server` and each client creates a socket `/tmp/kbl_client_<timestamp>`. Clients send a request to the server (plugin), and the server (plugin) sends the current keyboard layout to each client each time the space bar is pressed or released.

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
It will print the current keyboard layout when pressing or releasing the space bar.

### Waybar module

Can be used as Waybar module

~~~json
"custom/wf-kbl": {
    "format": "{}",
    "exec": "wf-kbl"
}
~~~
