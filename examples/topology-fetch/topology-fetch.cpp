/** This example shows how to build generic service classes, 
 * and how each instance of a generic class can fetch its own topology from the service controller. */

#include <n3rv/n3rvservicecontroller.hpp>
#include <n3rv/n3rvservice.hpp>
#include <n3rv/n3rvtopology.hpp>

class genericservice: public n3rv::service {
    using n3rv::service::service;

    public:

    bool plock;

    std::map<std::string, n3rv::qhandler*> hdlist;


    int initialize() {      

       this->register_rcb("ping_received", ping_received);
       this->register_rcb("pong_received", pong_received);
          
       this->plock = false;

    }

 

    void hkloop() {
        if (this->service_class == "ping") {

            if (n3rv::blookup(this->directory, "com.pong.*.pong") && ! this->plock) {
                 n3rv::message ping_msg;
                 ping_msg.action = "PING";
                 this->ll->log(n3rv::LOGLV_NOTICE, "Sending Ping..");
                 this->send(this->hdlist["pong_conn"],ping_msg,0);
                 this->plock = true;
                 sleep(1);

            }         
        }
    }


    static void* pong_received(void* objref, zmq::message_t* zmsg) {
        genericservice* self = (genericservice*) objref;
        self->plock = false;
    }


    static void* ping_received(void* objref, zmq::message_t* zmsg) {

        genericservice* self = (genericservice*) objref;

        self->ll->log(n3rv::LOGLV_NOTICE, "Received ping, sending back\"pong\"");
        n3rv::message pong_msg;
        pong_msg.action = "PONG";
        self->send(self->hdlist["pong"], pong_msg,0);
    }
};



int main(int argc, char* argv[]) {

    n3rv::logger* ll = new n3rv::logger(n3rv::LOGLV_DEBUG);
    ll->add_dest("stdout");

    std::string sclass = argv[1];

    if (sclass == "ping" || sclass == "pong") {
         genericservice sc("127.0.0.1", 10001,ll);
         sc.set_uid(("com." + sclass + "." + sclass + "1").c_str());
         sc.initialize();

         //We fetch the topology data from service controller to 
         // determine how the services will behave.
         sc.hdlist = sc.fetch_topology();
         sc.run();
    }

    else if (sclass == "ctl") {
          std::cout << "STARTING CTRL!!" << std::endl;
          n3rv::servicecontroller sc1("0.0.0.0",10001,ll);
          
          //We load a topology file in the service controller.
          sc1.load_topology("topology.json");
          sc1.run();
    }

}