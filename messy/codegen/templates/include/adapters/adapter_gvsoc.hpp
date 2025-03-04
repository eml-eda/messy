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


// https://gvsoc.readthedocs.io/en/latest/tutorials.html#how-to-control-gvsoc-from-an-external-simulator
/**
 * @class AdapterGvsoc
 * @brief Adapter class for interfacing with GVSOC.
 * 
 * The AdapterGvsoc class provides methods for executing events, retrieving power consumption,
 * handling requests, and managing the simulation state. It inherits from gv::Io_user and gv::Gvsoc_user. More details are present in the GvSoC Documentation (https://gvsoc.readthedocs.io/en/latest/tutorials.html#how-to-control-gvsoc-from-an-external-simulator)
 */
class AdapterGvsoc : public gv::Io_user, public gv::Gvsoc_user {
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
     * @brief Handles access requests from GVSOC, and gets called when an access from gvsoc side is reaching us. As per gvsoc documentation (https://gvsoc.readthedocs.io/en/latest/tutorials.html#how-to-control-gvsoc-from-an-external-simulator) when controlling GvSoC from an external simulator, the external simulator must have a class, in this case the adapter, that inherits from gv::Io_user and overloads some methods that handles the access to requests.
     * 
     * @param req Pointer to the Io_request from GVSOC.
     */
    void access(gv::Io_request *req);
    
    /**
     * @brief Handles granted requests from GVSOC. It gets called when one of our access gets granted.
     * 
     * @param req Pointer to the Io_request that was granted.
     */
    void grant(gv::Io_request *req);
    
    /**
     * @brief Handles replies to requests from GVSOC. It gets called when one of our access gets a reply.
     * 
     * @param req Pointer to the Io_request that received a reply.
     */
    void reply(gv::Io_request *req);
    
    /**
     * Called by GVSOC to notify the simulation has ended. It sets the finished flag to true.
     *
     * This means the simulated software is over and probably exited and GVSOC cannnot further
     * simulate it.
     */
    void has_ended() override;

    /**
     * Called by GVSOC to notify the simulation has stopped.
     *
     * This means the an event occurs which stopped the simulation. Simulation can be still be
     * resumed.
     */
    void has_stopped() override;

    /**
     * Called by GVSOC to notify the simulation engine was updated.
     *
     * This means a new event was posted to the engine and modified the timestamp of the next
     * event to be executed.
     */
    void was_updated() override;

    /**
     * @brief Retrieves a MessyRequest from a GVSOC Io_request. It basically maps the GVSOC request to a MessyRequest.
     * 
     * @param req Pointer to the Io_request from GVSOC.
     * @return Pointer to the corresponding MessyRequest.
     */
    MessyRequest *get_messy_request_from_gvsoc(gv::Io_request *req);

    /**
     * @brief Constructor for AdapterGvsoc.
     * 
     * Initializes the AdapterGvsoc object.
     */
    AdapterGvsoc();

    //~AdapterGvsoc() {};
    bool finished = false; /**< Flag indicating whether the adapter has finished */

  private:
    int closed = 0; /**< Flag indicating whether the adapter is closed */
    gv::Io_binding *axi; /**< Pointer to the AXI binding, as per GvSoC Documentation */
    gv::GvsocLauncher *gvsoc; /**< Pointer to the GVSOC launcher */
    gv::PowerReport *power_report; /**< Pointer to the power report */
#ifdef LOG_REQUESTS
    std::ofstream request_addresses; /**< Output file stream for logging request addresses */
    std::ofstream request_sizes; /**< Output file stream for logging request sizes */
#endif
};

#endif