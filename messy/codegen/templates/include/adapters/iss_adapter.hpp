#ifndef __ISS_ADAPTER_H
#define __ISS_ADAPTER_H

#include <messy_request.hpp>
#include <stdint.h>

/**
 * @class ISS_Adapter
 * @brief Abstract base class for an ISS (Instruction Set Simulator) adapter.
 * 
 * The ISS_Adapter class provides an interface for interacting with an ISS. It includes
 * methods for executing events, retrieving power consumption, handling requests, and
 * managing the simulation state.
 */
class ISS_Adapter {
  public:
    /**
     * @brief Executes events at the specified timestamp.
     * 
     * This function advances the ISS simulation to the specified timestamp.
     * 
     * @param timestamp The timestamp at which to execute events.
     * @return The next timestamp at which events should be executed.
     */
    virtual int64_t exec_events_at(int64_t timestamp);

    /**
     * @brief Closes the ISS adapter.
     * 
     * This function performs any necessary cleanup and closes the ISS adapter.
     */
    virtual void close();

    /**
     * @brief Destructor for ISS_Adapter.
     * 
     * This virtual destructor ensures that derived classes can clean up resources properly.
     */
    virtual ~ISS_Adapter() {};

    /**
     * @brief Starts up the ISS adapter.
     * 
     * This function initializes and starts up the ISS adapter.
     */
    virtual void startup();

    /**
     * @brief Custom reply handler for MessyRequest.
     * 
     * This function handles the reply for a MessyRequest.
     * 
     * @param req Pointer to the MessyRequest to be handled.
     */
    virtual void custom_reply(MessyRequest *req);

    /**
     * @brief Retrieves the power consumption at the specified timestamp.
     * 
     * This function retrieves the power consumption from the ISS simulation at the specified timestamp.
     * 
     * @param timestamp The timestamp at which to retrieve power consumption.
     * @return The power consumption at the specified timestamp.
     */
    virtual double get_power_at(int64_t timestamp);

    bool finished = false; /**< Flag indicating whether the ISS simulation has finished */
};

#endif