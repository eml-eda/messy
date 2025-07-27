#ifndef __ADAPTER_GVSOC_H
#define __ADAPTER_GVSOC_H

#include <config.hpp>
#include <gv/gvsoc.hpp>
// vtable issues? how to build abstract class?
// #include <adapters/iss_adapter.hpp>
#include <cstring>
#include <messy_request.hpp>
#include <string.h>
#include <vp/itf/io.hpp>
#include <vp/launcher.hpp>
#define LOG_REQUESTS 1
#ifdef LOG_REQUESTS
#include <fstream>
#endif

/**
 * @class AdapterGvsocGap9
 * @brief Adapter class for interfacing with GVSOC for GAP9.
 * 
 * The AdapterGvsocGap9 class provides methods for executing events, retrieving power consumption,
 * handling requests, and managing the simulation state specifically for GAP9. It inherits from gv::Io_user and vp::Notifier.
 */
class AdapterGvsocGap9 : public gv::Io_user, public vp::Notifier {
  public:
    /**
     * @brief Executes events at the specified timestamp.
     * 
     * @param timestamp The timestamp at which to execute events.
     * @return The next timestamp at which events should be executed.
     */
    int64_t exec_events_at(int64_t timestamp);
    /**
     * @brief Retrieves the power consumption at the specified timestamp.
     * 
     * @param timestamp The timestamp at which to retrieve power consumption.
     * @return The power consumption at the specified timestamp.
     */
    double get_power_at(int64_t timestamp);
    /**
     * @brief Closes the adapter.
     * 
     * This function performs any necessary cleanup and closes the adapter.
     */
    void close();
    /**
     * @brief Starts up the adapter.
     * 
     * This function initializes and starts up the adapter.
     */
    void startup();
    /**
     * @brief Custom reply handler for MessyRequest.
     * 
     * @param req Pointer to the MessyRequest to be handled.
     */
    void custom_reply(MessyRequest *req);
    /**
     * @brief Handles access requests from GVSOC.
     * 
     * @param req Pointer to the Io_request from GVSOC.
     */
    void access(gv::Io_request *req);
    /**
     * @brief Handles granted requests from GVSOC.
     * 
     * @param req Pointer to the Io_request that was granted.
     */
    void grant(gv::Io_request *req);
    /**
     * @brief Handles replies to requests from GVSOC.
     * 
     * @param req Pointer to the Io_request that received a reply.
     */
    void reply(gv::Io_request *req);
    /**
     * @brief Notifies that the simulation has stopped.
     * 
     * @param time The time at which the simulation stopped.
     */
    void notify_stop(int64_t time);
    /**
     * @brief Notifies that the simulation is running.
     * 
     * @param time The time at which the simulation started running.
     */
    void notify_run(int64_t time);
    /**
     * @brief Retrieves a MessyRequest from a GVSOC Io_request.
     * 
     * @param req Pointer to the Io_request from GVSOC.
     * @return Pointer to the corresponding MessyRequest.
     */
    MessyRequest *get_messy_request_from_gvsoc(gv::Io_request *req);
    /**
     * @brief Constructor for AdapterGvsocGap9.
     * 
     * Initializes the AdapterGvsocGap9 object.
     */
    AdapterGvsocGap9();
    //~AdapterGvsocGap9() {};
    bool finished = false; /**< Flag indicating whether the adapter has finished */

  private:
    int closed = 0; /**< Flag indicating whether the adapter is closed */
    gv::Io_binding *axi; /**< Pointer to the AXI binding */
    Gvsoc_launcher *gvsoc; /**< Pointer to the GVSOC launcher */
#ifdef LOG_REQUESTS
    std::ofstream request_addresses; /**< Output file stream for logging request addresses */
    std::ofstream request_sizes; /**< Output file stream for logging request sizes */
#endif
};

#endif