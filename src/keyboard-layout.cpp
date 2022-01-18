#include <wayfire/plugin.hpp>
#include <wayfire/signal-definitions.hpp>
#include <wayfire/util/log.hpp>
#include <xkbcommon/xkbcommon.h>
#include <wayfire/nonstd/wlroots-full.hpp>

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>


#define SERVER_PATH "/tmp/kbl_server"


class keyboard_layout_t : public wf::plugin_interface_t {

    int sd;
    struct sockaddr_un server_addr;
    xkb_layout_index_t current_layout;


    xkb_layout_index_t get_kb_layout () {
        wlr_seat* seat = wf::get_core().get_current_seat();
        wlr_keyboard* keyboard = wlr_seat_get_keyboard(seat);
        return xkb_state_serialize_layout(keyboard->xkb_state,
                                          XKB_STATE_LAYOUT_LOCKED);
    }

    wf::signal_callback_t on_key = [=] (wf::signal_data_t*) {
        xkb_layout_index_t layout = get_kb_layout();
        if (layout == current_layout)
            return;

        current_layout = layout;

        LOG(wf::log::LOG_LEVEL_DEBUG, "layout: ", layout);

        struct sockaddr_un client_addr;
        socklen_t size = sizeof(client_addr);
        while (get_request(&client_addr, &size)) 
            send_to(layout, &client_addr, size);
    };

    bool get_request (struct sockaddr_un* addr, socklen_t* size) {
        LOG(wf::log::LOG_LEVEL_DEBUG, "get_request");
        int ret = recvfrom(sd, NULL, 0, MSG_DONTWAIT, (struct sockaddr*) addr, size);
        LOG(wf::log::LOG_LEVEL_DEBUG, addr->sun_path);
        LOG(wf::log::LOG_LEVEL_DEBUG, ret >= 0);
        return ret >= 0;
    }

    void send_to (xkb_layout_index_t layout, struct sockaddr_un* addr, socklen_t size) {
        LOG(wf::log::LOG_LEVEL_DEBUG, "send_to");
        int ret = sendto(sd, &layout, sizeof(layout), 0, (struct sockaddr*)addr, size);
        if (ret < 0)
            LOG(wf::log::LOG_LEVEL_ERROR, "sendto");
    }

    void init_sock () {
        if ((sd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
            LOG(wf::log::LOG_LEVEL_ERROR, "socket error");
            close(sd);
            exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_PATH);

        unlink(SERVER_PATH);
        if (bind(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            LOG(wf::log::LOG_LEVEL_ERROR, "bind error");
            close(sd);
            exit(2);
        }
    }

public:

    void init () override {
        wf::get_core().connect_signal("keyboard_key", &on_key);
        init_sock();
        current_layout = get_kb_layout();
    }

    void fini() override {
        close(sd);
    }
};


DECLARE_WAYFIRE_PLUGIN(keyboard_layout_t)
