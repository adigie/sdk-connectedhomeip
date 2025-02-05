#pragma once

#include <access/AccessControl.h>
#include <access/examples/ExampleAccessControlDelegate.h>
#include <app/TestEventTriggerDelegate.h>
#include <app/TimerDelegates.h>
#include <app/reporting/ReportSchedulerImpl.h>
#include <app/server/AclStorage.h>
#include <app/server/AppDelegate.h>
#include <app/server/DefaultAclStorage.h>
#include <credentials/GroupDataProviderImpl.h>
#include <credentials/PersistentStorageOpCertStore.h>
#include <crypto/DefaultSessionKeystore.h>
#include <crypto/PersistentStorageOperationalKeystore.h>
#include <platform/KvsPersistentStorageDelegate.h>
#include <protocols/secure_channel/SimpleSessionResumptionStorage.h>

namespace chip {

struct ServerInitParams
{
    ServerInitParams() = default;

    // Not copyable
    ServerInitParams(const ServerInitParams &)             = delete;
    ServerInitParams & operator=(const ServerInitParams &) = delete;

    // Application delegate to handle some commissioning lifecycle events
    AppDelegate * appDelegate = nullptr;
    // Port to use for Matter commissioning/operational traffic
    uint16_t operationalServicePort = CHIP_PORT;
    // Port to use for UDC if supported
    uint16_t userDirectedCommissioningPort = CHIP_UDC_PORT;
    // Interface on which to run daemon
    Inet::InterfaceId interfaceId = Inet::InterfaceId::Null();

    // Persistent storage delegate: MUST be injected. Used to maintain storage by much common code.
    // Must be initialized before being provided.
    PersistentStorageDelegate * persistentStorageDelegate = nullptr;
    // Session resumption storage: Optional. Support session resumption when provided.
    // Must be initialized before being provided.
    SessionResumptionStorage * sessionResumptionStorage = nullptr;
    // Session resumption storage: Optional. Support session resumption when provided.
    // Must be initialized before being provided.
    app::SubscriptionResumptionStorage * subscriptionResumptionStorage = nullptr;
    // Certificate validity policy: Optional. If none is injected, CHIPCert
    // enforces a default policy.
    Credentials::CertificateValidityPolicy * certificateValidityPolicy = nullptr;
    // Group data provider: MUST be injected. Used to maintain critical keys such as the Identity
    // Protection Key (IPK) for CASE. Must be initialized before being provided.
    Credentials::GroupDataProvider * groupDataProvider = nullptr;
    // Session keystore: MUST be injected. Used to derive and manage lifecycle of symmetric keys.
    Crypto::SessionKeystore * sessionKeystore = nullptr;
    // Access control delegate: MUST be injected. Used to look up access control rules. Must be
    // initialized before being provided.
    Access::AccessControl::Delegate * accessDelegate = nullptr;
    // ACL storage: MUST be injected. Used to store ACL entries in persistent storage. Must NOT
    // be initialized before being provided.
    app::AclStorage * aclStorage = nullptr;

#if CHIP_CONFIG_USE_ACCESS_RESTRICTIONS
    // Access Restriction implementation: MUST be injected if MNGD feature enabled. Used to enforce
    // access restrictions that are managed by the device.
    Access::AccessRestrictionProvider * accessRestrictionProvider = nullptr;
#endif

    // Network native params can be injected depending on the
    // selected Endpoint implementation
    void * endpointNativeParams = nullptr;
    // Optional. Support test event triggers when provided. Must be initialized before being
    // provided.
    TestEventTriggerDelegate * testEventTriggerDelegate = nullptr;
    // Operational keystore with access to the operational keys: MUST be injected.
    Crypto::OperationalKeystore * operationalKeystore = nullptr;
    // Operational certificate store with access to the operational certs in persisted storage:
    // must not be null at timne of Server::Init().
    Credentials::OperationalCertificateStore * opCertStore = nullptr;
    // Required, if not provided, the Server::Init() WILL fail.
    app::reporting::ReportScheduler * reportScheduler = nullptr;
#if CHIP_CONFIG_ENABLE_ICD_CIP
    // Optional. Support for the ICD Check-In BackOff strategy. Must be initialized before being provided.
    // If the ICD Check-In protocol use-case is supported and no strategy is provided, server will use the default strategy.
    app::ICDCheckInBackOffStrategy * icdCheckInBackOffStrategy = nullptr;
#endif // CHIP_CONFIG_ENABLE_ICD_CIP
};

/**
 * Transitional version of ServerInitParams to assist SDK integrators in
 * transitioning to injecting product/platform-owned resources. This version
 * of `ServerInitParams` statically owns and initializes (via the
 * `InitializeStaticResourcesBeforeServerInit()` method) the persistent storage
 * delegate, the group data provider, and the access control delegate. This is to reduce
 * the amount of copied boilerplate in all the example initializations (e.g. AppTask.cpp,
 * main.cpp).
 *
 * This version SHOULD BE USED ONLY FOR THE IN-TREE EXAMPLES.
 *
 * ACTION ITEMS FOR TRANSITION from a example in-tree to a product:
 *
 * While this could be used indefinitely, it does not exemplify orderly management of
 * application-injected resources. It is recommended for actual products to instead:
 *   - Use the basic ServerInitParams in the application
 *   - Have the application own an instance of the resources being injected in its own
 *     state (e.g. an implementation of PersistentStorageDelegate and GroupDataProvider
 *     interfaces).
 *   - Initialize the injected resources prior to calling Server::Init()
 *   - De-initialize the injected resources after calling Server::Shutdown()
 *
 * WARNING: DO NOT replicate the pattern shown here of having a subclass of ServerInitParams
 *          own the resources outside of examples. This was done to reduce the amount of change
 *          to existing examples while still supporting non-example versions of the
 *          resources to be injected.
 */
struct CommonCaseDeviceServerInitParams : public ServerInitParams
{
    CommonCaseDeviceServerInitParams() = default;

    // Not copyable
    CommonCaseDeviceServerInitParams(const CommonCaseDeviceServerInitParams &)             = delete;
    CommonCaseDeviceServerInitParams & operator=(const CommonCaseDeviceServerInitParams &) = delete;

    /**
     * Call this before Server::Init() to initialize the internally-owned resources.
     * Server::Init() will fail if this is not done, since several params required to
     * be non-null will be null without calling this method. ** See the transition method
     * in the outer comment of this class **.
     *
     * @return CHIP_NO_ERROR on success or a CHIP_ERROR value from APIs called to initialize
     *         resources on failure.
     */
    CHIP_ERROR InitializeStaticResourcesBeforeServerInit()
    {
        // KVS-based persistent storage delegate injection
        if (persistentStorageDelegate == nullptr)
        {
            chip::DeviceLayer::PersistedStorage::KeyValueStoreManager & kvsManager =
                DeviceLayer::PersistedStorage::KeyValueStoreMgr();
            ReturnErrorOnFailure(sKvsPersistenStorageDelegate.Init(&kvsManager));
            this->persistentStorageDelegate = &sKvsPersistenStorageDelegate;
        }

        // PersistentStorageDelegate "software-based" operational key access injection
        if (this->operationalKeystore == nullptr)
        {
            // WARNING: PersistentStorageOperationalKeystore::Finish() is never called. It's fine for
            //          for examples and for now.
            ReturnErrorOnFailure(sPersistentStorageOperationalKeystore.Init(this->persistentStorageDelegate));
            this->operationalKeystore = &sPersistentStorageOperationalKeystore;
        }

        // OpCertStore can be injected but default to persistent storage default
        // for simplicity of the examples.
        if (this->opCertStore == nullptr)
        {
            // WARNING: PersistentStorageOpCertStore::Finish() is never called. It's fine for
            //          for examples and for now, since all storage is immediate for that impl.
            ReturnErrorOnFailure(sPersistentStorageOpCertStore.Init(this->persistentStorageDelegate));
            this->opCertStore = &sPersistentStorageOpCertStore;
        }

        // Injection of report scheduler WILL lead to two schedulers being allocated. As recommended above, this should only be used
        // for IN-TREE examples. If a default scheduler is desired, the basic ServerInitParams should be used by the application and
        // CommonCaseDeviceServerInitParams should not be allocated.
        if (this->reportScheduler == nullptr)
        {
            reportScheduler = &sReportScheduler;
        }

        // Session Keystore injection
        this->sessionKeystore = &sSessionKeystore;

        // Group Data provider injection
        sGroupDataProvider.SetStorageDelegate(this->persistentStorageDelegate);
        sGroupDataProvider.SetSessionKeystore(this->sessionKeystore);
        ReturnErrorOnFailure(sGroupDataProvider.Init());
        this->groupDataProvider = &sGroupDataProvider;

#if CHIP_CONFIG_ENABLE_SESSION_RESUMPTION
        ReturnErrorOnFailure(sSessionResumptionStorage.Init(this->persistentStorageDelegate));
        this->sessionResumptionStorage = &sSessionResumptionStorage;
#else
        this->sessionResumptionStorage = nullptr;
#endif

        // Inject access control delegate
        this->accessDelegate = Access::Examples::GetAccessControlDelegate();

        // Inject ACL storage. (Don't initialize it.)
        this->aclStorage = &sAclStorage;

#if CHIP_CONFIG_PERSIST_SUBSCRIPTIONS
        ChipLogProgress(AppServer, "Initializing subscription resumption storage...");
        ReturnErrorOnFailure(sSubscriptionResumptionStorage.Init(this->persistentStorageDelegate));
        this->subscriptionResumptionStorage = &sSubscriptionResumptionStorage;
#else
        ChipLogProgress(AppServer, "Subscription persistence not supported");
#endif

#if CHIP_CONFIG_ENABLE_ICD_CIP
        if (this->icdCheckInBackOffStrategy == nullptr)
        {
            this->icdCheckInBackOffStrategy = &sDefaultICDCheckInBackOffStrategy;
        }
#endif

        return CHIP_NO_ERROR;
    }

private:
    static KvsPersistentStorageDelegate sKvsPersistenStorageDelegate;
    static PersistentStorageOperationalKeystore sPersistentStorageOperationalKeystore;
    static Credentials::PersistentStorageOpCertStore sPersistentStorageOpCertStore;
    static Credentials::GroupDataProviderImpl sGroupDataProvider;
    static chip::app::DefaultTimerDelegate sTimerDelegate;
    static app::reporting::ReportSchedulerImpl sReportScheduler;

#if CHIP_CONFIG_ENABLE_SESSION_RESUMPTION
    static SimpleSessionResumptionStorage sSessionResumptionStorage;
#endif
#if CHIP_CONFIG_PERSIST_SUBSCRIPTIONS
    static app::SimpleSubscriptionResumptionStorage sSubscriptionResumptionStorage;
#endif
    static app::DefaultAclStorage sAclStorage;
    static Crypto::DefaultSessionKeystore sSessionKeystore;
#if CHIP_CONFIG_ENABLE_ICD_CIP
    static app::DefaultICDCheckInBackOffStrategy sDefaultICDCheckInBackOffStrategy;
#endif
};

} // namespace chip
