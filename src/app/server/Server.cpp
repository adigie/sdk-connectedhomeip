#include "lib/core/CHIPError.h"
#include <app/server/Server.h>
#include <app/server/ServerImpl.h>

namespace chip {

Server Server::sServer;

Server::Server()
{
    static ServerImpl sImpl{ *this };
    mImpl = &sImpl;
}

CHIP_ERROR Server::Init(const ServerInitParams & initParams)
{
    return mImpl->Init(initParams);
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

#if CHIP_CONFIG_ENABLE_ICD_SERVER
app::ICDManager & Server::GetICDManager()
{
    return mImpl->GetICDManager();
}
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

void ServerScheduleFactoryReset()
{
    chip::Server::GetInstance().ScheduleFactoryReset();
}

} // namespace chip
