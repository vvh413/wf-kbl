#include <wayfire/core.hpp>
#include <wayfire/option-wrapper.hpp>
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
    xkb_layout_index_t current_layout = -1;
    wf::option_wrapper_t<std::string> xkb_layout {"input/xkb_layout"};

    xkb_layout_index_t get_kb_layout () {
        wlr_seat* seat = wf::get_core().get_current_seat();
        wlr_keyboard* keyboard = wlr_seat_get_keyboard(seat);
        return xkb_state_serialize_layout(keyboard->xkb_state,
                                          XKB_STATE_LAYOUT_LOCKED);
    }

    void index2lang (xkb_layout_index_t layout, char* lang) {
        strcpy(lang, xkb_layout.value().substr(layout * 3, 2).c_str());
    }

    wf::signal_callback_t on_key = [=] (wf::signal_data_t*) {
        xkb_layout_index_t layout = get_kb_layout();
        if (layout == current_layout)
            return;

        current_layout = layout;
        LOG(wf::log::LOG_LEVEL_DEBUG, "layout: ", layout);
        
        char lang[2];
        index2lang(layout, lang);
        LOG(wf::log::LOG_LEVEL_DEBUG, "lang: ", lang);

        send2all(lang, sizeof(lang));
    };

    bool get_request (struct sockaddr_un* addr, socklen_t* socklen) {
        LOG(wf::log::LOG_LEVEL_DEBUG, "get_request");
        int ret = recvfrom(sd, NULL, 0, MSG_DONTWAIT, (struct sockaddr*) addr, socklen);
        LOG(wf::log::LOG_LEVEL_DEBUG, addr->sun_path);
        LOG(wf::log::LOG_LEVEL_DEBUG, ret >= 0);
        return ret >= 0;
    }

    void send_to (void* data, size_t size, struct sockaddr_un* addr, socklen_t socklen) {
        LOG(wf::log::LOG_LEVEL_DEBUG, "send_to");
        int ret = sendto(sd, data, size, 0, (struct sockaddr*)addr, socklen);
        if (ret < 0)
            LOG(wf::log::LOG_LEVEL_ERROR, "sendto");
    }

    void send2all (void* data, size_t size) {
        struct sockaddr_un client_addr;
        memset(&client_addr, 0, sizeof(client_addr));
        socklen_t socklen = sizeof(client_addr);
        while (get_request(&client_addr, &socklen)) 
            send_to(data, size, &client_addr, socklen);
    }

    int init_sock () {
        if ((sd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
            LOG(wf::log::LOG_LEVEL_ERROR, "socket error");
            close(sd);
            return 1;
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, SERVER_PATH);

        unlink(SERVER_PATH);
        if (bind(sd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
            LOG(wf::log::LOG_LEVEL_ERROR, "bind error");
            close(sd);
            return 2;
        }
        return 0;
    }

public:

    void init () override {
        if (init_sock()) return;
        wf::get_core().connect_signal("keyboard_key", &on_key);
    }

    void fini() override {
        wf::get_core().disconnect_signal("keyboard_key", &on_key);
        char end[] = "--";
        send2all(end, sizeof(end));
        close(sd);
    }
};


DECLARE_WAYFIRE_PLUGIN(keyboard_layout_t)
