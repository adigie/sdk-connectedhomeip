#pragma once
#include <system/SystemClock.h>
#include <transport/raw/PeerAddress.h>

namespace chip {

namespace Messaging {
class ExchangeManager;
}

namespace app {
class SubscriptionResumptionStorage;
class FailSafeContext;
class DefaultAttributePersistenceProvider;
class ICDManager;

namespace reporting {
class ReportScheduler;
}
} // namespace app

namespace Credentials {
class GroupDataProvider;
class OperationalCertificateStore;
} // namespace Credentials

namespace Crypto {
class SessionKeystore;
class OperationalKeystore;
} // namespace Crypto

namespace Ble {
class BleLayer;
}

class ServerInitParams;
class FabricTable;
class CASESessionManager;
class SessionManager;
class SessionResumptionStorage;
class TransportMgrBase;
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

    /**
     * @brief Call this function to rejoin existing groups found in the GroupDataProvider
     */
    void RejoinExistingMulticastGroups();

    FabricTable & GetFabricTable();
    CASESessionManager * GetCASESessionManager();
    Messaging::ExchangeManager & GetExchangeManager();
    SessionManager & GetSecureSessionManager();
    SessionResumptionStorage * GetSessionResumptionStorage();
    app::SubscriptionResumptionStorage * GetSubscriptionResumptionStorage();
    TransportMgrBase & GetTransportManager();
    Credentials::GroupDataProvider * GetGroupDataProvider();
    Crypto::SessionKeystore * GetSessionKeystore() const;

#if CONFIG_NETWORK_LAYER_BLE
    Ble::BleLayer * GetBleLayerObject();
#endif

    CommissioningWindowManager & GetCommissioningWindowManager();
    PersistentStorageDelegate & GetPersistentStorage();
    app::FailSafeContext & GetFailSafeContext();
    TestEventTriggerDelegate * GetTestEventTriggerDelegate();
    Crypto::OperationalKeystore * GetOperationalKeystore();
    Credentials::OperationalCertificateStore * GetOpCertStore();
    app::DefaultAttributePersistenceProvider & GetDefaultAttributePersister();
    app::reporting::ReportScheduler * GetReportScheduler();

#if CHIP_CONFIG_ENABLE_ICD_SERVER
    app::ICDManager & GetICDManager();

#if CHIP_CONFIG_ENABLE_ICD_CIP
    /**
     * @brief Function to determine if a Check-In message would be sent at Boot up
     *
     * @param aFabricIndex client fabric index
     * @param subjectID client subject ID
     * @return true Check-In message would be sent on boot up.
     * @return false Device has a persisted subscription with the client. See CHIP_CONFIG_PERSIST_SUBSCRIPTIONS.
     */
    bool ShouldCheckInMsgsBeSentAtBootFunction(FabricIndex aFabricIndex, NodeId subjectID);
#endif // CHIP_CONFIG_ENABLE_ICD_CIP
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

    void InitFailSafe();
    void OnPlatformEvent(const DeviceLayer::ChipDeviceEvent & event);
    void CheckServerReadyEvent();

    // static void OnPlatformEventWrapper(const DeviceLayer::ChipDeviceEvent * event, intptr_t);

#if CHIP_CONFIG_PERSIST_SUBSCRIPTIONS
    /**
     * @brief Called at Server::Init time to resume persisted subscriptions if the feature flag is enabled
     */
    void ResumeSubscriptions();
#endif
};

void ServerScheduleFactoryReset();

} // namespace chip
