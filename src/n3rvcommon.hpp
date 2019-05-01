#ifndef N3RV_COMMON
#define N3RV_COMMON

#include <vector>
#include <zmq.hpp>
#include <regex>


namespace n3rv {

     /** Connction handler returned by connect() or bind() 
     *  used for send/recv operations. 
     */
    typedef struct qhandler_ {
        std::string cid;
        std::string peer_uid;
    } qhandler;

    /** stores deffered connections for later use, 
     *  when service becomes available in directory.
     */
    typedef struct qdef_ {
        std::string name;
        int socket_type;
        qhandler* hdl;
    } qdef;

    /** qconn aims to store information about 
     *  connections and manages zmq sockets. */
     
    typedef struct qconn_ {
      int type;
      zmq::socket_t* socket;
      int socket_type;
      std::vector<std::string> peers;
    } qconn;

     /**
     * binding structure aims to store configuration for a zmq socket.
     * (socket_type is a direct allusion to zmq socket types.)
     */
    typedef struct binding_  {
        void* parent;
        std::string name;
        unsigned int port;
        unsigned int socket_type;
    } binding;

    /**
     * qserv aims to store each services directory entries.
     */
    typedef struct qserv_ {
        std::string namespace_;
        std::string service_class;
        std::string node_name;
        std::string ip;
        std::vector<binding> bindings;
    } qserv;

    // Main n3rv callbacks signature.
    typedef void* (*fctptr)(void*, zmq::message_t*);


    /**
     * Generates a random string of length "length"
     */
    std::string randstr(size_t length);

    /**
     * Tries to Resolve node from directory provided as argument. 
     */
    qserv* nlookup(std::vector<qserv_>& dir, std::string service_class, std::string node_name, std::string namespace_ = "*" );
    
    /**
     * Tries to Resolve node from directory provided as argument, given full node addr.
     */
    qserv* nlookup(std::vector<qserv_>& dir, std::string addr);

    /** 
     * Tries to resolve binding from full address.
     */
    binding* blookup(std::vector<qserv>& dir, std::string addr);


}

#endif