// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// Enclosing macro to prevent multiple inclusion
#ifndef __AF_GEN_EVENT__
#define __AF_GEN_EVENT__


// Code used to configure the cluster event mechanism
#define EMBER_AF_GENERATED_EVENT_CODE \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl1; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl2; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl3; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl4; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl5; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl6; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl7; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl8; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl9; \
  EmberEventControl emberAfIdentifyClusterServerTickCallbackControl10; \
  extern EmberEventControl TimerCheckNetworkStartupControl; \
  extern EmberEventControl TimerProcessDataControl; \
  extern EmberEventControl TimerReadDHT11Control; \
  extern EmberEventControl TimerSendDataToCoordinatorControl; \
  extern EmberEventControl commissioningLedEventControl; \
  extern EmberEventControl emberAfPluginFindAndBindInitiatorCheckTargetResponsesEventControl; \
  extern EmberEventControl emberAfPluginInterpanFragmentReceiveEventControl; \
  extern EmberEventControl emberAfPluginInterpanFragmentTransmitEventControl; \
  extern EmberEventControl emberAfPluginNetworkSteeringFinishSteeringEventControl; \
  extern EmberEventControl emberAfPluginReportingTickEventControl; \
  extern EmberEventControl emberAfPluginScanDispatchScanEventControl; \
  extern EmberEventControl emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl; \
  extern EmberEventControl findingAndBindingEventControl; \
  extern EmberEventControl joinNetworkEventControl; \
  extern EmberEventControl mainStateEventControl; \
  extern EmberEventControl pirDetechEventControl; \
  extern EmberEventControl timerCheckNetworkControl; \
  extern EmberEventControl timerResetSystemControl; \
  extern void TimerCheckNetworkStartupHandler(void); \
  extern void TimerProcessDataHandler(void); \
  extern void TimerReadDHT11Handler(void); \
  extern void TimerSendDataToCoordinatorHandler(void); \
  extern void commissioningLedEventHandler(void); \
  extern void emberAfPluginFindAndBindInitiatorCheckTargetResponsesEventHandler(void); \
  extern void emberAfPluginInterpanFragmentReceiveEventHandler(void); \
  extern void emberAfPluginInterpanFragmentTransmitEventHandler(void); \
  extern void emberAfPluginNetworkSteeringFinishSteeringEventHandler(void); \
  extern void emberAfPluginReportingTickEventHandler(void); \
  extern void emberAfPluginScanDispatchScanEventHandler(void); \
  extern void emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler(void); \
  extern void findingAndBindingEventHandler(void); \
  extern void joinNetworkEventHandler(void); \
  extern void mainStateEventHandler(void); \
  extern void pirDetechEventHandle(void); \
  extern void timerCheckNetworkHandler(void); \
  extern void timerResetSystemHandler(void); \
  static void clusterTickWrapper(EmberEventControl *control, EmberAfTickFunction callback, uint8_t endpoint) \
  { \
    emberAfPushEndpointNetworkIndex(endpoint); \
    emberEventControlSetInactive(*control); \
    (*callback)(endpoint); \
    emberAfPopNetworkIndex(); \
  } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction1(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl1, emberAfIdentifyClusterServerTickCallback, 1); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction2(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl2, emberAfIdentifyClusterServerTickCallback, 2); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction3(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl3, emberAfIdentifyClusterServerTickCallback, 3); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction4(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl4, emberAfIdentifyClusterServerTickCallback, 4); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction5(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl5, emberAfIdentifyClusterServerTickCallback, 5); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction6(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl6, emberAfIdentifyClusterServerTickCallback, 6); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction7(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl7, emberAfIdentifyClusterServerTickCallback, 7); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction8(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl8, emberAfIdentifyClusterServerTickCallback, 8); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction9(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl9, emberAfIdentifyClusterServerTickCallback, 9); } \
  void emberAfIdentifyClusterServerTickCallbackWrapperFunction10(void) { clusterTickWrapper(&emberAfIdentifyClusterServerTickCallbackControl10, emberAfIdentifyClusterServerTickCallback, 10); } \


// EmberEventData structs used to populate the EmberEventData table
#define EMBER_AF_GENERATED_EVENTS   \
  { &emberAfIdentifyClusterServerTickCallbackControl1, emberAfIdentifyClusterServerTickCallbackWrapperFunction1 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl2, emberAfIdentifyClusterServerTickCallbackWrapperFunction2 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl3, emberAfIdentifyClusterServerTickCallbackWrapperFunction3 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl4, emberAfIdentifyClusterServerTickCallbackWrapperFunction4 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl5, emberAfIdentifyClusterServerTickCallbackWrapperFunction5 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl6, emberAfIdentifyClusterServerTickCallbackWrapperFunction6 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl7, emberAfIdentifyClusterServerTickCallbackWrapperFunction7 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl8, emberAfIdentifyClusterServerTickCallbackWrapperFunction8 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl9, emberAfIdentifyClusterServerTickCallbackWrapperFunction9 }, \
  { &emberAfIdentifyClusterServerTickCallbackControl10, emberAfIdentifyClusterServerTickCallbackWrapperFunction10 }, \
  { &TimerCheckNetworkStartupControl, TimerCheckNetworkStartupHandler }, \
  { &TimerProcessDataControl, TimerProcessDataHandler }, \
  { &TimerReadDHT11Control, TimerReadDHT11Handler }, \
  { &TimerSendDataToCoordinatorControl, TimerSendDataToCoordinatorHandler }, \
  { &commissioningLedEventControl, commissioningLedEventHandler }, \
  { &emberAfPluginFindAndBindInitiatorCheckTargetResponsesEventControl, emberAfPluginFindAndBindInitiatorCheckTargetResponsesEventHandler }, \
  { &emberAfPluginInterpanFragmentReceiveEventControl, emberAfPluginInterpanFragmentReceiveEventHandler }, \
  { &emberAfPluginInterpanFragmentTransmitEventControl, emberAfPluginInterpanFragmentTransmitEventHandler }, \
  { &emberAfPluginNetworkSteeringFinishSteeringEventControl, emberAfPluginNetworkSteeringFinishSteeringEventHandler }, \
  { &emberAfPluginReportingTickEventControl, emberAfPluginReportingTickEventHandler }, \
  { &emberAfPluginScanDispatchScanEventControl, emberAfPluginScanDispatchScanEventHandler }, \
  { &emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventControl, emberAfPluginUpdateTcLinkKeyBeginTcLinkKeyUpdateEventHandler }, \
  { &findingAndBindingEventControl, findingAndBindingEventHandler }, \
  { &joinNetworkEventControl, joinNetworkEventHandler }, \
  { &mainStateEventControl, mainStateEventHandler }, \
  { &pirDetechEventControl, pirDetechEventHandle }, \
  { &timerCheckNetworkControl, timerCheckNetworkHandler }, \
  { &timerResetSystemControl, timerResetSystemHandler }, \


#define EMBER_AF_GENERATED_EVENT_STRINGS   \
  "Identify Cluster Server EP 1",  \
  "Identify Cluster Server EP 2",  \
  "Identify Cluster Server EP 3",  \
  "Identify Cluster Server EP 4",  \
  "Identify Cluster Server EP 5",  \
  "Identify Cluster Server EP 6",  \
  "Identify Cluster Server EP 7",  \
  "Identify Cluster Server EP 8",  \
  "Identify Cluster Server EP 9",  \
  "Identify Cluster Server EP 10",  \
  "Event data",  \
  "Timer process data control",  \
  "Timer read d h t11 control",  \
  "Timer send data to coordinator control",  \
  "Commissioning led event control",  \
  "Find and Bind Initiator Plugin CheckTargetResponses",  \
  "Interpan Plugin FragmentReceive",  \
  "Interpan Plugin FragmentTransmit",  \
  "Network Steering Plugin FinishSteering",  \
  "Reporting Plugin Tick",  \
  "Scan Dispatch Plugin Scan",  \
  "Update TC Link Key Plugin BeginTcLinkKeyUpdate",  \
  "Finding and binding event control",  \
  "Join network event control",  \
  "Main state event control",  \
  "Pir detech event control",  \
  "Timer check network control",  \
  "Timer reset system control",  \


// The length of the event context table used to track and retrieve cluster events
#define EMBER_AF_EVENT_CONTEXT_LENGTH 10

// EmberAfEventContext structs used to populate the EmberAfEventContext table
#define EMBER_AF_GENERATED_EVENT_CONTEXT { 0x1, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl1}, \
{ 0x2, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl2}, \
{ 0x3, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl3}, \
{ 0x4, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl4}, \
{ 0x5, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl5}, \
{ 0x6, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl6}, \
{ 0x7, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl7}, \
{ 0x8, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl8}, \
{ 0x9, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl9}, \
{ 0xa, 0x3, false, EMBER_AF_LONG_POLL, EMBER_AF_OK_TO_SLEEP, &emberAfIdentifyClusterServerTickCallbackControl10}


#endif // __AF_GEN_EVENT__
