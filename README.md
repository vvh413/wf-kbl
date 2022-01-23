# wf-kbl
Keyboard Layout Plugin for [Wayfire](https://github.com/WayfireWM/wayfire)

Based on simple IPC sockets. The plugin creates a socket `/tmp/kbl_server` and each client creates a socket `/tmp/kbl_client_<timestamp>`. Clients send a request to the server (plugin), and the server (plugin) sends the current keyboard layout when it changes.

## TODO
 * Automatic client reload on plugin reload
 * When the client starts print the current layout (currently it only prints on first key event after the client starts)

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
It will print the current keyboard layout when it is changed.

### Waybar module

Can be used as [Waybar](https://github.com/Alexays/Waybar) module

~~~json
"custom/wf-kbl": {
    "format": "{}",
    "exec": "wf-kbl"
}
~~~
