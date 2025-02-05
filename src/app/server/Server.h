#pragma once
#include <system/SystemClock.h>
#include <transport/raw/PeerAddress.h>

namespace chip {

namespace Messaging {
class ExchangeManager;
}

namespace app {
class FailSafeContext;
#if CHIP_CONFIG_ENABLE_ICD_SERVER
class ICDManager;
#endif
} // namespace app

namespace Crypto {
class SessionKeystore;
} // namespace Crypto

#if CONFIG_NETWORK_LAYER_BLE
namespace Ble {
class BleLayer;
}
#endif

class ServerInitParams;
class FabricTable;
class CASESessionManager;
class SessionManager;
class SessionResumptionStorage;
class CommissioningWindowManager;
class PersistentStorageDelegate;
class TestEventTriggerDelegate;

class ServerImpl;

class Server
{
public:
    CHIP_ERROR Init(const ServerInitParams & initParams);

#if CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY_CLIENT
    CHIP_ERROR
    SendUserDirectedCommissioningRequest(chip::Transport::PeerAddress commissioner,
                                         Protocols::UserDirectedCommissioning::IdentificationDeclaration & id);

    Protocols::UserDirectedCommissioning::UserDirectedCommissioningClient * GetUserDirectedCommissioningClient();
#endif // CHIP_DEVICE_CONFIG_ENABLE_COMMISSIONER_DISCOVERY_CLIENT

    FabricTable & GetFabricTable();
    CASESessionManager * GetCASESessionManager();
    Messaging::ExchangeManager & GetExchangeManager();
    SessionManager & GetSecureSessionManager();
    SessionResumptionStorage * GetSessionResumptionStorage();
    Crypto::SessionKeystore * GetSessionKeystore() const;

#if CONFIG_NETWORK_LAYER_BLE
    Ble::BleLayer * GetBleLayerObject();
#endif

    CommissioningWindowManager & GetCommissioningWindowManager();
    PersistentStorageDelegate & GetPersistentStorage();
    app::FailSafeContext & GetFailSafeContext();
    TestEventTriggerDelegate * GetTestEventTriggerDelegate();

#if CHIP_CONFIG_ENABLE_ICD_SERVER
    app::ICDManager & GetICDManager();
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER

    /**
     * This function causes the ShutDown event to be generated async on the
     * Matter event loop.  Should be called before stopping the event loop.
     */
    void GenerateShutDownEvent();

    void Shutdown();

    void ScheduleFactoryReset();

    System::Clock::Microseconds64 TimeSinceInit() const;

    static Server & GetInstance() { return sServer; }

private:
    Server();

    static Server sServer;

    ServerImpl * mImpl;
};

void ServerScheduleFactoryReset();

} // namespace chip
