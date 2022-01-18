kbl = shared_module('keyboard-layout', 'keyboard-layout.cpp',
    dependencies: [wayfire, wlroots],
    install: true, install_dir: wayfire.get_variable(pkgconfig: 'plugindir'))

kblc = executable('wf-kbl', 'wf-kbl.cpp',
    install: true)
