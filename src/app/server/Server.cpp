#include "lib/core/CHIPError.h"
#include <app/server/Server.h>
#include <app/server/ServerImpl.h>

namespace chip {

Server::Server()
{
    static ServerImpl sImpl{ *this };
    mImpl = &sImpl;
}

CHIP_ERROR Server::Init(const ServerInitParams & initParams)
{
    return mImpl->Init(initParams);
}

void Server::RejoinExistingMulticastGroups()
{
    mImpl->RejoinExistingMulticastGroups();
}

FabricTable & Server::GetFabricTable()
{
    return mImpl->GetFabricTable();
}

CASESessionManager * Server::GetCASESessionManager()
{
    return mImpl->GetCASESessionManager();
}

Messaging::ExchangeManager & Server::GetExchangeManager()
{
    return mImpl->GetExchangeManager();
}

SessionManager & Server::GetSecureSessionManager()
{
    return mImpl->GetSecureSessionManager();
}

SessionResumptionStorage * Server::GetSessionResumptionStorage()
{
    return mImpl->GetSessionResumptionStorage();
}

app::SubscriptionResumptionStorage * Server::GetSubscriptionResumptionStorage()
{
    return mImpl->GetSubscriptionResumptionStorage();
}

TransportMgrBase & Server::GetTransportManager()
{
    return mImpl->GetTransportManager();
}

Credentials::GroupDataProvider * Server::GetGroupDataProvider()
{
    return mImpl->GetGroupDataProvider();
}

Crypto::SessionKeystore * Server::GetSessionKeystore() const
{
    return mImpl->GetSessionKeystore();
}

#if CONFIG_NETWORK_LAYER_BLE
Ble::BleLayer * Server::GetBleLayerObject()
{
    return mImpl->GetBleLayerObject();
}
#endif

CommissioningWindowManager & Server::GetCommissioningWindowManager()
{
    return mImpl->GetCommissioningWindowManager();
}

PersistentStorageDelegate & Server::GetPersistentStorage()
{

    return mImpl->GetPersistentStorage();
}

app::FailSafeContext & Server::GetFailSafeContext()
{

    return mImpl->GetFailSafeContext();
}

TestEventTriggerDelegate * Server::GetTestEventTriggerDelegate()
{

    return mImpl->GetTestEventTriggerDelegate();
}

Crypto::OperationalKeystore * Server::GetOperationalKeystore()
{

    return mImpl->GetOperationalKeystore();
}

Credentials::OperationalCertificateStore * Server::GetOpCertStore()
{

    return mImpl->GetOpCertStore();
}

app::DefaultAttributePersistenceProvider & Server::GetDefaultAttributePersister()
{

    return mImpl->GetDefaultAttributePersister();
}

app::reporting::ReportScheduler * Server::GetReportScheduler()
{

    return mImpl->GetReportScheduler();
}

#if CHIP_CONFIG_ENABLE_ICD_SERVER
app::ICDManager & Server::GetICDManager()
{
    return mImpl->GetICDManager();
}

#if CHIP_CONFIG_ENABLE_ICD_CIP
bool Server::ShouldCheckInMsgsBeSentAtBootFunction(FabricIndex aFabricIndex, NodeId subjectID)
{
    return mImpl->ShouldCheckInMsgsBeSentAtBootFunction(aFabricIndex, subjectID);
}
#endif // CHIP_CONFIG_ENABLE_ICD_CIP
#endif // CHIP_CONFIG_ENABLE_ICD_SERVER

void Server::GenerateShutDownEvent()
{
    mImpl->GenerateShutDownEvent();
}

void Server::Shutdown()
{
    mImpl->Shutdown();
}

void Server::ScheduleFactoryReset()
{
    mImpl->ScheduleFactoryReset();
}

System::Clock::Microseconds64 Server::TimeSinceInit() const
{
    return mImpl->TimeSinceInit();
}

void Server::InitFailSafe()
{
    mImpl->InitFailSafe();
}

void Server::OnPlatformEvent(const DeviceLayer::ChipDeviceEvent & event)
{
    mImpl->OnPlatformEvent(event);
}
void Server::CheckServerReadyEvent()
{
    mImpl->CheckServerReadyEvent();
}

// static void Server::OnPlatformEventWrapper(const DeviceLayer::ChipDeviceEvent * event, intptr_t);

#if CHIP_CONFIG_PERSIST_SUBSCRIPTIONS
void Server::ResumeSubscriptions()
{
    mImpl->ResumeSubscriptions();
}
#endif

Server Server::sServer;

void ServerScheduleFactoryReset()
{
    chip::Server::GetInstance().ScheduleFactoryReset();
}

} // namespace chip
