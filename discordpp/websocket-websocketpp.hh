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
        WebsocketWebsocketPPModule(std::shared_ptr<asio::io_service> asio_ios, const std::string &token):
            WebsocketModule(asio_ios, token){}

        void init(const unsigned int apiVersion, const std::string &gateway, DispatchHandler disHandler){
            gateway_ = gateway;

            keepalive_timer_ = std::make_unique<asio::steady_timer>(*aios_);

            client_.set_access_channels(websocketpp::log::alevel::all);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            client_.set_tls_init_handler([this](websocketpp::connection_hdl){
                return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
            });

            client_.init_asio(aios_.get());

            client_.set_message_handler([this, apiVersion, disHandler](websocketpp::connection_hdl hdl, message_ptr msg){
                json jmessage = json::parse(msg->get_payload());
                handleMessage(apiVersion, disHandler, jmessage);
                /*if(!toSend.empty()){
                    for(json msg : toSend) {
                        client_.send(hdl, msg.dump(), websocketpp::frame::opcode::text);
                    }
                }*/
            });
            client_.set_open_handler ([this](websocketpp::connection_hdl hdl){
                std::cout << "Connection established.\n";
            });
            client_.set_close_handler([this](websocketpp::connection_hdl hdl){
                std::cout << "Connection lost.\n" << std::endl;
                closeHandler();
            });
            client_.set_fail_handler([this](websocketpp::connection_hdl hdl){
                connect_timer_ = std::make_unique<asio::deadline_timer>(*aios_, boost::posix_time::seconds(5));
                connect_timer_->async_wait([this](const boost::system::error_code&){connect();});
            });
            //client_.set_close_handler([this, token](websocketpp::connection_hdl hdl){on_close(hdl, token);})

            connect();
        }

        void send(int opcode = 0, json payload = {}){
            client_.send(connection_, json({{"op", opcode}, {"d", payload}}).dump(), websocketpp::frame::opcode::TEXT);
        }

        void close(){
            keepalive_timer_->cancel();
            hold_asio_ = std::make_unique<asio::io_service::work>(*aios_);
            //client_.stop();
            client_.reset();
            //client_.close(connection_, 4000, "No heartbeat ack");
        }

    protected:
        void connect(){
            try {
                websocketpp::lib::error_code ec;
                std::cout << "Connecting to gateway at " << gateway_ << "\n";
                client::connection_ptr connection = client_.get_connection(gateway_, ec);
                if (ec) {
                    std::cout << "could not create connection because: " << ec.message() << std::endl;
                    //TODO TBD: throw something
                } else {
                    // Note that connect here only requests a connection. No network messages are
                    // exchanged until the event loop starts running in the next line.
                    client_.connect(connection);
                    connection_ = connection;
                }
            }catch(websocketpp::exception){
                std::cout << "Connection failed" << "\n";
            }
        }

        void sendkeepalive(json message){
            client_.send(connection_, message.dump(), websocketpp::frame::opcode::text);
        }
    private:

        client client_;
        websocketpp::uri_ptr uri_ptr_;
        client::connection_ptr connection_;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKET_WEBSOCKETPP_HH
