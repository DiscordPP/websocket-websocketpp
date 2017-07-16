//
// Created by aidan on 7/15/17.
//

#ifndef EXAMPLE_BOT_WEBSOCKET_WEBSOCKETPP_HH
#define EXAMPLE_BOT_WEBSOCKET_WEBSOCKETPP_HH

#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>

#include <discordpp/websocketmodule.hh>

namespace discordpp {
    namespace asio = boost::asio;
    using json = nlohmann::json;

    class WebsocketWebsocketPPModule : public WebsocketModule {
        using client = websocketpp::client<websocketpp::config::asio_tls_client>;
        using message_ptr = websocketpp::config::asio_client::message_type::ptr;

    public:
        void init(std::shared_ptr<asio::io_service> asio_ios, const std::string &token, const std::string &gateway, std::function<void(json)> message_handler){
            keepalive_timer_ = std::make_shared<asio::steady_timer>(*asio_ios);

            client_.set_access_channels(websocketpp::log::alevel::all);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            client_.set_tls_init_handler([this](websocketpp::connection_hdl){
                return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
            });

            client_.init_asio(asio_ios.get());

            client_.set_message_handler([message_handler, asio_ios](websocketpp::connection_hdl hdl, message_ptr msg){
                json jmessage = json::parse(msg->get_payload());
                message_handler(jmessage);
            });
            client_.set_open_handler([this, token](websocketpp::connection_hdl hdl){on_open(hdl, token);});

            websocketpp::lib::error_code ec;
            std::cout << "Connecting to gateway at " << gateway << "\n";
            connection_ = client_.get_connection(gateway, ec);
            if (ec) {
                std::cout << "could not create connection because: " << ec.message() << std::endl;
                //TODO TBD: throw something
            } else {
                // Note that connect here only requests a connection. No network messages are
                // exchanged until the event loop starts running in the next line.
                client_.connect(connection_);
            }
        }
    private:
        void on_open(websocketpp::connection_hdl hdl, std::string token){
            std::cout << "Connection established.\n";

            json connect = {
                    {"op", 2},
                    {"d", {
                                   {"token", token},
                                   {"v", 4},
                                   {"properties", {
                                                          {"$os", "linux"},
                                                          {"$browser", "discordpp"},
                                                          {"$device", "discordpp"},
                                                          {"$referrer",""}, {"$referring_domain",""}
                                                  }
                                   },
                                   {"compress", false},
                                   {"large_threshold", 250}
                           }
                    }
            };
            std::cout << "Client Handshake:\n" << connect.dump(1) << "\n";
            client_.send(hdl, connect.dump(), websocketpp::frame::opcode::text);
        }

        void sendkeepalive(json message){
            client_.send(connection_, message.dump(), websocketpp::frame::opcode::text);
        }

        client client_;
        websocketpp::uri_ptr uri_ptr_;
        client::connection_ptr connection_;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKET_WEBSOCKETPP_HH
