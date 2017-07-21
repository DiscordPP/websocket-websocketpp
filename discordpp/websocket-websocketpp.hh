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
        void init(aios_ptr asio_ios, const std::string &token, const unsigned int apiVersion, const std::string &gateway, DispatchHandler disHandler){
            keepalive_timer_ = std::make_shared<asio::steady_timer>(*asio_ios);

            client_.set_access_channels(websocketpp::log::alevel::all);
            client_.clear_access_channels(websocketpp::log::alevel::frame_payload);

            client_.set_tls_init_handler([this](websocketpp::connection_hdl){
                return websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv1);
            });

            client_.init_asio(asio_ios.get());

            client_.set_message_handler([this, asio_ios, token, apiVersion, disHandler](websocketpp::connection_hdl hdl, message_ptr msg){
                json jmessage = json::parse(msg->get_payload());
                handleMessage(asio_ios, token, apiVersion, disHandler, jmessage);
                /*if(!toSend.empty()){
                    for(json msg : toSend) {
                        client_.send(hdl, msg.dump(), websocketpp::frame::opcode::text);
                    }
                }*/
            });
            client_.set_open_handler([this](websocketpp::connection_hdl hdl){std::cout << "Connection established.\n";});
            //client_.set_close_handler([this, token](websocketpp::connection_hdl hdl){on_close(hdl, token);})

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

        void send(int opcode = 0, json payload = {}){
            client_.send(connection_, json({{"op", opcode}, {"d", payload}}).dump(), websocketpp::frame::opcode::TEXT);
        }
    private:
        //void on_open(websocketpp::connection_hdl hdl, std::string token){
        //    std::cout << "Connection established.\n";
        //}

        //void on_close(websocketpp::connection_hdl hdl, std::string token){
        //    hdl.
        //}

        /*void on_message(websocketpp::connection_hdl hdl, std::string token, websocketpp::connection_hdl hdl, message_ptr msg) {
            json jmessage = json::parse(msg->get_payload());
            if (jmessage["op"].get<int>() == 0) { //Dispatch
                std::map<std::string, Handler>::iterator it = handlers_.find(jmessage["t"]);
                if (it != handlers_.end()) {
                    asio_ios->post(std::bind(it->second, this, asio_ios, jmessage));
                } else {
                    std::cout << "There is no function for the event " << jmessage["t"] << ".\n";
                }
                if (jmessage["t"] == "READY") {

                }
            } else if (jmessage["op"].get<int>() == 1) {
                //Heartbeat (This isn't implemented yet, still using periodic heartbeats for now.)
                //client_.send(hdl, jmessage.dump(), websocketpp::frame::opcode::text);
            } else if (jmessage["op"].get<int>() == 10) {
                json connect =
                std::cout << "Client Handshake:\n" << connect.dump(1) << "\n";

                client_.send(hdl, connect.dump(), websocketpp::frame::opcode::text);
                uint32_t ms = jmessage["d"]["heartbeat_interval"];
                keepalive(ms);
            } else { //Wat
                std::cout << "Unexpected opcode received:\n\n" << jmessage.dump(4) << "\n\n\n";
            }
        }*/

        void sendkeepalive(json message){
            client_.send(connection_, message.dump(), websocketpp::frame::opcode::text);
        }

        client client_;
        websocketpp::uri_ptr uri_ptr_;
        client::connection_ptr connection_;
    };
}

#endif //EXAMPLE_BOT_WEBSOCKET_WEBSOCKETPP_HH
