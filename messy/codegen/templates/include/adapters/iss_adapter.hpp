class ISS_Adapter {
    public:
    virtual int64_t exec_event_at(int64_t timestamp);
    virtual void close();
    virtual ~ISS_Adapter() {};
}