#include <n3rv/n3rvservice.hpp>
#include <n3rv/n3rvlogger.hpp>
#include <n3rv/n3rvservicecontroller.hpp>

class exec: public n3rv::service {
    using n3rv::service::service;

    public:

    int initialize() { 

        this->connect("broker1",ZMQ_SUB);
        this->attach("broker1",process_data);
        this->connect("exec1",ZMQ_REQ);

    }

    int send_order(std::string asset, int size, float stop, float limit) {
        
        n3rv::message msg;
        msg.action = "MARKET_ORDER";
        msg.args.emplace_back(asset);
        std::stringstream ss;
        ss << size;
        msg.args.emplace_back(ss.str());
        ss.str("");
        ss << stop;
        msg.args.emplace_back(ss.str());
        ss.str("");
        ss << limit;
        msg.args.emplace_back(ss.str());
        this->send("exec1",msg,0);

        return 0;


    }


    static void* process_data(void* objref, zmq::message_t* zmsg) {

        eval* self = (eval*) objref;

        //retrieves market datastream
        n3rv::message msg = n3rv::parse_msg(zmsg);

        if ( msg.action == "market_price" && msg.args[0] == "DOW_IA" ) {

            self->dow_price = atoi( msg.payload.c_str() );

            //processing of retrieved data
            self->ll->log(n3rv::LOGLV_NORM,"DOW_PRICE:" + msg.payload );

            if (self->dow_price != 0 && self->dow_price <= 20900 ) {
                self->ll->log(n3rv::LOGLV_NORM,"SELL SIGNAL!");
                self->send_order("DOW",-1, 21000, 20500);
            }

            else if (self->dow_price >= 21200) {
                self->ll->log(n3rv::LOGLV_NORM,"BUY SIGNAL!");
                self->send_order("DOW",1, 21000, 21500);
            }
        
            else if (self->dow_price != 0) {
                self->ll->log(n3rv::LOGLV_NORM,"NO SIGNAL!");
            }

        }

    }

};





int main() {


    eval e0("eval1", "evaluator", "127.0.0.1", 10001, 11002);

    e0.ll->set_loglevel(4);
    e0.ll->add_dest("stdout");

    e0.initialize();
    e0.subscribe();    
    e0.run();


}